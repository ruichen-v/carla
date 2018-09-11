// Copyright 2018 Rui Chen. All Rights Reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "StreetMapRuntime.h"
#include "StreetMapMeshTag.h"

#include "Package.h"

#include <array>

EStreetMapMeshTag StreetMapMeshTag::GetBaseMeshTag()
{
  return EStreetMapMeshTag::Road_2_Lanes_OneWay_Plain;
}

uint32 StreetMapMeshTag::GetRoadIntersectionSize()
{
	// _MARK TODO discard if dynamically done in map generator
  return 5u;
}

FString StreetMapMeshTag::ToString(EStreetMapMeshTag Tag)
{
  const UEnum* ptr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EStreetMapMeshTag"), true);
  if(!ptr)
    return FString("Invalid");
  return ptr->GetNameStringByIndex(static_cast<int32>(Tag));
}
