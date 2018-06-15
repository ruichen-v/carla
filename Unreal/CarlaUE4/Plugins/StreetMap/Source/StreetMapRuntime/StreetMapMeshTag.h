// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

/// Tag to identify the meshes used by the ProceduralMapGenerator.
///
/// It will work as long as we have less than 255 meshes, currently blueprint
/// type enums support uint8 only.
UENUM(BlueprintType)
enum class EStreetMapMeshTag : uint8
{
  Road_3_Lanes_OneWay            UMETA(DisplayName = "Road: Three Lanes - One Way"),
  Road_2_Lanes_OneWay            UMETA(DisplayName = "Road: Two Lanes - One Way"),
  Road_2_Lanes_OneWay_Plain      UMETA(DisplayName = "Road: Two Lanes - One Way - Plain"),
  Road_2_Lanes_DoubleWay         UMETA(DisplayName = "Road: Two Lanes - Double Way"),
  Road_1_Lane_DoubleWay_Solid    UMETA(DisplayName = "Road: One Lane - Double Way - Solid Lane Mark"),
  Road_1_Lane_DoubleWay_Dashed   UMETA(DisplayName = "Road: One Lane - Double Way - Dashed Lane Mark"),

  NUMBER_OF_TAGS                 UMETA(Hidden),
  INVALID                        UMETA(Hidden)
};

/// Helper class for working with EStreetMapMeshTag.
class StreetMapMeshTag
{
public:

  /// Return the number of tags.
  static constexpr uint8 GetNumberOfTags() {
    return ToUInt(EStreetMapMeshTag::NUMBER_OF_TAGS);
  }

  /// Return the base mesh. The base mesh defines the road width
  static EStreetMapMeshTag GetBaseMeshTag();

  // _MARK TODO discard if dynamically done in map generator
  /// Get the size in tiles of a road intersection side. I.e., return N such NxN
  /// is the size of a road intersection piece.
  static uint32 GetRoadIntersectionSize();

  /// @name Tag conversions
  /// @{

  /// Convert @a Tag to an unsigned integer type.
  static constexpr uint8 ToUInt(EStreetMapMeshTag Tag) {
    return static_cast<uint8>(Tag);
  }

  /// Convert an unsigned integer to a EStreetMapMeshTag.
  static EStreetMapMeshTag FromUInt(uint8 Value) {
    check(Value < GetNumberOfTags());
    return static_cast<EStreetMapMeshTag>(Value);
  }

  /// Get @a Tag name as FString.
  static FString ToString(EStreetMapMeshTag Tag);

  /// Convert @a Value to EStreetMapMeshTag and get its name as FString.
  static FString ToString(uint8 Value) {
    return ToString(FromUInt(Value));
  }

  /// @}
};
