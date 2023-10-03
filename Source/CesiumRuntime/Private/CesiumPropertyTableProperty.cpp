// Copyright 2020-2023 CesiumGS, Inc. and Contributors

#include "CesiumPropertyTableProperty.h"
#include "CesiumGltf/PropertyTypeTraits.h"
#include "CesiumMetadataConversions.h"
#include <utility>

using namespace CesiumGltf;

namespace {
/**
 * Callback on a std::any, assuming that it contains a PropertyTablePropertyView
 * on a scalar type. If the valueType does not have a valid component type, the
 * callback is performed on an invalid PropertyTablePropertyView instead.
 *
 * @param property The std::any containing the property.
 * @param valueType The FCesiumMetadataValueType of the property.
 * @param callback The callback function.
 *
 * @tparam Normalized Whether the PropertyTablePropertyView is normalized.
 * @tparam TResult The type of the output from the callback function.
 * @tparam Callback The callback function type.
 */
template <bool Normalized, typename TResult, typename Callback>
TResult scalarPropertyTablePropertyCallback(
    std::any property,
    FCesiumMetadataValueType valueType,
    Callback&& callback) {
  switch (valueType.ComponentType) {
  case ECesiumMetadataComponentType::Int8:
    return callback(
        std::any_cast<PropertyTablePropertyView<int8, Normalized>>(property));
  case ECesiumMetadataComponentType::Uint8:
    return callback(
        std::any_cast<PropertyTablePropertyView<uint8, Normalized>>(property));
  case ECesiumMetadataComponentType::Int16:
    return callback(
        std::any_cast<PropertyTablePropertyView<int16, Normalized>>(property));
  case ECesiumMetadataComponentType::Uint16:
    return callback(
        std::any_cast<PropertyTablePropertyView<uint16, Normalized>>(property));
  case ECesiumMetadataComponentType::Int32:
    return callback(
        std::any_cast<PropertyTablePropertyView<int32, Normalized>>(property));
  case ECesiumMetadataComponentType::Uint32:
    return callback(
        std::any_cast<PropertyTablePropertyView<uint32, Normalized>>(property));
  case ECesiumMetadataComponentType::Int64:
    return callback(
        std::any_cast<PropertyTablePropertyView<int64, Normalized>>(property));
  case ECesiumMetadataComponentType::Uint64:
    return callback(
        std::any_cast<PropertyTablePropertyView<uint64, Normalized>>(property));
  case ECesiumMetadataComponentType::Float32:
    return callback(std::any_cast<PropertyTablePropertyView<float>>(property));
  case ECesiumMetadataComponentType::Float64:
    return callback(std::any_cast<PropertyTablePropertyView<double>>(property));
  default:
    return callback(PropertyTablePropertyView<uint8>());
  }
}

/**
 * Callback on a std::any, assuming that it contains a PropertyTablePropertyView
 * on a scalar array type. If the valueType does not have a valid component
 * type, the callback is performed on an invalid PropertyTablePropertyView
 * instead.
 *
 * @param property The std::any containing the property.
 * @param valueType The FCesiumMetadataValueType of the property.
 * @param callback The callback function.
 *
 * @tparam Normalized Whether the PropertyTablePropertyView is normalized.
 * @tparam TResult The type of the output from the callback function.
 * @tparam Callback The callback function type.
 */
template <bool Normalized, typename TResult, typename Callback>
TResult scalarArrayPropertyTablePropertyCallback(
    std::any property,
    FCesiumMetadataValueType valueType,
    Callback&& callback) {
  switch (valueType.ComponentType) {
  case ECesiumMetadataComponentType::Int8:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<PropertyArrayView<int8>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Uint8:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<PropertyArrayView<uint8>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Int16:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<PropertyArrayView<int16>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Uint16:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<PropertyArrayView<uint16>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Int32:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<PropertyArrayView<int32>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Uint32:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<PropertyArrayView<uint32>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Int64:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<PropertyArrayView<int64>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Uint64:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<PropertyArrayView<uint64>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Float32:
    return callback(
        std::any_cast<PropertyTablePropertyView<PropertyArrayView<float>>>(
            property));
  case ECesiumMetadataComponentType::Float64:
    return callback(
        std::any_cast<PropertyTablePropertyView<PropertyArrayView<double>>>(
            property));
  default:
    return callback(PropertyTablePropertyView<uint8>());
  }
}

/**
 * Callback on a std::any, assuming that it contains a PropertyTablePropertyView
 * on a glm::vecN type. If the valueType does not have a valid component type,
 * the callback is performed on an invalid PropertyTablePropertyView instead.
 *
 * @param property The std::any containing the property.
 * @param valueType The FCesiumMetadataValueType of the property.
 * @param callback The callback function.
 *
 * @tparam N The dimensions of the glm::vecN
 * @tparam Normalized Whether the PropertyTablePropertyView is normalized.
 * @tparam TResult The type of the output from the callback function.
 * @tparam Callback The callback function type.
 */
template <glm::length_t N, bool Normalized, typename TResult, typename Callback>
TResult vecNPropertyTablePropertyCallback(
    std::any property,
    FCesiumMetadataValueType valueType,
    Callback&& callback) {
  switch (valueType.ComponentType) {
  case ECesiumMetadataComponentType::Int8:
    return callback(
        std::any_cast<PropertyTablePropertyView<glm::vec<N, int8>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Uint8:
    return callback(std::any_cast<
                    PropertyTablePropertyView<glm::vec<N, uint8>, Normalized>>(
        property));
  case ECesiumMetadataComponentType::Int16:
    return callback(std::any_cast<
                    PropertyTablePropertyView<glm::vec<N, int16>, Normalized>>(
        property));
  case ECesiumMetadataComponentType::Uint16:
    return callback(std::any_cast<
                    PropertyTablePropertyView<glm::vec<N, uint16>, Normalized>>(
        property));
  case ECesiumMetadataComponentType::Int32:
    return callback(std::any_cast<
                    PropertyTablePropertyView<glm::vec<N, int32>, Normalized>>(
        property));
  case ECesiumMetadataComponentType::Uint32:
    return callback(std::any_cast<
                    PropertyTablePropertyView<glm::vec<N, uint32>, Normalized>>(
        property));
  case ECesiumMetadataComponentType::Int64:
    return callback(std::any_cast<
                    PropertyTablePropertyView<glm::vec<N, int64>, Normalized>>(
        property));
  case ECesiumMetadataComponentType::Uint64:
    return callback(std::any_cast<
                    PropertyTablePropertyView<glm::vec<N, uint64>, Normalized>>(
        property));
  case ECesiumMetadataComponentType::Float32:
    return callback(
        std::any_cast<PropertyTablePropertyView<glm::vec<N, float>>>(property));
  case ECesiumMetadataComponentType::Float64:
    return callback(
        std::any_cast<PropertyTablePropertyView<glm::vec<N, double>>>(
            property));
  default:
    return callback(PropertyTablePropertyView<uint8>());
  }
}

/**
 * Callback on a std::any, assuming that it contains a PropertyTablePropertyView
 * on a glm::vecN type. If the valueType does not have a valid component type,
 * the callback is performed on an invalid PropertyTablePropertyView instead.
 *
 * @param property The std::any containing the property.
 * @param valueType The FCesiumMetadataValueType of the property.
 * @param callback The callback function.
 *
 * @tparam Normalized Whether the PropertyTablePropertyView is normalized.
 * @tparam TResult The type of the output from the callback function.
 * @tparam Callback The callback function type.
 */
template <bool Normalized, typename TResult, typename Callback>
TResult vecNPropertyTablePropertyCallback(
    std::any property,
    FCesiumMetadataValueType valueType,
    Callback&& callback) {
  if (valueType.Type == ECesiumMetadataType::Vec2) {
    return vecNPropertyTablePropertyCallback<2, Normalized, TResult, Callback>(
        property,
        valueType,
        std::forward<Callback>(callback));
  }

  if (valueType.Type == ECesiumMetadataType::Vec3) {
    return vecNPropertyTablePropertyCallback<3, Normalized, TResult, Callback>(
        property,
        valueType,
        std::forward<Callback>(callback));
  }

  if (valueType.Type == ECesiumMetadataType::Vec4) {
    return vecNPropertyTablePropertyCallback<4, Normalized, TResult, Callback>(
        property,
        valueType,
        std::forward<Callback>(callback));
  }

  return callback(PropertyTablePropertyView<uint8>());
}

/**
 * Callback on a std::any, assuming that it contains a PropertyTablePropertyView
 * on a glm::vecN array type. If the valueType does not have a valid component
 * type, the callback is performed on an invalid PropertyTablePropertyView
 * instead.
 *
 * @param property The std::any containing the property.
 * @param valueType The FCesiumMetadataValueType of the property.
 * @param callback The callback function.
 *
 * @tparam N The dimensions of the glm::vecN
 * @tparam Normalized Whether the PropertyTablePropertyView is normalized.
 * @tparam TResult The type of the output from the callback function.
 * @tparam Callback The callback function type.
 */
template <glm::length_t N, bool Normalized, typename TResult, typename Callback>
TResult vecNArrayPropertyTablePropertyCallback(
    std::any property,
    FCesiumMetadataValueType valueType,
    Callback&& callback) {
  switch (valueType.ComponentType) {
  case ECesiumMetadataComponentType::Int8:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::vec<N, int8>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Uint8:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::vec<N, uint8>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Int16:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::vec<N, int16>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Uint16:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::vec<N, uint16>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Int32:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::vec<N, int32>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Uint32:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::vec<N, uint32>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Int64:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::vec<N, int64>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Uint64:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::vec<N, uint64>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Float32:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<PropertyArrayView<glm::vec<N, float>>>>(
            property));
  case ECesiumMetadataComponentType::Float64:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<PropertyArrayView<glm::vec<N, double>>>>(
            property));
  default:
    return callback(PropertyTablePropertyView<uint8>());
  }
}

