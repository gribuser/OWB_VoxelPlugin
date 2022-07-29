#pragma once
#include "CoreMinimal.h"
#include "VoxelGenerators/Hack_PrimitiveVoxelGenerator.h"
#include "OpenWorldBakery.h"
#include "OWB_WorldGenerator.generated.h"

USTRUCT(Blueprintable)
struct OWB_VOXELPLUGIN_API FOWB_WorldGenerator : public FPrimitiveVoxelGenerator {
	GENERATED_BODY()
public:
	FOWB_WorldGenerator(UOpenWorldBakery* AOpenWorldBakery);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Open World Bakery")
		UOpenWorldBakery* OpenWorldBakery = NULL;
	FOWB_WorldGenerator& operator =(const FOWB_WorldGenerator& SRC) {
		OpenWorldBakery = SRC.OpenWorldBakery;
		Layer = SRC.Layer;
		return *this;
	}
	FOWB_WorldGenerator() {}

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Open World Bakery")
	EOWBMeshBlockTypes Layer = EOWBMeshBlockTypes::Ground;

		//uint8 MaterialID_FromSUrfaceType(EOWBGroundSurfaceTypes SurfaceType);


	float GetValueImpl(float X, float Y, float Z, int LOD) const;
	// FVoxelMaterial GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const;

	// TVoxelRange<v_flt> GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const;


	int VoxelXToOWBX(const float X) const;
	int VoxelYToOWBY(const float Y) const;
	double VoxelZToOWBZ(const float Z) const;
	float OWBZToVoxelZ(const double Z) const;
	float OWBHeightToVoxelHeight(double GroundElevation) const;
	double MyOceanDeep = 0.0;
};