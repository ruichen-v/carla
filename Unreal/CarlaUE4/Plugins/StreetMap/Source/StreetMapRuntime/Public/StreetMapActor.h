// Copyright 2017 Mike Fricker. All Rights Reserved.

#pragma once

#include "StreetMapMeshTag.h"
#include "StreetSplineRoad.h"
#include "StreetMapActor.generated.h"

/** An actor that renders a street map mesh component */
UCLASS(hidecategories = (Physics)) // Physics category in detail panel is hidden. Our component/Actor is not simulated !
class STREETMAPRUNTIME_API AStreetMapActor : public AActor
{
	GENERATED_UCLASS_BODY()

	/**  Component that represents a section of street map roads and buildings */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StreetMap")
		class UStreetMapComponent* StreetMapComponent;

public:
	FORCEINLINE class UStreetMapComponent* GetStreetMapComponent() { return StreetMapComponent; }

protected:

  /// Initializes the instantiators.
  virtual void OnConstruction(const FTransform &Transform) override;
  virtual void PostInitializeComponents() override;

#if WITH_EDITOR
  /// Clears and updates the instantiators.
  virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

  // ===========================================================================
  // -- Mesh-related protected methods -----------------------------------------
  // ===========================================================================
protected:

  /// Update Tag Map
  void UpdateTagMap();

  /// Set the static mesh associated with @a Tag.
  void SetStaticMesh(EStreetMapMeshTag Tag, UStaticMesh *Mesh, ESplineMeshAxis::Type);

  /// Return the static mesh corresponding to @a Tag.
  UStaticMesh *GetStaticMesh(EStreetMapMeshTag Tag);

  /// Return the static mesh corresponding to @a Tag.
  const UStaticMesh *GetStaticMesh(EStreetMapMeshTag Tag) const;

  /// Return forward axis corresponding to @a Tag.
  TEnumAsByte<ESplineMeshAxis::Type> GetMeshForwardAxis(EStreetMapMeshTag Tag);

  /// Return the tag corresponding to @a StaticMesh.
  EStreetMapMeshTag GetTag(const UStaticMesh &StaticMesh) const;

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

  /// Add road instance (no intersection)
  void AddRoadInstance(const FRoadSkeletonDescriptor& RoadDescriptor);

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
  // UPROPERTY(Category = "Map Generation", EditAnywhere)
  // bool bGenerateRoads = true;

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
  // -- Road mesh config -------------------------------------------------------
  // ===========================================================================
private:

  UPROPERTY(Category = "Meshes", EditAnywhere)
  TMap<EStreetMapMeshTag, FRoadMeshDescriptor> StaticMeshes;

  UPROPERTY(Category = "Meshes", VisibleAnywhere)
  TMap<UStaticMesh *, EStreetMapMeshTag> TagMap;
};