/**
 * Callback on a std::any, assuming that it contains a PropertyTablePropertyView
 * on a glm::vecN array type. If the valueType does not have a valid component
 * type, the callback is performed on an invalid PropertyTablePropertyView
 * instead.
 *
 * @param property The std::any containing the property.
 * @param valueType The FCesiumMetadataValueType of the property.
 * @param callback The callback function.
 *
 * @tparam Normalized Whether the PropertyTablePropertyView is normalized.
 * @tparam TResult The type of the output from the callback function.
 * @tparam Callback The callback function type.
 */
template <bool Normalized, typename TResult, typename Callback>
TResult vecNArrayPropertyTablePropertyCallback(
    std::any property,
    FCesiumMetadataValueType valueType,
    Callback&& callback) {
  if (valueType.Type == ECesiumMetadataType::Vec2) {
    return vecNArrayPropertyTablePropertyCallback<
        2,
        Normalized,
        TResult,
        Callback>(property, valueType, std::forward<Callback>(callback));
  }

  if (valueType.Type == ECesiumMetadataType::Vec3) {
    return vecNArrayPropertyTablePropertyCallback<
        3,
        Normalized,
        TResult,
        Callback>(property, valueType, std::forward<Callback>(callback));
  }

  if (valueType.Type == ECesiumMetadataType::Vec4) {
    return vecNArrayPropertyTablePropertyCallback<
        4,
        Normalized,
        TResult,
        Callback>(property, valueType, std::forward<Callback>(callback));
  }

  return callback(PropertyTablePropertyView<uint8>());
}

