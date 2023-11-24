// Copyright 2020-2021 CesiumGS, Inc. and Contributors

#include "CesiumIonSession.h"
#include "CesiumEditor.h"
#include "CesiumEditorSettings.h"
#include "CesiumIonServer.h"
#include "CesiumRuntimeSettings.h"
#include "CesiumSourceControl.h"
#include "CesiumUtility/Uri.h"
#include "HAL/PlatformProcess.h"
#include "Misc/App.h"

using namespace CesiumAsync;
using namespace CesiumIonClient;

CesiumIonSession::CesiumIonSession(
    CesiumAsync::AsyncSystem& asyncSystem,
    const std::shared_ptr<CesiumAsync::IAssetAccessor>& pAssetAccessor,
    TWeakObjectPtr<UCesiumIonServer> pServer)
    : _asyncSystem(asyncSystem),
      _pAssetAccessor(pAssetAccessor),
      _pServer(pServer),
      _connection(std::nullopt),
      _profile(std::nullopt),
      _assets(std::nullopt),
      _tokens(std::nullopt),
      _isConnecting(false),
      _isResuming(false),
      _isLoadingProfile(false),
      _isLoadingAssets(false),
      _isLoadingTokens(false),
      _loadProfileQueued(false),
      _loadAssetsQueued(false),
      _loadTokensQueued(false),
      _authorizeUrl() {}

void CesiumIonSession::connect() {
  if (!this->_pServer.IsValid() || this->isConnecting() ||
      this->isConnected() || this->isResuming()) {
    return;
  }

  UCesiumIonServer* pServer = this->_pServer.Get();

  this->_isConnecting = true;

  std::string ionServerUrl = TCHAR_TO_UTF8(*pServer->ServerUrl);

  Future<std::optional<std::string>> futureApiUrl =
      !pServer->ApiUrl.IsEmpty()
          ? this->_asyncSystem.createResolvedFuture<std::optional<std::string>>(
                TCHAR_TO_UTF8(*pServer->ApiUrl))
          : Connection::getApiUrl(
                this->_asyncSystem,
                this->_pAssetAccessor,
                ionServerUrl);

  std::shared_ptr<CesiumIonSession> thiz = this->shared_from_this();

  std::move(futureApiUrl)
      .thenInMainThread([ionServerUrl, thiz, pServer = this->_pServer](
                            std::optional<std::string>&& ionApiUrl) {
        if (!pServer.IsValid()) {
          thiz->_isConnecting = false;
          thiz->_connection = std::nullopt;
          thiz->ConnectionUpdated.Broadcast();
          return;
        }

        if (!ionApiUrl) {
          thiz->_isConnecting = false;
          thiz->_connection = std::nullopt;
          thiz->ConnectionUpdated.Broadcast();
          UE_LOG(
              LogCesiumEditor,
              Error,
              TEXT(
                  "Failed to retrieve API URL from the config.json file at the specified Ion server URL: %s"),
              UTF8_TO_TCHAR(ionServerUrl.c_str()));
          return;
        }

        if (pServer->ApiUrl.IsEmpty()) {
          pServer->ApiUrl = UTF8_TO_TCHAR(ionApiUrl->c_str());
          pServer->Modify();
        }

        int64_t clientID = pServer->OAuth2ApplicationID;

        Connection::authorize(
            thiz->_asyncSystem,
            thiz->_pAssetAccessor,
            "Cesium for Unreal",
            clientID,
            "/cesium-for-unreal/oauth2/callback",
            {"assets:list",
             "assets:read",
             "profile:read",
             "tokens:read",
             "tokens:write",
             "geocode"},
            [thiz](const std::string& url) {
              thiz->_authorizeUrl = url;

              thiz->_redirectUrl =
                  CesiumUtility::Uri::getQueryValue(url, "redirect_uri");

              FPlatformProcess::LaunchURL(
                  UTF8_TO_TCHAR(thiz->_authorizeUrl.c_str()),
                  NULL,
                  NULL);
            },
            *ionApiUrl,
            CesiumUtility::Uri::resolve(ionServerUrl, "oauth"))
            .thenInMainThread([thiz](CesiumIonClient::Connection&& connection) {
              thiz->_isConnecting = false;
              thiz->_connection = std::move(connection);

              UCesiumEditorSettings* pSettings =
                  GetMutableDefault<UCesiumEditorSettings>();
              pSettings->UserAccessTokenMap.Add(
                  thiz->_pServer.Get(),
                  UTF8_TO_TCHAR(
                      thiz->_connection.value().getAccessToken().c_str()));
              pSettings->Save();

              thiz->ConnectionUpdated.Broadcast();
            })
            .catchInMainThread([thiz](std::exception&& e) {
              thiz->_isConnecting = false;
              thiz->_connection = std::nullopt;
              thiz->ConnectionUpdated.Broadcast();
            });
      });
}

