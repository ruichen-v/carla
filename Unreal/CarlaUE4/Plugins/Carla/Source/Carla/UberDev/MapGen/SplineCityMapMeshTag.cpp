// Copyright 2018 Rui Chen. All Rights Reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "SplineCityMapMeshTag.h"

#include "Package.h"

#include <array>

ESplineCityMapMeshTag SplineCityMapMeshTag::GetBaseMeshTag()
{
  return ESplineCityMapMeshTag::RoadTwoLanes_NoSide;
}

uint32 SplineCityMapMeshTag::GetRoadIntersectionSize()
{
	// _MARK TODO discard if dynamically done in map generator
  return 5u;
}

FString SplineCityMapMeshTag::ToString(ESplineCityMapMeshTag Tag)
{
  const UEnum* ptr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESplineCityMapMeshTag"), true);
  if(!ptr)
    return FString("Invalid");
  return ptr->GetNameStringByIndex(static_cast<int32>(Tag));
}