/**
 * Callback on a std::any, assuming that it contains a PropertyTablePropertyView
 * on a glm::matN type. If the valueType does not have a valid component type,
 * the callback is performed on an invalid PropertyTablePropertyView instead.
 *
 * @param property The std::any containing the property.
 * @param valueType The FCesiumMetadataValueType of the property.
 * @param callback The callback function.
 *
 * @tparam N The dimensions of the glm::matN
 * @tparam TNormalized Whether the PropertyTablePropertyView is normalized.
 * @tparam TResult The type of the output from the callback function.
 * @tparam Callback The callback function type.
 */
template <glm::length_t N, bool Normalized, typename TResult, typename Callback>
TResult matNPropertyTablePropertyCallback(
    std::any property,
    FCesiumMetadataValueType valueType,
    Callback&& callback) {
  switch (valueType.ComponentType) {
  case ECesiumMetadataComponentType::Int8:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<glm::mat<N, N, int8>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Uint8:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<glm::mat<N, N, uint8>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Int16:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<glm::mat<N, N, int16>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Uint16:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<glm::mat<N, N, uint16>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Int32:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<glm::mat<N, N, int32>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Uint32:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<glm::mat<N, N, uint32>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Int64:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<glm::mat<N, N, int64>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Uint64:
    return callback(
        std::any_cast<
            PropertyTablePropertyView<glm::mat<N, N, uint64>, Normalized>>(
            property));
  case ECesiumMetadataComponentType::Float32:
    return callback(
        std::any_cast<PropertyTablePropertyView<glm::mat<N, N, float>>>(
            property));
  case ECesiumMetadataComponentType::Float64:
    return callback(
        std::any_cast<PropertyTablePropertyView<glm::mat<N, N, double>>>(
            property));
  default:
    return callback(PropertyTablePropertyView<uint8>());
  }
}

/**
 * Callback on a std::any, assuming that it contains a PropertyTablePropertyView
 * on a glm::matN type. This restricts the number of typeid checks done on the
 * std::any. If the valueType does not have a valid component type, the callback
 * is performed on an invalid PropertyTablePropertyView instead.
 *
 * @param property The std::any containing the property.
 * @param valueType The FCesiumMetadataValueType of the property.
 * @param callback The callback function.
 *
 * @tparam Normalized Whether the PropertyTablePropertyView is normalized.
 * @tparam TResult The type of the output from the callback function.
 * @tparam Callback The callback function type.
 */
template <bool Normalized, typename TResult, typename Callback>
TResult matNPropertyTablePropertyCallback(
    std::any property,
    FCesiumMetadataValueType valueType,
    Callback&& callback) {
  if (valueType.Type == ECesiumMetadataType::Mat2) {
    return matNPropertyTablePropertyCallback<2, Normalized, TResult, Callback>(
        property,
        valueType,
        std::forward<Callback>(callback));
  }

  if (valueType.Type == ECesiumMetadataType::Mat3) {
    return matNPropertyTablePropertyCallback<3, Normalized, TResult, Callback>(
        property,
        valueType,
        std::forward<Callback>(callback));
  }

  if (valueType.Type == ECesiumMetadataType::Mat4) {
    return matNPropertyTablePropertyCallback<4, Normalized, TResult, Callback>(
        property,
        valueType,
        std::forward<Callback>(callback));
  }

  return callback(PropertyTablePropertyView<uint8>());
}

