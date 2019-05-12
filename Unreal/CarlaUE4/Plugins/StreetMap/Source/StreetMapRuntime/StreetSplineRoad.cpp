// Copyright 2018 Rui Chen. All Rights Reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "StreetMapRuntime.h"
#include "StreetSplineRoad.h"


// Sets default values
AStreetSplineRoad::AStreetSplineRoad(const FObjectInitializer& ObjectInitializer) :
  Super(ObjectInitializer)
{
  // UE_LOG(LogTemp, Warning, TEXT("AStreetSplineRoad::Construct StreetSplineRoadInstance"));
  PrimaryActorTick.bCanEverTick = false;
  RootComponent =
      ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("TopScene"));
  RootComponent->SetMobility(EComponentMobility::Static);

  /// RoadMesh to be assigned manually
  RoadMesh = nullptr;

  RoadSkeleton = ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this, TEXT("Skeleton"));
  RoadSkeleton->SetupAttachment(RootComponent);
  RoadSkeleton->SetHiddenInGame(true);
  RoadSkeleton->SetMobility(EComponentMobility::Static);
  //RoadSkeleton->RegisterComponent();
  //RoadSkeleton->ClearSplinePoints();

  /// Spline init to empty
  //SplineMeshSegments.Reset(RoadSkeleton->GetNumberOfSplinePoints());

}

#if WITH_EDITOR
void AStreetSplineRoad::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
  Super::PostEditChangeProperty(PropertyChangedEvent);
  // UE_LOG(LogCarla, Log, TEXT("StreetSplineRoad: PostEditChangeProperty."));
  if (PropertyChangedEvent.Property)
  {
    //ClearRoadSegments();
    //GenerateRoadSegments();
  }
}
#endif // WITH_EDITOR

void AStreetSplineRoad::GenerateRoadSegments()
{
  if (RoadMesh == nullptr)
  {
    // UE_LOG(LogCarla, Warning,
    //   TEXT("StreetSplineRoad: Road Mesh not set, road not generated."));
    return;
  }
  // UE_LOG(LogCarla, Log, TEXT("StreetSplineRoad: GenerateRoadSegments."));

  int32 NumOfKnots = RoadSkeleton->GetNumberOfSplinePoints();
  // UE_LOG(LogCarla, Log, TEXT("StreetSplineRoad: NumOfKnots is %d."),NumOfKnots);
  check(NumOfKnots>0);
  // RoadSegments.Reset(NumOfKnots-1);
  // UE_LOG(LogCarla, Log, TEXT("StreetSplineRoad: RoadSegments has size %d."),RoadSegments.Num());
  for (int32 i=0; i<NumOfKnots-1; ++i)
  {
    // Create spline mesh component
    USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
    SplineMesh->RegisterComponent();
    SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
    SplineMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    // Get start and end of segment
    FVector StartLocation, StartTangent;
    FVector EndLocation,   EndTangent;
    RoadSkeleton->GetLocationAndTangentAtSplinePoint(
      i,   StartLocation, StartTangent, ESplineCoordinateSpace::Local);
    RoadSkeleton->GetLocationAndTangentAtSplinePoint(
      i+1, EndLocation,   EndTangent,   ESplineCoordinateSpace::Local);
    SplineMesh->SetMobility(EComponentMobility::Movable);
    // Set road mesh type
    SplineMesh->SetStaticMesh(RoadMesh);
    SplineMesh->SetForwardAxis(ForwardAxis, true);

    // Set start and end
    SplineMesh->SetStartAndEnd(
      StartLocation, StartTangent, EndLocation, EndTangent, true);

    // Forward axis should always be X

    // Attach to root
    SplineMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    SplineMesh->SetHiddenInGame(false);
  }
}

// void AStreetSplineRoad::ClearRoadSegments()
// {
//   for (int32 i=0; i<RoadSegments.Num(); ++i)
//   {
//     check(RoadSegments[i] != nullptr);
//     RoadSegments[i]->UnregisterComponent();
//     RoadSegments[i]->DestroyComponent();
//     RoadSegments[i] = nullptr;
//   }
  // UE_LOG(LogCarla, Log, TEXT("StreetSplineRoad: Road segments cleared."));
// }

void AStreetSplineRoad::SetRoadSkeleton(const TArray<FVector>& knots,
                                        const FVector& HeadTangent,
                                        const FVector& TailTangent)
{
  RoadSkeleton->ClearSplinePoints();
  for (int32 i=0; i<knots.Num(); ++i)
  {
    RoadSkeleton->AddSplinePoint(knots[i], ESplineCoordinateSpace::Local, true);
  }
  check(RoadSkeleton->GetNumberOfSplinePoints() == knots.Num());
  RoadSkeleton->SetTangentAtSplinePoint(0,
                                        HeadTangent,
                                        ESplineCoordinateSpace::Local,
                                        true);
  RoadSkeleton->SetTangentAtSplinePoint(knots.Num()-1,
                                        TailTangent,
                                        ESplineCoordinateSpace::Local,
                                        true);
  // UE_LOG(LogCarla, Log, TEXT("StreetSplineRoad: RoadSkeleton set."));
  //ClearRoadSegments();
  GenerateRoadSegments();
}

void AStreetSplineRoad::SetRoadMesh(
  EStreetMapMeshTag InTag, UStaticMesh *InRoadMesh, ESplineMeshAxis::Type Axis)
{
  Tag = InTag;
  RoadMesh = InRoadMesh;
  ForwardAxis = Axis;
}

void AStreetSplineRoad::OnConstruction(const FTransform &Transform)
{
  Super::OnConstruction(Transform);
  // UE_LOG(LogCarla, Log, TEXT("StreetSplineRoad: OnConstruction."));
  //ClearRoadSegments();
  GenerateRoadSegments();
}