void CesiumIonSession::resume() {
  if (!this->_pServer.IsValid() || this->isConnecting() ||
      this->isConnected() || this->isResuming()) {
    return;
  }

  const UCesiumEditorSettings* pSettings = GetDefault<UCesiumEditorSettings>();
  const FString* pUserAccessToken =
      pSettings->UserAccessTokenMap.Find(this->_pServer.Get());

  if (!pUserAccessToken || pUserAccessToken->IsEmpty()) {
    // No existing session to resume.
    return;
  }

  this->_isResuming = true;

  this->_connection = Connection(
      this->_asyncSystem,
      this->_pAssetAccessor,
      TCHAR_TO_UTF8(**pUserAccessToken),
      TCHAR_TO_UTF8(*this->_pServer->ApiUrl));

  std::shared_ptr<CesiumIonSession> thiz = this->shared_from_this();

  // Verify that the connection actually works.
  this->_connection.value()
      .me()
      .thenInMainThread([thiz](Response<Profile>&& response) {
        if (!response.value.has_value()) {
          thiz->_connection.reset();
        }
        thiz->_isResuming = false;
        thiz->ConnectionUpdated.Broadcast();
      })
      .catchInMainThread([thiz](std::exception&& e) {
        thiz->_isResuming = false;
        thiz->_connection.reset();
      });
}

void CesiumIonSession::disconnect() {
  this->_connection.reset();
  this->_profile.reset();
  this->_assets.reset();
  this->_tokens.reset();

  UCesiumEditorSettings* pSettings = GetMutableDefault<UCesiumEditorSettings>();
  pSettings->UserAccessTokenMap.Remove(this->_pServer.Get());
  pSettings->Save();

  this->ConnectionUpdated.Broadcast();
  this->ProfileUpdated.Broadcast();
  this->AssetsUpdated.Broadcast();
  this->TokensUpdated.Broadcast();
}

void CesiumIonSession::refreshProfile() {
  if (!this->_connection || this->_isLoadingProfile) {
    this->_loadProfileQueued = true;
    return;
  }

  this->_isLoadingProfile = true;
  this->_loadProfileQueued = false;

  std::shared_ptr<CesiumIonSession> thiz = this->shared_from_this();

  this->_connection->me()
      .thenInMainThread([thiz](Response<Profile>&& profile) {
        thiz->_isLoadingProfile = false;
        thiz->_profile = std::move(profile.value);
        thiz->ProfileUpdated.Broadcast();
        thiz->refreshProfileIfNeeded();
      })
      .catchInMainThread([thiz](std::exception&& e) {
        thiz->_isLoadingProfile = false;
        thiz->_profile = std::nullopt;
        thiz->ProfileUpdated.Broadcast();
        thiz->refreshProfileIfNeeded();
      });
}

void CesiumIonSession::refreshAssets() {
  if (!this->_connection || this->_isLoadingAssets) {
    return;
  }

  this->_isLoadingAssets = true;
  this->_loadAssetsQueued = false;

  std::shared_ptr<CesiumIonSession> thiz = this->shared_from_this();

  this->_connection->assets()
      .thenInMainThread([thiz](Response<Assets>&& assets) {
        thiz->_isLoadingAssets = false;
        thiz->_assets = std::move(assets.value);
        thiz->AssetsUpdated.Broadcast();
        thiz->refreshAssetsIfNeeded();
      })
      .catchInMainThread([thiz](std::exception&& e) {
        thiz->_isLoadingAssets = false;
        thiz->_assets = std::nullopt;
        thiz->AssetsUpdated.Broadcast();
        thiz->refreshAssetsIfNeeded();
      });
}

void CesiumIonSession::refreshTokens() {
  if (!this->_connection || this->_isLoadingTokens) {
    return;
  }

  this->_isLoadingTokens = true;
  this->_loadTokensQueued = false;

  std::shared_ptr<CesiumIonSession> thiz = this->shared_from_this();

  this->_connection->tokens()
      .thenInMainThread([thiz](Response<TokenList>&& tokens) {
        thiz->_isLoadingTokens = false;
        thiz->_tokens = tokens.value
                            ? std::make_optional(std::move(tokens.value->items))
                            : std::nullopt;
        thiz->TokensUpdated.Broadcast();
        thiz->refreshTokensIfNeeded();
      })
      .catchInMainThread([thiz](std::exception&& e) {
        thiz->_isLoadingTokens = false;
        thiz->_tokens = std::nullopt;
        thiz->TokensUpdated.Broadcast();
        thiz->refreshTokensIfNeeded();
      });
}