/**
 * Callback on a std::any, assuming that it contains a PropertyTablePropertyView
 * on a glm::matN array type. This restricts the number of typeid checks done on
 * the std::any. If the valueType does not have a valid component type, the
 * callback is performed on an invalid PropertyTablePropertyView instead.
 *
 * @param property The std::any containing the property.
 * @param valueType The FCesiumMetadataValueType of the property.
 * @param callback The callback function.
 *
 * @tparam N The dimensions of the glm::matN
 * @tparam TNormalized Whether the PropertyTablePropertyView is normalized.
 * @tparam TResult The type of the output from the callback function.
 * @tparam Callback The callback function type.
 */
template <glm::length_t N, bool Normalized, typename TResult, typename Callback>
TResult matNArrayPropertyTablePropertyCallback(
    std::any property,
    FCesiumMetadataValueType valueType,
    Callback&& callback) {
  switch (valueType.ComponentType) {
  case ECesiumMetadataComponentType::Int8:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::mat<N, N, int8>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Uint8:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::mat<N, N, uint8>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Int16:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::mat<N, N, int16>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Uint16:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::mat<N, N, uint16>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Int32:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::mat<N, N, int32>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Uint32:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::mat<N, N, uint32>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Int64:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::mat<N, N, int64>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Uint64:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::mat<N, N, uint64>>,
                        Normalized>>(property));
  case ECesiumMetadataComponentType::Float32:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::mat<N, N, float>>>>(property));
  case ECesiumMetadataComponentType::Float64:
    return callback(std::any_cast<PropertyTablePropertyView<
                        PropertyArrayView<glm::mat<N, N, double>>>>(property));
  default:
    return callback(PropertyTablePropertyView<uint8>());
  }
}

/**
 * Callback on a std::any, assuming that it contains a PropertyTablePropertyView
 * on a glm::matN array type. This restricts the number of typeid checks done on
 * the std::any. If the valueType does not have a valid component type, the
 * callback is performed on an invalid PropertyTablePropertyView instead.
 *
 * @param property The std::any containing the property.
 * @param valueType The FCesiumMetadataValueType of the property.
 * @param callback The callback function.
 *
 * @tparam Normalized Whether the PropertyTablePropertyView is normalized.
 * @tparam TResult The type of the output from the callback function.
 * @tparam Callback The callback function type.
 */
template <bool Normalized, typename TResult, typename Callback>
TResult matNArrayPropertyTablePropertyCallback(
    std::any property,
    FCesiumMetadataValueType valueType,
    Callback&& callback) {
  if (valueType.Type == ECesiumMetadataType::Mat2) {
    return matNArrayPropertyTablePropertyCallback<
        2,
        Normalized,
        TResult,
        Callback>(property, valueType, std::forward<Callback>(callback));
  }

  if (valueType.Type == ECesiumMetadataType::Mat3) {
    return matNArrayPropertyTablePropertyCallback<
        3,
        Normalized,
        TResult,
        Callback>(property, valueType, std::forward<Callback>(callback));
  }

  if (valueType.Type == ECesiumMetadataType::Mat4) {
    return matNArrayPropertyTablePropertyCallback<
        4,
        Normalized,
        TResult,
        Callback>(property, valueType, std::forward<Callback>(callback));
  }

  return callback(PropertyTablePropertyView<uint8>());
}

template <bool Normalized, typename TResult, typename Callback>
TResult arrayPropertyTablePropertyCallback(
    std::any property,
    FCesiumMetadataValueType valueType,
    Callback&& callback) {
  switch (valueType.Type) {
  case ECesiumMetadataType::Scalar:
    return scalarArrayPropertyTablePropertyCallback<
        Normalized,
        TResult,
        Callback>(property, valueType, std::forward<Callback>(callback));
  case ECesiumMetadataType::Vec2:
  case ECesiumMetadataType::Vec3:
  case ECesiumMetadataType::Vec4:
    return vecNArrayPropertyTablePropertyCallback<
        Normalized,
        TResult,
        Callback>(property, valueType, std::forward<Callback>(callback));
  case ECesiumMetadataType::Mat2:
  case ECesiumMetadataType::Mat3:
  case ECesiumMetadataType::Mat4:
    return matNArrayPropertyTablePropertyCallback<
        Normalized,
        TResult,
        Callback>(property, valueType, std::forward<Callback>(callback));
  case ECesiumMetadataType::Boolean:
    if (property.type() ==
        typeid(PropertyTablePropertyView<PropertyArrayView<bool>>))
      return callback(
          std::any_cast<PropertyTablePropertyView<PropertyArrayView<bool>>>(
              property));
    [[fallthrough]];
  case ECesiumMetadataType::String:
    if (property.type() ==
        typeid(PropertyTablePropertyView<PropertyArrayView<std::string_view>>))
      return callback(
          std::any_cast<
              PropertyTablePropertyView<PropertyArrayView<std::string_view>>>(
              property));
    [[fallthrough]];
  default:
    return callback(PropertyTablePropertyView<uint8>());
  }
}

