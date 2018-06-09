// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "SplineRoad.h"


// Sets default values
ASplineRoad::ASplineRoad(const FObjectInitializer& ObjectInitializer) :
  Super(ObjectInitializer)
{
  UE_LOG(LogCarla, Log, TEXT("SplineRoad: Constructed with ObjectInitializer."));
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
void ASplineRoad::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
  Super::PostEditChangeProperty(PropertyChangedEvent);
  UE_LOG(LogCarla, Log, TEXT("SplineRoad: PostEditChangeProperty."));
  if (PropertyChangedEvent.Property)
  {
    //ClearRoadSegments();
    //GenerateRoadSegments();
  }
}
#endif // WITH_EDITOR

void ASplineRoad::GenerateRoadSegments()
{
  if (RoadMesh == nullptr)
  {
    UE_LOG(LogCarla, Warning,
      TEXT("SplineRoad: Road Mesh not set, road not generated."));
    return;
  }
  UE_LOG(LogCarla, Log, TEXT("SplineRoad: GenerateRoadSegments."));

  int32 NumOfKnots = RoadSkeleton->GetNumberOfSplinePoints();
  UE_LOG(LogCarla, Log, TEXT("SplineRoad: NumOfKnots is %d."),NumOfKnots);
  check(NumOfKnots>0);
  // RoadSegments.Reset(NumOfKnots-1);
  // UE_LOG(LogCarla, Log, TEXT("SplineRoad: RoadSegments has size %d."),RoadSegments.Num());
  for (int32 i=0; i<NumOfKnots-1; ++i)
  {
    // Create spline mesh component
    USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
    SplineMesh->RegisterComponent();
    SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;

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

    // Set start and end
    SplineMesh->SetStartAndEnd(
      StartLocation, StartTangent, EndLocation, EndTangent, true);

    // Forward axis should always be X

    // Attach to root
    SplineMesh->AttachToComponent(RoadSkeleton, FAttachmentTransformRules::KeepRelativeTransform);
    SplineMesh->SetHiddenInGame(false);
  }
}

// void ASplineRoad::ClearRoadSegments()
// {
//   for (int32 i=0; i<RoadSegments.Num(); ++i)
//   {
//     check(RoadSegments[i] != nullptr);
//     RoadSegments[i]->UnregisterComponent();
//     RoadSegments[i]->DestroyComponent();
//     RoadSegments[i] = nullptr;
//   }
//   UE_LOG(LogCarla, Log, TEXT("SplineRoad: Road segments cleared."));
// }

void ASplineRoad::SetRoadSkeleton(const TArray<FVector>& knots,
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
  UE_LOG(LogCarla, Log, TEXT("SplineRoad: RoadSkeleton set."));
  //ClearRoadSegments();
  GenerateRoadSegments();
}

void ASplineRoad::SetRoadMesh(UStaticMesh *InRoadMesh)
{
  RoadMesh = InRoadMesh;
}

void ASplineRoad::OnConstruction(const FTransform &Transform)
{
  Super::OnConstruction(Transform);
  UE_LOG(LogCarla, Log, TEXT("SplineRoad: OnConstruction."));
  //ClearRoadSegments();
  GenerateRoadSegments();
}
