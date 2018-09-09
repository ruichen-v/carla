// Copyright 2017 Mike Fricker. All Rights Reserved.
// Copyright 2018 Rui Chen. All Rights Reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "StreetMapRuntime.h"
#include "StreetMapActor.h"
#include "StreetMapComponent.h"

using tag_size_t = std::underlying_type<EStreetMapMeshTag>::type;

constexpr static tag_size_t NUMBER_OF_SPTAGS = StreetMapMeshTag::GetNumberOfTags();

AStreetMapActor::AStreetMapActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StreetMapComponent = CreateDefaultSubobject<UStreetMapComponent>(TEXT("StreetMapComp"));
  StreetMapComponent->SetMobility(EComponentMobility::Static);
	RootComponent = StreetMapComponent;

  for (tag_size_t i = 0u; i < NUMBER_OF_SPTAGS; ++i) {
    // Add static mesh holder.
    // UE_LOG(LogTemp, Warning, TEXT("add to StaticMesh"));
    StaticMeshes.Add(StreetMapMeshTag::FromUInt(i));
  }
}

void AStreetMapActor::OnConstruction(const FTransform &Transform)
{
  Super::OnConstruction(Transform);
  // UE_LOG(LogTemp, Warning, TEXT("AStreetMapActor::OnConstruction"));
}

void AStreetMapActor::PostInitializeComponents()
{
  // UE_LOG(LogTemp, Warning, TEXT("AStreetMapActor::PostInitializeComponents"));
  Super::PostInitializeComponents();
  if(IsValid(GetLevel())&&!GetLevel()->IsPendingKill())
  {
    TArray<AActor*> SplineRoads;
    GetAttachedActors(SplineRoads);
    if(SplineRoads.Num()==0)
    {
     UpdateMap();
    }
  }
}

#if WITH_EDITOR
void AStreetMapActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
  Super::PostEditChangeProperty(PropertyChangedEvent);
  if (PropertyChangedEvent.Property)
  {
    // UE_LOG(LogTemp, Warning, TEXT("AStreetMapActor::PostEditChangeProperty"));
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
void AStreetMapActor::UpdateTagMap()
{
  TagMap.Empty();

  for (auto &TagMeshPair: StaticMeshes)
  {
    TagMap.Add(TagMeshPair.Value.Mesh, TagMeshPair.Key);
  }
}

/// Set the static mesh associated with @a Tag.
void AStreetMapActor::SetStaticMesh(EStreetMapMeshTag Tag, UStaticMesh *Mesh, ESplineMeshAxis::Type Axis)
{
  StaticMeshes[Tag] = FRoadMeshDescriptor(Mesh, Axis);
  if (Mesh != nullptr) {
    //Map key is unique
    TagMap.Add(Mesh, Tag);
  }
}

/// Return the static mesh corresponding to @a Tag.
UStaticMesh *AStreetMapActor::GetStaticMesh(EStreetMapMeshTag Tag)
{
  // UE_LOG(LogTemp, Warning, TEXT("AStreetMapActor::GetStaticMesh"));
  return StaticMeshes[Tag].Mesh;
}

/// Return the static mesh corresponding to @a Tag.
const UStaticMesh *AStreetMapActor::GetStaticMesh(EStreetMapMeshTag Tag) const
{
  return StaticMeshes[Tag].Mesh;
}

/// Return the static mesh corresponding to @a Tag.
TEnumAsByte<ESplineMeshAxis::Type> AStreetMapActor::GetMeshForwardAxis(EStreetMapMeshTag Tag)
{
  return StaticMeshes[Tag].ForwardAxis;
}

/// Return the tag corresponding to @a StaticMesh.
EStreetMapMeshTag AStreetMapActor::GetTag(const UStaticMesh &StaticMesh) const
{
  const EStreetMapMeshTag *Tag = TagMap.Find(&StaticMesh);
  return (Tag != nullptr ? *Tag : EStreetMapMeshTag::INVALID);
}

// =============================================================================
// -- Overriden from UObject ---------------------------------------------------
// =============================================================================

void AStreetMapActor::PreSave(const ITargetPlatform *TargetPlatform)
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

void AStreetMapActor::DeleteRoads()
{
  TArray<AActor*> SplineRoads;
  GetAttachedActors(SplineRoads);
  // UE_LOG(LogTemp, Warning, TEXT("AStreetMapActor::DeleteRoadNum  %d"), SplineRoads.Num());
  for (int32 i=SplineRoads.Num()-1; i>=0; --i)
  {
    // UE_LOG(LogTemp, Warning, TEXT("AStreetMapActor::DeleteRoad - %d"), i);
    SplineRoads[i]->Destroy();
  }
}

void AStreetMapActor::UpdateMap()
{
  //
  // AStreetSplineRoad only reads skeleton
  // from StreetMapComponent and generate roads
  //
  // UE_LOG(LogTemp, Warning, TEXT("AStreetMapActor::UpdateMap"));

  GenerateRoads();
  if (bTriggerRoadMapGeneration) {
    bTriggerRoadMapGeneration = false;
    // _MARK TODO call navigation-related info generator here
    //GenerateRoadMap();
  }
}

void AStreetMapActor::AddRoadInstance(const FRoadSkeletonDescriptor& RoadDescriptor)
{
  /// Create USplineComponent for road skeleton
  if (GetStaticMesh(RoadDescriptor.Tag) == nullptr)
  {
    // UE_LOG(LogTemp, Warning, TEXT("AStreetMapActor::AddRoadInstance: StaticMesh not set, RoadInstance not added."));
    return;
  }
  // Spawn param
  FActorSpawnParameters params;
  params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

  // Spawn road segment actor
  FVector Location(0.0f, 0.0f, 0.0f);
  FRotator Rotation(0.0f, 0.0f, 0.0f);
  AStreetSplineRoad* StreetSplineRoadActor = Cast<AStreetSplineRoad>(
    GetWorld()->SpawnActor(AStreetSplineRoad::StaticClass(), &Location, &Rotation, params));
  StreetSplineRoadActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
  // Configure road segment
  StreetSplineRoadActor->SetRoadMesh(RoadDescriptor.Tag,
    GetStaticMesh(RoadDescriptor.Tag), GetMeshForwardAxis(RoadDescriptor.Tag));
  StreetSplineRoadActor->SetRoadSkeleton(
                  RoadDescriptor.Knots,
                  RoadDescriptor.StartTangent*RoadDescriptor.TangentLength,
                  RoadDescriptor.EndTangent*RoadDescriptor.TangentLength);

  // StreetSplineRoadActor->Tags.Add(UCarlaSettings::CARLA_ROAD_TAG);
  StreetSplineRoadActor->bEnableAutoLODGeneration = true;
}

void AStreetMapActor::GenerateRoads()
{
  for (auto& RoadDescriptor: StreetMapComponent->MapSkeleton)
  {
    // UE_LOG(LogTemp, Warning, TEXT("AStreetMapActor::AddRoadInstance"));
    AddRoadInstance(RoadDescriptor);
  }
}