template <typename TResult, typename Callback>
TResult propertyTablePropertyCallback(
    std::any property,
    FCesiumMetadataValueType valueType,
    bool normalized,
    Callback&& callback) {
  if (valueType.bIsArray) {
    return normalized
               ? arrayPropertyTablePropertyCallback<true, TResult, Callback>(
                     property,
                     valueType,
                     std::forward<Callback>(callback))
               : arrayPropertyTablePropertyCallback<false, TResult, Callback>(
                     property,
                     valueType,
                     std::forward<Callback>(callback));
  }

  switch (valueType.Type) {
  case ECesiumMetadataType::Scalar:
    return normalized
               ? scalarPropertyTablePropertyCallback<true, TResult, Callback>(
                     property,
                     valueType,
                     std::forward<Callback>(callback))
               : scalarPropertyTablePropertyCallback<false, TResult, Callback>(
                     property,
                     valueType,
                     std::forward<Callback>(callback));
  case ECesiumMetadataType::Vec2:
  case ECesiumMetadataType::Vec3:
  case ECesiumMetadataType::Vec4:
    return normalized
               ? vecNPropertyTablePropertyCallback<true, TResult, Callback>(
                     property,
                     valueType,
                     std::forward<Callback>(callback))
               : vecNPropertyTablePropertyCallback<false, TResult, Callback>(
                     property,
                     valueType,
                     std::forward<Callback>(callback));
  case ECesiumMetadataType::Mat2:
  case ECesiumMetadataType::Mat3:
  case ECesiumMetadataType::Mat4:
    return normalized
               ? matNPropertyTablePropertyCallback<true, TResult, Callback>(
                     property,
                     valueType,
                     std::forward<Callback>(callback))
               : matNPropertyTablePropertyCallback<false, TResult, Callback>(
                     property,
                     valueType,
                     std::forward<Callback>(callback));
  case ECesiumMetadataType::Boolean:
    if (property.type() == typeid(PropertyTablePropertyView<bool>))
      return callback(std::any_cast<PropertyTablePropertyView<bool>>(property));
    [[fallthrough]];
  case ECesiumMetadataType::String:
    if (property.type() == typeid(PropertyTablePropertyView<std::string_view>))
      return callback(
          std::any_cast<PropertyTablePropertyView<std::string_view>>(property));
    [[fallthrough]];
  default:
    return callback(PropertyTablePropertyView<uint8>());
  }
}

} // namespace

ECesiumPropertyTablePropertyStatus
UCesiumPropertyTablePropertyBlueprintLibrary::GetPropertyTablePropertyStatus(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return Property._status;
}

ECesiumMetadataBlueprintType
UCesiumPropertyTablePropertyBlueprintLibrary::GetBlueprintType(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return CesiumMetadataValueTypeToBlueprintType(Property._valueType);
}

ECesiumMetadataBlueprintType
UCesiumPropertyTablePropertyBlueprintLibrary::GetArrayElementBlueprintType(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  if (!Property._valueType.bIsArray) {
    return ECesiumMetadataBlueprintType::None;
  }

  FCesiumMetadataValueType valueType(Property._valueType);
  valueType.bIsArray = false;

  return CesiumMetadataValueTypeToBlueprintType(valueType);
}

FCesiumMetadataValueType
UCesiumPropertyTablePropertyBlueprintLibrary::GetValueType(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return Property._valueType;
}

int64 UCesiumPropertyTablePropertyBlueprintLibrary::GetPropertySize(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return propertyTablePropertyCallback<int64>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [](const auto& view) -> int64 { return view.size(); });
}

int64 UCesiumPropertyTablePropertyBlueprintLibrary::GetArraySize(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return propertyTablePropertyCallback<int64>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [](const auto& view) -> int64 { return view.arrayCount(); });
}

bool UCesiumPropertyTablePropertyBlueprintLibrary::GetBoolean(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID,
    bool DefaultValue) {
  return propertyTablePropertyCallback<bool>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID, DefaultValue](const auto& v) -> bool {
        // size() returns zero if the view is invalid.
        if (FeatureID < 0 || FeatureID >= v.size()) {
          return DefaultValue;
        }
        auto maybeValue = v.get(FeatureID);
        if (maybeValue) {
          auto value = *maybeValue;
          return CesiumMetadataConversions<bool, decltype(value)>::convert(
              value,
              DefaultValue);
        }
        return DefaultValue;
      });
}

