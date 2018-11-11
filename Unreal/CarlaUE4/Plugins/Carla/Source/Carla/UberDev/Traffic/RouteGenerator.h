// Copyright 2018 Rui Chen. All Rights Reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "CoreMinimal.h"
#include "Traffic/RoutePlanner.h"
#include "RouteGenerator.generated.h"

#define _PI 3.14159265359f

#define Carla_LANE_WIDTH                                400.0f
#define Route_LANE_CHANGE_LINE_PRE_DEFAULT              470.0f
#define Route_LANE_CHANGE_LINE_POST_DEFAULT             470.0f
#define Route_LANE_CHANGE_DISTANCE_FORWARD_DEFAULT      1000.0f
#define Route_LANE_CHANGE_DISTANCE_SIDE_DEFAULT         Carla_LANE_WIDTH
#define Route_LANE_CHANGE_CURVE_PT_NUM_DEFAULT          5

/**
 * Route Generator
 * Generate random local trajectory of a single vehicle (non-player)
 */
UCLASS()
class CARLA_API ARouteGenerator : public ARoutePlanner
{
  GENERATED_BODY()

public:

    ARouteGenerator(const FObjectInitializer& ObjectInitializer);
    FVector2D LatLongToCM(const double Longitude, const double Latitude);

protected:

    virtual void OnConstruction(const FTransform &Transform) override;

#if WITH_EDITOR
  virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

    UFUNCTION()
    void UpdateRoutePointNumbers();

    UFUNCTION()
    void ResetRoutePossibilities();

    void GenerateRoutes();

public:

    UPROPERTY(VisibleAnywhere, Category="Traffic Routes")
    TArray<uint32> RoutePointNumbers;

    UPROPERTY(EditAnywhere, Category = "Projection")
    double RelativeLongitude;

    UPROPERTY(EditAnywhere, Category = "Projection")
    double RelativeLatitude;

};