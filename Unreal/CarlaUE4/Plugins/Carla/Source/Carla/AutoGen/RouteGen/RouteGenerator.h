// Copyright 2019 Rui Chen. All Rights Reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
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

private:

    FVector2D LatLongToCM(const double Latitude, const double Longitude);

    void DeleteRoutes();

    void UpdateRoutes();

protected:

    virtual void OnConstruction(const FTransform &Transform) override;

#if WITH_EDITOR
  virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

    UFUNCTION()
    void UpdateRoutePointNumbers();

    UFUNCTION()
    void ResetRoutePossibilities();

    UFUNCTION()
    void AddLaneChangeRoute();

    UFUNCTION()
    void GenerateRouteFromDataTable();

public:

    UPROPERTY(VisibleAnywhere, Category="Traffic Routes")
    TArray<uint32> RoutePointNumbers;

    UPROPERTY(EditAnywhere, Category="Traffic Routes")
    TArray<UDataTable*> GPS_routes;

    UPROPERTY(EditAnywhere, Category="Test")
    bool bTriggerLaneChangeGeneration = false;

    UPROPERTY(EditAnywhere, Category = "Projection")
    double AverageLatitude = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Projection")
    double AverageLongitude = 0.0f;

};

USTRUCT(BlueprintType)
struct CARLA_API FGPSLocationDescriptor : public FTableRowBase
{
  GENERATED_BODY()

public:

    FGPSLocationDescriptor(){}

    FGPSLocationDescriptor(const double _Latitude, const double _Longitude):
        Latitude(_Latitude), Longitude(_Longitude)
    {
        //
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Latitude;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Longitude;
};
