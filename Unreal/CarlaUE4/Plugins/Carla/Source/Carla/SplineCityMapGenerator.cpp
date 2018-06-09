// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "SplineCityMapGenerator.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/World.h"
#include "Settings/CarlaSettings.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Game/CarlaGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"

#include <vector>

using tag_size_t = std::underlying_type<ESplineCityMapMeshTag>::type;

//static float (&Sqrt)(float) = FGenericPlatformMath::Sqrt;

constexpr static tag_size_t NUMBER_OF_SPTAGS = SplineCityMapMeshTag::GetNumberOfTags();


// =============================================================================
// -- Construction and update related methods ----------------------------------
// =============================================================================

ASplineCityMapGenerator::ASplineCityMapGenerator(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  SceneRootComponent =
      ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("TopScene"));
  SceneRootComponent->SetMobility(EComponentMobility::Static);
  RootComponent = SceneRootComponent;

  MapSkeleton.Empty();

  for (tag_size_t i = 0u; i < NUMBER_OF_SPTAGS; ++i) {
    // Add static mesh holder.
    StaticMeshes.Add(SplineCityMapMeshTag::FromUInt(i));
  }
}

ASplineCityMapGenerator::~ASplineCityMapGenerator() {}

void ASplineCityMapGenerator::PostInitializeComponents()
{
  Super::PostInitializeComponents();

  if(IsValid(GetLevel())&&!GetLevel()->IsPendingKill())
  {
    TArray<AActor*> SplineRoads;
    GetAttachedActors(SplineRoads);
    if(SplineRoads.Num()==0)
    {
     UE_LOG(LogCarla, Error, TEXT("Please regenerate the road in edit mode for '%s' actor"), *UKismetSystemLibrary::GetDisplayName(this));
     UpdateMap();
    }
  }

}

#if WITH_EDITOR
void ASplineCityMapGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
  Super::PostEditChangeProperty(PropertyChangedEvent);
  if (PropertyChangedEvent.Property)
  {
    DeleteRoads();
    UpdateTagMap();
    UpdateMap();
  }
}
#endif // WITH_EDITOR


// ===========================================================================
// -- Other protected methods ------------------------------------------------
// ===========================================================================

/// Update Tag Map
void ASplineCityMapGenerator::UpdateTagMap()
{
  UE_LOG(LogCarla, Log, TEXT("SplineCityMapGenerator: Updating Tag Map..."));
  // for (auto &e: TagMap)
  // {
  //   UE_LOG(LogCarla, Log, TEXT("PreReset, tag: %s"), *SplineCityMapMeshTag::ToString(e.Value));
  // }

  TagMap.Reset();
  //TagMap.Reserve(NUMBER_OF_SPTAGS);

  // for (auto &e: TagMap)
  // {
  //   UE_LOG(LogCarla, Log, TEXT("PostReset, tag: %s"), *SplineCityMapMeshTag::ToString(e.Value));
  // }

  for (auto &TagMeshPair: StaticMeshes)
  {
    TagMap.Add(TagMeshPair.Value, TagMeshPair.Key);
  }
}

/// Set the static mesh associated with @a Tag.
void ASplineCityMapGenerator::SetStaticMesh(ESplineCityMapMeshTag Tag, UStaticMesh *Mesh)
{
  StaticMeshes[Tag] = Mesh;
  if (Mesh != nullptr) {
    TagMap.Add(Mesh, Tag);
  }
}

/// Return the static mesh corresponding to @a Tag.
UStaticMesh *ASplineCityMapGenerator::GetStaticMesh(ESplineCityMapMeshTag Tag)
{
  return StaticMeshes[Tag];
}

/// Return the static mesh corresponding to @a Tag.
const UStaticMesh *ASplineCityMapGenerator::GetStaticMesh(ESplineCityMapMeshTag Tag) const
{
  return StaticMeshes[Tag];
}

/// Return the tag corresponding to @a StaticMesh.
ESplineCityMapMeshTag ASplineCityMapGenerator::GetTag(const UStaticMesh &StaticMesh) const
{
  const ESplineCityMapMeshTag *Tag = TagMap.Find(&StaticMesh);
  return (Tag != nullptr ? *Tag : ESplineCityMapMeshTag::INVALID);
}

// =============================================================================
// -- Overriden from UObject ---------------------------------------------------
// =============================================================================

void ASplineCityMapGenerator::PreSave(const ITargetPlatform *TargetPlatform)
{
#if WITH_EDITOR
  if (bGenerateRoadMapOnSave) {
    // Generate road map only if we are not cooking.
    // _MARK nothing for now
    // FCoreUObjectDelegates::OnObjectSaved.Broadcast(this);
    // if (!GIsCookerLoadingPackage) {
    //   check(RoadMap != nullptr);
    //   GenerateRoadMap();
    // }
  }
#endif // WITH_EDITOR

  Super::PreSave(TargetPlatform);
}

// =============================================================================
// -- Map construction and update related methods ------------------------------
// =============================================================================

void ASplineCityMapGenerator::DeleteRoads()
{
  TArray<AActor*> SplineRoads;
  GetAttachedActors(SplineRoads);
  for (int32 i=SplineRoads.Num()-1; i>=0; --i)
  {
    SplineRoads[i]->Destroy();
  }
  UE_LOG(LogCarla, Log, TEXT("SplineCityMapGenerator: DeleteRoads."));
}

