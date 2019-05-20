// Copyright 2019 Rui Chen. All Rights Reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "RouteGenerator.h"
#include "UnrealMathUtility.h"


#if WITH_EDITOR
void ARouteGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
  Super::PostEditChangeProperty(PropertyChangedEvent);
  if (PropertyChangedEvent.Property)
  {
    DeleteRoutes();
    UpdateRoutes();
  }
}
#endif // WITH_EDITOR

void ARouteGenerator::DeleteRoutes()
{
  // UE_LOG(LogCarla, Warning, TEXT("ARouteGenerator::DeleteRoadNum  %d"), Routes.Num());
  for (int32 i=Routes.Num()-1; i>=0; --i)
  {
    // UE_LOG(LogCarla, Warning, TEXT("ARouteGenerator::DeleteRoad - %d"), i);
    Routes[i]->UnregisterComponent();
  }
  Routes.Reset(0);
}

void ARouteGenerator::UpdateRoutes()
{
    GenerateRouteFromDataTable();
    if (bTriggerLaneChangeGeneration) {
        bTriggerLaneChangeGeneration = false;
        // _MARK TODO call navigation-related info generator here
        AddLaneChangeRoute();
    }
    UpdateRoutePointNumbers();
    ResetRoutePossibilities();
}

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
}

void ARouteGenerator::OnConstruction(const FTransform &Transform)
{
  Super::OnConstruction(Transform);
}

void ARouteGenerator::AddLaneChangeRoute()
{
    auto NewRoute = NewObject<USplineComponent>(this);

    NewRoute->SetupAttachment(RootComponent);
    NewRoute->SetHiddenInGame(true);
    NewRoute->SetMobility(EComponentMobility::Static);
    NewRoute->RegisterComponent();

    NewRoute->ClearSplinePoints();

    // Trajectory params
    float l1(Route_LANE_CHANGE_LINE_PRE_DEFAULT);
    float l2(Route_LANE_CHANGE_LINE_POST_DEFAULT);
    float d(Route_LANE_CHANGE_DISTANCE_FORWARD_DEFAULT);
    float A(Route_LANE_CHANGE_DISTANCE_SIDE_DEFAULT/2.0f);
    float w(_PI/d);
    uint8 N(Route_LANE_CHANGE_CURVE_PT_NUM_DEFAULT);

    UE_LOG(LogCarla, Warning, TEXT("Root position %f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z );
    // Origin
    NewRoute->AddSplineLocalPoint(FVector(0.0f, 0.0f, 0.0f));
    // Lane Change Start
    NewRoute->AddSplineLocalPoint(FVector(l1, 0.0f, 0.0f));
    // Lane Change
    float x(0.0f), y(0.0f), SinVal(0.0f), CosVal(0.0f);
    const float deltaX(d/static_cast<float>(N+1));
    for (uint8 i=1; i<=N; ++i)
    {
    x = l1 + i*deltaX;
    FMath::SinCos(&SinVal, &CosVal, w*(x-l1));
    y = A*CosVal-A;
    NewRoute->AddSplineLocalPoint(FVector(x, -y, 0.0f));
    }

    NewRoute->AddSplineLocalPoint(FVector(l1 + d     , 2*A, 0.0f));
    NewRoute->AddSplineLocalPoint(FVector(l1 + d + l2, 2*A, 0.0f));

    Routes.Add(NewRoute);
}

FVector2D ARouteGenerator::LatLongToCM(const double Latitude, const double Longitude)
{
    // Latitude/longitude scale factor
    //			- https://en.wikipedia.org/wiki/Equator#Exact_length
    static const double EarthCircumference = 40075036.0;
    double LatitudeLongitudeScale = EarthCircumference / 360.0; // meters per degree
    const float OSMToCentimetersScaleFactor = 100.0f;

    // Converts latitude to meters
    auto ConvertLatitudeToMeters = [LatitudeLongitudeScale]( const double Latitude ) -> double
    {
        return -Latitude * LatitudeLongitudeScale;
    };

    // Converts longitude to meters
    auto ConvertLongitudeToMeters = [LatitudeLongitudeScale]( const double Longitude, const double Latitude ) -> double
    {
        return Longitude * LatitudeLongitudeScale * FMath::Cos( FMath::DegreesToRadians( Latitude ) );
    };

    // Converts latitude and longitude to X/Y coordinates, relative to some other latitude/longitude
    auto ConvertLatLongToMetersRelative = [ConvertLatitudeToMeters, ConvertLongitudeToMeters](
        const double Latitude,
        const double Longitude,
        const double RelativeToLatitude,
        const double RelativeToLongitude ) -> FVector2D
    {
        // Applies Sanson-Flamsteed (sinusoidal) Projection (see http://www.progonos.com/furuti/MapProj/Normal/CartHow/HowSanson/howSanson.html)
        return FVector2D(
            (float)( ConvertLongitudeToMeters( Longitude, Latitude ) - ConvertLongitudeToMeters( RelativeToLongitude, Latitude ) ),
            (float)( ConvertLatitudeToMeters( Latitude ) - ConvertLatitudeToMeters( RelativeToLatitude ) ) );
    };

    return ConvertLatLongToMetersRelative(
                Latitude,
                Longitude,
                AverageLatitude,
                AverageLongitude ) * OSMToCentimetersScaleFactor;
}

void ARouteGenerator::GenerateRouteFromDataTable()
{
    Routes.Reset(GPS_routes.Num());
    for (int32 i=0; i<GPS_routes.Num(); ++i)
    {
    
        UDataTable* RouteData = GPS_routes[i];

        if (RouteData==NULL) continue;

        UE_LOG(LogCarla, Log, TEXT("Processing RouteData index: %d"), i);

        auto NewRoute = NewObject<USplineComponent>(this);

        NewRoute->SetupAttachment(RootComponent);
        NewRoute->SetHiddenInGame(true);
        NewRoute->SetMobility(EComponentMobility::Static);
        NewRoute->RegisterComponent();
        NewRoute->ClearSplinePoints();

        for(int32 nodei = 0; nodei < RouteData->GetRowNames().Num(); nodei++)
        {
            FGPSLocationDescriptor* Row = RouteData->FindRow<FGPSLocationDescriptor>(FName(*FString::FromInt(nodei)), TEXT(""));
            if (Row)
            {
                FVector2D location2D = LatLongToCM(Row->Latitude, Row->Longitude);
                NewRoute->AddSplineLocalPoint(FVector(location2D, 0.0f));
                UE_LOG(LogCarla, Log, TEXT("Adding GPS location index: %d, x: %f, y: %f"), nodei, location2D.X, location2D.Y);
            }
            else
            {
                UE_LOG(LogCarla, Error, TEXT("Cannot find GPS location index: %d"), nodei);
            }
        }

        Routes.Add(NewRoute);
    }
}