// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "GameFramework/Actor.h"
#include "MapGen/SplineCityMapMeshTag.h"

#include "MapGen/SplineRoad.h"
#include "SplineCityMapGenerator.generated.h"

USTRUCT(BlueprintType)
struct CARLA_API FRoadDescriptor
{
  GENERATED_BODY()

public:

  FRoadDescriptor(){}

  FRoadDescriptor(const TArray<FVector>& InKnots,
                  const FVector& InStartTangent,
                  const FVector& InEndTangent,
                  const float& InTangentLength) :
    Knots(InKnots),
    StartTangent(InStartTangent),
    EndTangent(InEndTangent),
    TangentLength(InTangentLength)
  {

  }

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FVector> Knots;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector StartTangent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector EndTangent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float TangentLength;
};

/**
 * 
 */
UCLASS()
class CARLA_API ASplineCityMapGenerator : public AActor
{
    GENERATED_BODY()
    
  // ===========================================================================
  // -- Construction and update related methods --------------------------------
  // ===========================================================================
public:
    
  ASplineCityMapGenerator(const FObjectInitializer& ObjectInitializer);

  ~ASplineCityMapGenerator();

protected:

  /// Initializes the instantiators.
  virtual void OnConstruction(const FTransform &Transform) override;
  virtual void PostInitializeComponents() override;

#if WITH_EDITOR
  /// Clears and updates the instantiators.
  virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

  // ===========================================================================
  // -- Other protected methods ------------------------------------------------
  // ===========================================================================
protected:

  /// Update Tag Map
  void UpdateTagMap();

  /// Set the static mesh associated with @a Tag.
  void SetStaticMesh(ESplineCityMapMeshTag Tag, UStaticMesh *Mesh);

  /// Return the static mesh corresponding to @a Tag.
  UStaticMesh *GetStaticMesh(ESplineCityMapMeshTag Tag);

  /// Return the static mesh corresponding to @a Tag.
  const UStaticMesh *GetStaticMesh(ESplineCityMapMeshTag Tag) const;

  /// Return the tag corresponding to @a StaticMesh.
  ESplineCityMapMeshTag GetTag(const UStaticMesh &StaticMesh) const;

  /// @}
  // ===========================================================================
  /// @name Overriden from UObject
  // ===========================================================================
  /// @{
public:

  virtual void PreSave(const ITargetPlatform *TargetPlatform) override;

  /// @}
  // ===========================================================================
  /// @name Map construction and update related methods
  // ===========================================================================
  /// @{
private:

  /// Clear all instances of the static mesh actors.
  void DeleteRoads();

  /// Update map
  void UpdateMap();

  /// Add road descriptor
  void AddRoadDescriptor(const TArray<FVector>& Knots,
                         const FVector& StartTangent,
                         const FVector& EndTangent,
                         const float& TangentLength);

  /// Add road instance (no intersection)
  void AddRoadInstance(const FRoadDescriptor& RoadDescriptor,
                       ESplineCityMapMeshTag Tag);

  /// Add the road meshes to the scene based on the current MAP REPRESENTATION.
  void GenerateRoads();

  /// @}
  // ===========================================================================
  /// @name Map generation properties
  // ===========================================================================
  /// @{
private:

  /** If false, no mesh is added, only the internal representation of road is
    * generated.
    */
  UPROPERTY(Category = "Map Generation", EditAnywhere)
  bool bGenerateRoads = true;

  /// @}
  // ===========================================================================
  /// @name Road Map
  // ===========================================================================
  /// @{
private:

  /** Trigger the generation a the road map image of the current layout (used
    * for off-road and opposite lane invasion detection).
    */
  UPROPERTY(Category = "Road Map", EditAnywhere)
  bool bTriggerRoadMapGeneration = false;

  /** If true, a debug point is drawn in the level for each pixel of the road
    * map.
    */
  UPROPERTY(Category = "Road Map", EditAnywhere)
  bool bDrawDebugPixelsToLevel = false;

  /** The road map is re-computed on save so we always store an up-to-date
    * version. Uncheck this only for testing purposes as the road map might get
    * out-of-sync with the current road layout.
    */
  UPROPERTY(Category = "Road Map", EditAnywhere, AdvancedDisplay)
  bool bGenerateRoadMapOnSave = true;

  /** If true, activate the custom depth pass of each tagged actor in the level.
    * This pass is necessary for rendering the semantic segmentation. However,
    * it may add a performance penalty since occlusion doesn't seem to be
    * applied to objects having this value active.
    */
  UPROPERTY(Category = "Road Map", EditAnywhere, AdvancedDisplay)
  bool bTagForSemanticSegmentation = false;

  // ===========================================================================
  // -- Private methods and members --------------------------------------------
  // ===========================================================================
private:

  UPROPERTY()
  USceneComponent *SceneRootComponent;

  /// @}
  // ===========================================================================
  /// @name Map representation
  // ===========================================================================
  /// @{
private:

  UPROPERTY(Category = "Map representation", EditAnywhere)
  TArray<FRoadDescriptor> MapSkeleton;

  // ===========================================================================
  // -- Road mesh config -------------------------------------------------------
  // ===========================================================================
private:
  // UPROPERTY(Category = "Map Generation", VisibleAnywhere)
  // float MapScale = 1.0f;

  UPROPERTY(Category = "Meshes", EditAnywhere)
  TMap<ESplineCityMapMeshTag, UStaticMesh *> StaticMeshes;

  UPROPERTY(Category = "Meshes", VisibleAnywhere)
  TMap<UStaticMesh *, ESplineCityMapMeshTag> TagMap;



};