void ASplineCityMapGenerator::UpdateMap()
{
  UE_LOG(LogCarla, Log, TEXT("SplineCityMapGenerator: UpdateMap."));
  if (bGenerateRoads)
  {
    GenerateRoads();
  }
  if (bTriggerRoadMapGeneration) {
    bTriggerRoadMapGeneration = false;
    // _MARK TODO call navigation-related info generator here
    //GenerateRoadMap();
  }
}

void ASplineCityMapGenerator::AddRoadInstance(const FRoadDescriptor& RoadDescriptor,
                                              ESplineCityMapMeshTag Tag)
{
  UE_LOG(LogCarla, Log, TEXT("SplineCityMapGenerator: AddRoadInstance."));
  /// Create USplineComponent for road skeleton
  // Spawn param
  FActorSpawnParameters params;
  params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

  // Spawn road segment actor
  FVector Location(0.0f, 0.0f, 0.0f);
  FRotator Rotation(0.0f, 0.0f, 0.0f);
  ASplineRoad* SplineRoadActor = Cast<ASplineRoad>(
    GetWorld()->SpawnActor(ASplineRoad::StaticClass(), &Location, &Rotation, params));
  SplineRoadActor->AttachToActor(this,FAttachmentTransformRules::KeepRelativeTransform);
  // Configure road segment
  SplineRoadActor->SetRoadMesh(GetStaticMesh(Tag));
  SplineRoadActor->SetRoadSkeleton(
                  RoadDescriptor.Knots,
                  RoadDescriptor.StartTangent*RoadDescriptor.TangentLength,
                  RoadDescriptor.EndTangent*RoadDescriptor.TangentLength);

  SplineRoadActor->Tags.Add(UCarlaSettings::CARLA_ROAD_TAG);
  SplineRoadActor->bEnableAutoLODGeneration = true;
}

void ASplineCityMapGenerator::AddRoadDescriptor(const TArray<FVector>& Knots,
                                                const FVector& StartTangent,
                                                const FVector& EndTangent,
                                                const float& TangentLength)
{
  FRoadDescriptor NewRoad(Knots, StartTangent, EndTangent, TangentLength);
  MapSkeleton.Add(NewRoad);
}

void ASplineCityMapGenerator::OnConstruction(const FTransform &Transform)
{
  Super::OnConstruction(Transform);
  //TEMP INIT start
  if (MapSkeleton.Num()==0)
  {
    float SingleRoadLength = 500.0f;
    float TangentLength = SingleRoadLength;
    int32 NumOfIntervals = 4;
    FVector ActorLocation = GetActorLocation();

    FVector entrance1_rel(-400.0f-400.0f*sqrt(2.0f), 0.0f, 0.0f);
    FVector dir1(-1.0f, 0.0f, 0.0f);

    FVector entrance2_rel(400.0f+200.0f*sqrt(2.0f), -400.0f-200.0f*sqrt(2.0f), 0.0f);
    FVector dir2(1.0f/sqrt(2.0f), -1.0f/sqrt(2.0f), 0.0f);

    FVector entrance3_rel(400.0f+400.0f*sqrt(2.0f), 0.0f, 0.0f);
    FVector dir3(1.0f, 0.0f, 0.0f);

    FVector entrance4_rel(-400.0f-200.0f*sqrt(2.0f), 400.0f+200.0f*sqrt(2.0f), 0.0f);
    FVector dir4(-1.0f/sqrt(2.0f), 1.0f/sqrt(2.0f), 0.0f);

    TArray<FVector> RelativeEntranceLocations;
    TArray<FVector> UnitRoadDirections;

    RelativeEntranceLocations.Emplace(entrance1_rel);
    RelativeEntranceLocations.Emplace(entrance2_rel);
    RelativeEntranceLocations.Emplace(entrance3_rel);
    RelativeEntranceLocations.Emplace(entrance4_rel);

    UnitRoadDirections.Emplace(dir1);
    UnitRoadDirections.Emplace(dir2);
    UnitRoadDirections.Emplace(dir3);
    UnitRoadDirections.Emplace(dir4);

    for (int32 ei=0; ei<RelativeEntranceLocations.Num(); ++ei)
    {
      TArray<FVector> Knots;
      for (int32 ii=0; ii<NumOfIntervals; ++ii)
      {
        Knots.Emplace(RelativeEntranceLocations[ei] +
                      ii*UnitRoadDirections[ei]*SingleRoadLength);
      }
      AddRoadDescriptor(Knots,
                        UnitRoadDirections[ei],
                        UnitRoadDirections[ei],
                        TangentLength);
    }
    // TEMP INIT end

  }
}

void ASplineCityMapGenerator::GenerateRoads()
{
  UE_LOG(LogCarla, Log, TEXT("SplineCityMapGenerator: GenerateRoads."));
  for (auto& RoadDescriptor: MapSkeleton)
  {
    AddRoadInstance(
      RoadDescriptor,
      ESplineCityMapMeshTag::RoadTwoLanes_NoSide);
  }
}