uint8 UCesiumPropertyTablePropertyBlueprintLibrary::GetByte(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID,
    uint8 DefaultValue) {
  return propertyTablePropertyCallback<uint8>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID, DefaultValue](const auto& v) -> uint8 {
        // size() returns zero if the view is invalid.
        if (FeatureID < 0 || FeatureID >= v.size()) {
          return DefaultValue;
        }
        auto maybeValue = v.get(FeatureID);
        if (maybeValue) {
          auto value = *maybeValue;
          return CesiumMetadataConversions<uint8, decltype(value)>::convert(
              value,
              DefaultValue);
        }
        return DefaultValue;
      });
}

int32 UCesiumPropertyTablePropertyBlueprintLibrary::GetInteger(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID,
    int32 DefaultValue) {
  return propertyTablePropertyCallback<int32>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID, DefaultValue](const auto& v) -> int32 {
        // size() returns zero if the view is invalid.
        if (FeatureID < 0 || FeatureID >= v.size()) {
          return DefaultValue;
        }
        auto maybeValue = v.get(FeatureID);
        if (maybeValue) {
          auto value = *maybeValue;
          return CesiumMetadataConversions<int32, decltype(value)>::convert(
              value,
              DefaultValue);
        }
        return DefaultValue;
      });
}

int64 UCesiumPropertyTablePropertyBlueprintLibrary::GetInteger64(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID,
    int64 DefaultValue) {
  return propertyTablePropertyCallback<int64>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID, DefaultValue](const auto& v) -> int64 {
        // size() returns zero if the view is invalid.
        if (FeatureID < 0 || FeatureID >= v.size()) {
          return DefaultValue;
        }
        auto maybeValue = v.get(FeatureID);
        if (maybeValue) {
          auto value = *maybeValue;
          return CesiumMetadataConversions<int64, decltype(value)>::convert(
              value,
              DefaultValue);
        }
        return DefaultValue;
      });
}

float UCesiumPropertyTablePropertyBlueprintLibrary::GetFloat(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID,
    float DefaultValue) {
  return propertyTablePropertyCallback<float>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID, DefaultValue](const auto& v) -> float {
        // size() returns zero if the view is invalid.
        if (FeatureID < 0 || FeatureID >= v.size()) {
          return DefaultValue;
        }
        auto maybeValue = v.get(FeatureID);
        if (maybeValue) {
          auto value = *maybeValue;
          return CesiumMetadataConversions<float, decltype(value)>::convert(
              value,
              DefaultValue);
        }
        return DefaultValue;
      });
}

double UCesiumPropertyTablePropertyBlueprintLibrary::GetFloat64(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID,
    double DefaultValue) {
  return propertyTablePropertyCallback<double>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID, DefaultValue](const auto& v) -> double {
        // size() returns zero if the view is invalid.
        if (FeatureID < 0 || FeatureID >= v.size()) {
          return DefaultValue;
        }
        auto maybeValue = v.get(FeatureID);
        if (maybeValue) {
          auto value = *maybeValue;
          return CesiumMetadataConversions<double, decltype(value)>::convert(
              value,
              DefaultValue);
        }
        return DefaultValue;
      });
}

FIntPoint UCesiumPropertyTablePropertyBlueprintLibrary::GetIntPoint(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID,
    const FIntPoint& DefaultValue) {
  return propertyTablePropertyCallback<FIntPoint>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID, DefaultValue](const auto& v) -> FIntPoint {
        // size() returns zero if the view is invalid.
        if (FeatureID < 0 || FeatureID >= v.size()) {
          return DefaultValue;
        }
        auto maybeValue = v.get(FeatureID);
        if (maybeValue) {
          auto value = *maybeValue;
          return CesiumMetadataConversions<FIntPoint, decltype(value)>::convert(
              value,
              DefaultValue);
        }
        return DefaultValue;
      });
}

FVector2D UCesiumPropertyTablePropertyBlueprintLibrary::GetVector2D(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID,
    const FVector2D& DefaultValue) {
  return propertyTablePropertyCallback<FVector2D>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID, DefaultValue](const auto& v) -> FVector2D {
        // size() returns zero if the view is invalid.
        if (FeatureID < 0 || FeatureID >= v.size()) {
          return DefaultValue;
        }
        auto maybeValue = v.get(FeatureID);
        if (maybeValue) {
          auto value = *maybeValue;
          return CesiumMetadataConversions<FVector2D, decltype(value)>::convert(
              value,
              DefaultValue);
        }
        return DefaultValue;
      });
}

