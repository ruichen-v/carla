// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SplineCityMapMeshTag.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "SplineRoad.generated.h"

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

UCLASS()
class CARLA_API ASplineRoad : public AActor
{
  GENERATED_BODY()

  /// @}
  // ===========================================================================
  /// @name Constructor
  // ===========================================================================
  /// @{
public:

  ASplineRoad(const FObjectInitializer& ObjectInitializer);

protected:

  virtual void OnConstruction(const FTransform &Transform) override;

  /// @}
  // ===========================================================================
  /// @name Exposed
  // ===========================================================================
  /// @{
public:

  UFUNCTION(BlueprintCallable)
  void SetRoadMesh(UStaticMesh *InRoadMesh);

  UFUNCTION(BlueprintCallable)
  void SetRoadSkeleton(const TArray<FVector>& knots,
                       const FVector& HeadTangent,
                       const FVector& TailTangent);

protected:

#if WITH_EDITOR
  virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

  void GenerateRoadSegments();
  // void ClearRoadSegments();

  /// @}
  // ===========================================================================
  /// @name Road Mesh
  // ===========================================================================
  /// @{
protected:
  /// Road assets

  UPROPERTY(BlueprintReadWrite, Category = "Road Mesh", EditAnyWhere, EditFixedSize)
  UStaticMesh *RoadMesh;

  // UPROPERTY(BlueprintReadWrite, Category = "Road Mesh", VisibleAnyWhere)
  // TArray<USplineMeshComponent *> RoadSegments;

  /// @}
  // ===========================================================================
  /// @name Road Skeleton
  // ===========================================================================
  /// @{
protected:
  /// Road curve

  UPROPERTY(Category = "Road structure", EditAnyWhere)
  USplineComponent *RoadSkeleton;

  /// @}
  // ===========================================================================
  /// @name Internal properties
  // ===========================================================================
  /// @{
private:

};
