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
enum class ESplineCityMapMeshTag : uint8
{
  RoadTwoLanes_NoSide               UMETA(DisplayName = "Road: Two Lanes - No Side"),

  NUMBER_OF_TAGS         UMETA(Hidden),
  INVALID                UMETA(Hidden)
};

/// Helper class for working with ESplineCityMapMeshTag.
class SplineCityMapMeshTag
{
public:

  /// Return the number of tags.
  static constexpr uint8 GetNumberOfTags() {
    return ToUInt(ESplineCityMapMeshTag::NUMBER_OF_TAGS);
  }

  /// Return the base mesh. The base mesh defines the road width
  static ESplineCityMapMeshTag GetBaseMeshTag();

  // _MARK TODO discard if dynamically done in map generator
  /// Get the size in tiles of a road intersection side. I.e., return N such NxN
  /// is the size of a road intersection piece.
  static uint32 GetRoadIntersectionSize();

  /// @name Tag conversions
  /// @{

  /// Convert @a Tag to an unsigned integer type.
  static constexpr uint8 ToUInt(ESplineCityMapMeshTag Tag) {
    return static_cast<uint8>(Tag);
  }

  /// Convert an unsigned integer to a ESplineCityMapMeshTag.
  static ESplineCityMapMeshTag FromUInt(uint8 Value) {
    check(Value < GetNumberOfTags());
    return static_cast<ESplineCityMapMeshTag>(Value);
  }

  /// Get @a Tag name as FString.
  static FString ToString(ESplineCityMapMeshTag Tag);

  /// Convert @a Value to ESplineCityMapMeshTag and get its name as FString.
  static FString ToString(uint8 Value) {
    return ToString(FromUInt(Value));
  }

  /// @}
};