const std::optional<CesiumIonClient::Connection>&
CesiumIonSession::getConnection() const {
  return this->_connection;
}

const CesiumIonClient::Profile& CesiumIonSession::getProfile() {
  static const CesiumIonClient::Profile empty{};
  if (this->_profile) {
    return *this->_profile;
  } else {
    this->refreshProfile();
    return empty;
  }
}

const CesiumIonClient::Assets& CesiumIonSession::getAssets() {
  static const CesiumIonClient::Assets empty;
  if (this->_assets) {
    return *this->_assets;
  } else {
    this->refreshAssets();
    return empty;
  }
}

const std::vector<CesiumIonClient::Token>& CesiumIonSession::getTokens() {
  static const std::vector<CesiumIonClient::Token> empty;
  if (this->_tokens) {
    return *this->_tokens;
  } else {
    this->refreshTokens();
    return empty;
  }
}

bool CesiumIonSession::refreshProfileIfNeeded() {
  if (this->_loadProfileQueued || !this->_profile.has_value()) {
    this->refreshProfile();
  }
  return this->isProfileLoaded();
}

bool CesiumIonSession::refreshAssetsIfNeeded() {
  if (this->_loadAssetsQueued || !this->_assets.has_value()) {
    this->refreshAssets();
  }
  return this->isAssetListLoaded();
}

bool CesiumIonSession::refreshTokensIfNeeded() {
  if (this->_loadTokensQueued || !this->_tokens.has_value()) {
    this->refreshTokens();
  }
  return this->isTokenListLoaded();
}

Future<Response<Token>>
CesiumIonSession::findToken(const FString& token) const {
  if (!this->_connection) {
    return this->getAsyncSystem().createResolvedFuture(
        Response<Token>(0, "NOTCONNECTED", "Not connected to Cesium ion."));
  }

  std::string tokenString = TCHAR_TO_UTF8(*token);
  std::optional<std::string> maybeTokenID =
      Connection::getIdFromToken(tokenString);

  if (!maybeTokenID) {
    return this->getAsyncSystem().createResolvedFuture(
        Response<Token>(0, "INVALIDTOKEN", "The token is not valid."));
  }

  return this->_connection->token(*maybeTokenID);
}

namespace {

Token tokenFromSettings() {
  Token result;
  result.token = TCHAR_TO_UTF8(
      *GetDefault<UCesiumRuntimeSettings>()->DefaultIonAccessToken);
  return result;
}

Future<Token> getTokenFuture(const CesiumIonSession& session) {
  if (!GetDefault<UCesiumRuntimeSettings>()
           ->DefaultIonAccessTokenId.IsEmpty()) {
    return session.getConnection()
        ->token(TCHAR_TO_UTF8(
            *GetDefault<UCesiumRuntimeSettings>()->DefaultIonAccessTokenId))
        .thenImmediately([](Response<Token>&& tokenResponse) {
          if (tokenResponse.value) {
            return *tokenResponse.value;
          } else {
            return tokenFromSettings();
          }
        });
  } else if (!GetDefault<UCesiumRuntimeSettings>()
                  ->DefaultIonAccessToken.IsEmpty()) {
    return session
        .findToken(GetDefault<UCesiumRuntimeSettings>()->DefaultIonAccessToken)
        .thenImmediately([](Response<Token>&& response) {
          if (response.value) {
            return *response.value;
          } else {
            return tokenFromSettings();
          }
        });
  } else {
    return session.getAsyncSystem().createResolvedFuture(tokenFromSettings());
  }
}

} // namespace

SharedFuture<Token> CesiumIonSession::getProjectDefaultTokenDetails() {
  if (this->_projectDefaultTokenDetailsFuture) {
    // If the future is resolved but its token doesn't match the designated
    // default token, do the request again because the user probably specified a
    // new token.
    if (this->_projectDefaultTokenDetailsFuture->isReady() &&
        this->_projectDefaultTokenDetailsFuture->wait().token !=
            TCHAR_TO_UTF8(
                *GetDefault<UCesiumRuntimeSettings>()->DefaultIonAccessToken)) {
      this->_projectDefaultTokenDetailsFuture.reset();
    } else {
      return *this->_projectDefaultTokenDetailsFuture;
    }
  }

  if (!this->isConnected()) {
    return this->getAsyncSystem()
        .createResolvedFuture(tokenFromSettings())
        .share();
  }

  this->_projectDefaultTokenDetailsFuture = getTokenFuture(*this).share();
  return *this->_projectDefaultTokenDetailsFuture;
}

void CesiumIonSession::invalidateProjectDefaultTokenDetails() {
  this->_projectDefaultTokenDetailsFuture.reset();
}
