// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "RouteGenerator.h"
#include "UnrealMathUtility.h"


#if WITH_EDITOR
void ARouteGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
  Super::PostEditChangeProperty(PropertyChangedEvent);
  UpdateRoutePointNumbers();
}
#endif // WITH_EDITOR

void ARouteGenerator::UpdateRoutePointNumbers()
{
  const auto Size = Routes.Num();
  RoutePointNumbers.Reset(Size);
  for (auto i = 0; i < Size; ++i) {
    RoutePointNumbers.Add(Routes[i]->GetNumberOfSplinePoints());
  }
}

void ARouteGenerator::ResetRoutePossibilities()
{
  const auto Size = Routes.Num();
  Probabilities.Reset(Size);
  for (auto i = 0; i < Size; ++i) {
    Probabilities.Add(1.0f / static_cast<float>(Size));
  }
}

ARouteGenerator::ARouteGenerator(const FObjectInitializer& ObjectInitializer) :
  Super(ObjectInitializer)
{
  GenerateRoutes(ObjectInitializer);
}

void ARouteGenerator::GenerateRoutes(const FObjectInitializer& ObjectInitializer)
{
  Routes.Reset(1);
  auto Route =
      ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this, TEXT("Route"));
  Routes.Add(Route);
  Routes[0]->SetupAttachment(RootComponent);
  Routes[0]->SetHiddenInGame(true);
  Routes[0]->SetMobility(EComponentMobility::Static);
  Routes[0]->RegisterComponent();

  Routes[0]->ClearSplinePoints();

  // Trajectory params
  float l1(Route_LANE_CHANGE_LINE_PRE_DEFAULT);
  float l2(Route_LANE_CHANGE_LINE_POST_DEFAULT);
  float d(Route_LANE_CHANGE_DISTANCE_FORWARD_DEFAULT);
  float A(Route_LANE_CHANGE_DISTANCE_SIDE_DEFAULT/2.0f);
  float w(_PI/d);
  uint8 N(Route_LANE_CHANGE_CURVE_PT_NUM_DEFAULT);

  UE_LOG(LogCarla, Warning, TEXT("Root position %f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z );
  // Origin
  Routes[0]->AddSplineLocalPoint(FVector(0.0f, 0.0f, 0.0f));
  // Lane Change Start
  Routes[0]->AddSplineLocalPoint(FVector(l1, 0.0f, 0.0f));
  // Lane Change
  float x(0.0f), y(0.0f), SinVal(0.0f), CosVal(0.0f);
  const float deltaX(d/static_cast<float>(N+1));
  for (uint8 i=1; i<=N; ++i)
  {
    x = l1 + i*deltaX;
    FMath::SinCos(&SinVal, &CosVal, w*(x-l1));
    y = A*CosVal-A;
    Routes[0]->AddSplineLocalPoint(FVector(x, -y, 0.0f));
  }

  Routes[0]->AddSplineLocalPoint(FVector(l1 + d     , 2*A, 0.0f));
  Routes[0]->AddSplineLocalPoint(FVector(l1 + d + l2, 2*A, 0.0f));
  
  UpdateRoutePointNumbers();
  ResetRoutePossibilities();
}