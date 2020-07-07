// Copyright 2020. Kangsan Bae all rights reserved.

#pragma once

#pragma pack(push, 1)

struct IF_SC100
{
	char object_ID[20];
	double latitude;
	double longitude;
	unsigned char object_Type;
	unsigned char isApproved;
	unsigned char accidentRiskLevel;
};

struct IF_SC200
{
	char location_Name[20];
	unsigned char accidentRiskType;
};

#pragma pack(pop)