FIntVector UCesiumPropertyTablePropertyBlueprintLibrary::GetIntVector(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID,
    const FIntVector& DefaultValue) {
  return propertyTablePropertyCallback<FIntVector>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID, DefaultValue](const auto& v) -> FIntVector {
        // size() returns zero if the view is invalid.
        if (FeatureID < 0 || FeatureID >= v.size()) {
          return DefaultValue;
        }
        auto maybeValue = v.get(FeatureID);
        if (maybeValue) {
          auto value = *maybeValue;
          return CesiumMetadataConversions<FIntVector, decltype(value)>::
              convert(value, DefaultValue);
        }
        return DefaultValue;
      });
}

FVector3f UCesiumPropertyTablePropertyBlueprintLibrary::GetVector3f(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID,
    const FVector3f& DefaultValue) {
  return propertyTablePropertyCallback<FVector3f>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID, DefaultValue](const auto& v) -> FVector3f {
        // size() returns zero if the view is invalid.
        if (FeatureID < 0 || FeatureID >= v.size()) {
          return DefaultValue;
        }
        auto maybeValue = v.get(FeatureID);
        if (maybeValue) {
          auto value = *maybeValue;
          return CesiumMetadataConversions<FVector3f, decltype(value)>::convert(
              value,
              DefaultValue);
        }
        return DefaultValue;
      });
}

FVector UCesiumPropertyTablePropertyBlueprintLibrary::GetVector(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID,
    const FVector& DefaultValue) {
  return propertyTablePropertyCallback<FVector>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID, DefaultValue](const auto& v) -> FVector {
        // size() returns zero if the view is invalid.
        if (FeatureID < 0 || FeatureID >= v.size()) {
          return DefaultValue;
        }
        auto maybeValue = v.get(FeatureID);
        if (maybeValue) {
          auto value = *maybeValue;
          return CesiumMetadataConversions<FVector, decltype(value)>::convert(
              value,
              DefaultValue);
        }
        return DefaultValue;
      });
}

FVector4 UCesiumPropertyTablePropertyBlueprintLibrary::GetVector4(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID,
    const FVector4& DefaultValue) {
  return propertyTablePropertyCallback<FVector4>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID, DefaultValue](const auto& v) -> FVector4 {
        // size() returns zero if the view is invalid.
        if (FeatureID < 0 || FeatureID >= v.size()) {
          return DefaultValue;
        }
        auto maybeValue = v.get(FeatureID);
        if (maybeValue) {
          auto value = *maybeValue;
          return CesiumMetadataConversions<FVector4, decltype(value)>::convert(
              value,
              DefaultValue);
        }
        return DefaultValue;
      });
}

FMatrix UCesiumPropertyTablePropertyBlueprintLibrary::GetMatrix(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID,
    const FMatrix& DefaultValue) {
  return propertyTablePropertyCallback<FMatrix>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID, DefaultValue](const auto& v) -> FMatrix {
        // size() returns zero if the view is invalid.
        if (FeatureID < 0 || FeatureID >= v.size()) {
          return DefaultValue;
        }
        auto maybeValue = v.get(FeatureID);
        if (maybeValue) {
          auto value = *maybeValue;
          return CesiumMetadataConversions<FMatrix, decltype(value)>::convert(
              value,
              DefaultValue);
        }
        return DefaultValue;
      });
}

FString UCesiumPropertyTablePropertyBlueprintLibrary::GetString(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID,
    const FString& DefaultValue) {
  return propertyTablePropertyCallback<FString>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID, &DefaultValue](const auto& v) -> FString {
        // size() returns zero if the view is invalid.
        if (FeatureID < 0 || FeatureID >= v.size()) {
          return DefaultValue;
        }
        auto maybeValue = v.get(FeatureID);
        if (maybeValue) {
          auto value = *maybeValue;
          return CesiumMetadataConversions<FString, decltype(value)>::convert(
              value,
              DefaultValue);
        }
        return DefaultValue;
      });
}

FCesiumPropertyArray UCesiumPropertyTablePropertyBlueprintLibrary::GetArray(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID) {
  return propertyTablePropertyCallback<FCesiumPropertyArray>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID](const auto& v) -> FCesiumPropertyArray {
        // size() returns zero if the view is invalid.
        if (FeatureID < 0 || FeatureID >= v.size()) {
          return FCesiumPropertyArray();
        }
        auto maybeValue = v.get(FeatureID);
        if (maybeValue) {
          auto value = *maybeValue;
          if constexpr (CesiumGltf::IsMetadataArray<decltype(value)>::value) {
            return FCesiumPropertyArray(value);
          }
        }
        return FCesiumPropertyArray();
      });
}

