// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "OpenWorldBakery.h"
#include "VoxelGenerators/VoxelGeneratorHelpers.h"
#include "OWB_WorldGenerator.generated.h"

 UCLASS(Blueprintable)
 class UOWB_WorldGenerator : public UVoxelGenerator
 {
	 GENERATED_BODY()

 public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Open World Bakery")
	UOpenWorldBakery* OpenWorldBakery = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Open World Bakery")
	TMap<EOWBGroundSurfaceTypes, uint8> SurfaceTypeMapping = {
		{EOWBGroundSurfaceTypes::Unmarked, 0},
		{EOWBGroundSurfaceTypes::Swamp, 1},
		{EOWBGroundSurfaceTypes::Forest, 2},
		{EOWBGroundSurfaceTypes::Grass, 3},
		{EOWBGroundSurfaceTypes::Bush, 4},
		{EOWBGroundSurfaceTypes::RockWall, 5},
		{EOWBGroundSurfaceTypes::RockFlat, 6},
		{EOWBGroundSurfaceTypes::LakeShore, 7},
		{EOWBGroundSurfaceTypes::LakeBed, 8},
		{EOWBGroundSurfaceTypes::LakeShallow, 9},
		{EOWBGroundSurfaceTypes::SeaShoreSand, 10},
		{EOWBGroundSurfaceTypes::SeaShoreRock, 11},
		{EOWBGroundSurfaceTypes::SeaBed, 12},
		{EOWBGroundSurfaceTypes::SeaShallowSand, 13},
		{EOWBGroundSurfaceTypes::SeaShallowRock, 14},
		{EOWBGroundSurfaceTypes::RiverShoreSand, 15},
		{EOWBGroundSurfaceTypes::RiverShoreRock, 16},
		{EOWBGroundSurfaceTypes::RiverShoreWaterfall, 17},
		{EOWBGroundSurfaceTypes::RiverShallowSand, 18},
		{EOWBGroundSurfaceTypes::RiverShallowRock, 19},
		{EOWBGroundSurfaceTypes::RiverShallowWaterfall, 20},
		{EOWBGroundSurfaceTypes::RiverBed, 21},
		{EOWBGroundSurfaceTypes::SpringEasy, 22},
		{EOWBGroundSurfaceTypes::SpringHard, 23},
		{EOWBGroundSurfaceTypes::LandSlideAged, 24},
		{EOWBGroundSurfaceTypes::LandSlideSmooth, 25},
		{EOWBGroundSurfaceTypes::LandSlideRocky, 26},
		{EOWBGroundSurfaceTypes::ErrorTerrain, 27}
	};

	//~ Begin UVoxelWorldGenerator Interface
	virtual TVoxelSharedRef<FVoxelGeneratorInstance> GetInstance() override;
	//~ End UVoxelWorldGenerator Interface

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Open World Bakery")
	EOWBMeshBlockTypes Layer = EOWBMeshBlockTypes::Ground;

	uint8 MaterialID_FromSUrfaceType(EOWBGroundSurfaceTypes SurfaceType);
 };


 class FOWB_VoxelWorldGeneratorInstance : public TVoxelGeneratorInstanceHelper<FOWB_VoxelWorldGeneratorInstance, UOWB_WorldGenerator>
 {
 public:
	 using Super = TVoxelGeneratorInstanceHelper<FOWB_VoxelWorldGeneratorInstance, UOWB_WorldGenerator>;
	 explicit FOWB_VoxelWorldGeneratorInstance(UOWB_WorldGenerator& MyGenerator);

	 //~ Begin FVoxelWorldGeneratorInstance Interface
	 virtual void Init(const FVoxelWorldGeneratorInit& InitStruct) override;

	 v_flt GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const;
	 FVoxelMaterial GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const;

	 TVoxelRange<v_flt> GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const;

	 virtual FVector GetUpVector(v_flt X, v_flt Y, v_flt Z) const override final;
	 //~ End FVoxelWorldGeneratorInstance Interface

 private:
	 UOpenWorldBakery* OpenWorldBakery;
	 const UOWB_WorldGenerator& Generator;

	 int VoxelXToOWBX(const v_flt X) const;
	 int VoxelYToOWBY(const v_flt Y) const;
	 double VoxelZToOWBZ(const v_flt Z) const;
	 v_flt OWBZToVoxelZ(const double Z) const;
	 v_flt OWBHeightToVoxelHeight(double GroundElevation) const;
	 EVoxelMaterialConfig MaterialConfig;
	 double MyOceanDeep = 0.0;
 };