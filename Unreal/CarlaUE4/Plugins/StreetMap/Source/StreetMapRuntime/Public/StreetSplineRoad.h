// Copyright 2018 Rui Chen. All Rights Reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "StreetMapMeshTag.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "StreetSplineRoad.generated.h"

USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FRoadSkeletonDescriptor
{
  GENERATED_BODY()

public:

  FRoadSkeletonDescriptor(){}

  FRoadSkeletonDescriptor(const TArray<FVector>& InKnots,
                  const FVector& InStartTangent,
                  const FVector& InEndTangent,
                  const float& InTangentLength,
                  EStreetMapMeshTag InTag):
    Knots(InKnots),
    StartTangent(InStartTangent),
    EndTangent(InEndTangent),
    TangentLength(InTangentLength)
  {
    Tag = InTag;
  }

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FVector> Knots;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector StartTangent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector EndTangent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EStreetMapMeshTag Tag;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float TangentLength;
};

USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FRoadMeshDescriptor
{
  GENERATED_BODY()

public:

  FRoadMeshDescriptor() : Mesh(nullptr), ForwardAxis(ESplineMeshAxis::X)
  {

  }

  FRoadMeshDescriptor(UStaticMesh *InMesh, ESplineMeshAxis::Type InAxis)
  {
    Mesh = InMesh;
    ForwardAxis = InAxis;
  }

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticMesh *Mesh;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;
};

UCLASS()
class STREETMAPRUNTIME_API AStreetSplineRoad : public AActor
{
  GENERATED_BODY()

  /// @}
  // ===========================================================================
  /// @name Constructor
  // ===========================================================================
  /// @{
public:

  AStreetSplineRoad(const FObjectInitializer& ObjectInitializer);

protected:

  virtual void OnConstruction(const FTransform &Transform) override;

  /// @}
  // ===========================================================================
  /// @name Exposed
  // ===========================================================================
  /// @{
public:

  UFUNCTION(BlueprintCallable)
  void SetRoadMesh(EStreetMapMeshTag Tag, UStaticMesh *InRoadMesh, ESplineMeshAxis::Type Axis);

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
  EStreetMapMeshTag Tag;

  UPROPERTY(BlueprintReadWrite, Category = "Road Mesh", EditAnyWhere, EditFixedSize)
  UStaticMesh *RoadMesh;

  UPROPERTY(BlueprintReadWrite, Category = "Road Mesh", EditAnyWhere, EditFixedSize)
  TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

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