FCesiumMetadataValue UCesiumPropertyTablePropertyBlueprintLibrary::GetValue(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID) {
  return propertyTablePropertyCallback<FCesiumMetadataValue>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID](const auto& view) -> FCesiumMetadataValue {
        // size() returns zero if the view is invalid.
        if (FeatureID >= 0 && FeatureID < view.size()) {
          return FCesiumMetadataValue(view.get(FeatureID));
        }
        return FCesiumMetadataValue();
      });
}

FCesiumMetadataValue UCesiumPropertyTablePropertyBlueprintLibrary::GetRawValue(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID) {
  return propertyTablePropertyCallback<FCesiumMetadataValue>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [FeatureID](const auto& view) -> FCesiumMetadataValue {
        // Return an empty value if the property is empty.
        if (view.status() ==
            PropertyTablePropertyViewStatus::EmptyPropertyWithDefault) {
          return FCesiumMetadataValue();
        }

        // size() returns zero if the view is invalid.
        if (FeatureID >= 0 && FeatureID < view.size()) {
          return FCesiumMetadataValue(view.getRaw(FeatureID));
        }

        return FCesiumMetadataValue();
      });
}

bool UCesiumPropertyTablePropertyBlueprintLibrary::IsNormalized(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return Property._normalized;
}

FCesiumMetadataValue UCesiumPropertyTablePropertyBlueprintLibrary::GetOffset(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return propertyTablePropertyCallback<FCesiumMetadataValue>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [](const auto& view) -> FCesiumMetadataValue {
        // Returns an empty value if no offset is specified.
        return FCesiumMetadataValue(view.offset());
      });
}

FCesiumMetadataValue UCesiumPropertyTablePropertyBlueprintLibrary::GetScale(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return propertyTablePropertyCallback<FCesiumMetadataValue>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [](const auto& view) -> FCesiumMetadataValue {
        // Returns an empty value if no scale is specified.
        return FCesiumMetadataValue(view.scale());
      });
}

FCesiumMetadataValue
UCesiumPropertyTablePropertyBlueprintLibrary::GetMinimumValue(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return propertyTablePropertyCallback<FCesiumMetadataValue>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [](const auto& view) -> FCesiumMetadataValue {
        // Returns an empty value if no min is specified.
        return FCesiumMetadataValue(view.min());
      });
}

FCesiumMetadataValue
UCesiumPropertyTablePropertyBlueprintLibrary::GetMaximumValue(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return propertyTablePropertyCallback<FCesiumMetadataValue>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [](const auto& view) -> FCesiumMetadataValue {
        // Returns an empty value if no max is specified.
        return FCesiumMetadataValue(view.max());
      });
}

FCesiumMetadataValue
UCesiumPropertyTablePropertyBlueprintLibrary::GetNoDataValue(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return propertyTablePropertyCallback<FCesiumMetadataValue>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [](const auto& view) -> FCesiumMetadataValue {
        // Returns an empty value if no "no data" value is specified.
        return FCesiumMetadataValue(view.noData());
      });
}

FCesiumMetadataValue
UCesiumPropertyTablePropertyBlueprintLibrary::GetDefaultValue(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return propertyTablePropertyCallback<FCesiumMetadataValue>(
      Property._property,
      Property._valueType,
      Property._normalized,
      [](const auto& view) -> FCesiumMetadataValue {
        // Returns an empty value if no default value is specified.
        return FCesiumMetadataValue(view.defaultValue());
      });
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS

ECesiumMetadataBlueprintType
UCesiumPropertyTablePropertyBlueprintLibrary::GetBlueprintComponentType(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return UCesiumPropertyTablePropertyBlueprintLibrary::
      GetArrayElementBlueprintType(Property);
}

ECesiumMetadataTrueType_DEPRECATED
UCesiumPropertyTablePropertyBlueprintLibrary::GetTrueType(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return CesiumMetadataValueTypeToTrueType(Property._valueType);
}

ECesiumMetadataTrueType_DEPRECATED
UCesiumPropertyTablePropertyBlueprintLibrary::GetTrueComponentType(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  FCesiumMetadataValueType type = Property._valueType;
  type.bIsArray = false;
  return CesiumMetadataValueTypeToTrueType(type);
}

int64 UCesiumPropertyTablePropertyBlueprintLibrary::GetNumberOfFeatures(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return UCesiumPropertyTablePropertyBlueprintLibrary::GetPropertySize(
      Property);
}

int64 UCesiumPropertyTablePropertyBlueprintLibrary::GetComponentCount(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property) {
  return UCesiumPropertyTablePropertyBlueprintLibrary::GetArraySize(Property);
}

FCesiumMetadataValue
UCesiumPropertyTablePropertyBlueprintLibrary::GetGenericValue(
    UPARAM(ref) const FCesiumPropertyTableProperty& Property,
    int64 FeatureID) {
  return UCesiumPropertyTablePropertyBlueprintLibrary::GetValue(
      Property,
      FeatureID);
}

PRAGMA_ENABLE_DEPRECATION_WARNINGS
