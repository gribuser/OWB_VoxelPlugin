#include "OWB_WorldGenerator.h"

FOWB_WorldGenerator::FOWB_WorldGenerator(UOpenWorldBakery* AOpenWorldBakery):
	OpenWorldBakery(AOpenWorldBakery)
{
	check(OpenWorldBakery);
	MyOceanDeep = OWBHeightToVoxelHeight(OpenWorldBakery->OceanDeep);
}

//int FOWB_WorldGenerator::VoxelXToOWBX(const v_flt X) const {
//	return FMath::RoundToInt(X + OpenWorldBakery->MapWidth / 2);
//}
//int FOWB_WorldGenerator::VoxelYToOWBY(const v_flt Y) const {
//	return FMath::RoundToInt(Y + OpenWorldBakery->MapHeight / 2);
//}
//
//double FOWB_WorldGenerator::VoxelZToOWBZ(const v_flt Z) const {
//	return Z * 2;
//}
//float FOWB_WorldGenerator::OWBZToVoxelZ(const double Z) const {
//	return Z / 2;
//}
//
float FOWB_WorldGenerator::GetValueImpl(float X, float Y, float Z, int LOD) const
{
	if (!IsValid(OpenWorldBakery) || OpenWorldBakery->Chunks.Num() == 0)
		return 10.0;

	int iX = VoxelXToOWBX(X);
	int iY = VoxelYToOWBY(Y);
	double iZ = VoxelZToOWBZ(Z);

	if (iX < 1 || iX >= OpenWorldBakery->MapWidth || iY < 1 || iY >= OpenWorldBakery->MapHeight) {
		return 10.0;
	}

	if (Z <= MyOceanDeep) {
		return Layer == EOWBMeshBlockTypes::Ground ? -10 : 10;
	}

	const FOWBSquareMeter& CookedGround = OpenWorldBakery->BakedHeightMap[iX + iY * OpenWorldBakery->MapWidth];

	double Elevation = CookedGround.HeightByType(Layer);

	const float HeightInVoxels = OWBHeightToVoxelHeight(Elevation);

	// Positive value -> empty voxel
	// Negative value -> full voxel
	// The voxel value is clamped between -1 and 1. That can result in a bad gradient/normal. To solve that we divide it
	float RetVal = (iZ - HeightInVoxels) / 5;
	if (Layer == EOWBMeshBlockTypes::FreshWater && RetVal < -1.0)
		RetVal = 10.0;
	return RetVal;
}

float FOWB_WorldGenerator::OWBHeightToVoxelHeight(double GroundElevation) const {
	return GroundElevation / OpenWorldBakery->CellWidth * 2;
}
////
////uint8 UOWB_WorldGenerator::MaterialID_FromSUrfaceType(EOWBGroundSurfaceTypes SurfaceType) {
////	uint8 Out = 0;
////	if (SurfaceTypeMapping.Contains(SurfaceType)) {
////		Out = SurfaceTypeMapping[SurfaceType];
////	}
////	return Out;
////}
//
//FVoxelMaterial FOWB_VoxelWorldGeneratorInstance::GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
//{
//	int iX = VoxelXToOWBX(X);
//	int iY = VoxelYToOWBY(Y);
//	const FOWBSquareMeter& CookedGround = OpenWorldBakery->BakedHeightMap[iX + iY * OpenWorldBakery->MapWidth];
//
////	const OpenWorldBakery::FSquareMeter& Ground = OpenWorldBakery->Ground(iX, iY);
//
//	// return FVoxelMaterial::CreateFromColor(FColor::Red);
//	FVoxelMaterialBuilder NewMaterial;
//	if (Generator.Layer == EOWBMeshBlockTypes::Ground) {
//		if (MaterialConfig == EVoxelMaterialConfig::RGB)
//			return FVoxelMaterial::CreateFromColor(OpenWorldBakery->TerrainVoxelColor(CookedGround));
//		else if (MaterialConfig == EVoxelMaterialConfig::SingleIndex) {
//			NewMaterial.SetMaterialConfig(EVoxelMaterialConfig::SingleIndex);
//			NewMaterial.SetSingleIndex((uint8)CookedGround.SurfaceType);
//			return NewMaterial.Build();
//		}
//		else if (CookedGround.SurfaceType == CookedGround.SurfaceTypeAdditional) {
//			NewMaterial.SetMaterialConfig(EVoxelMaterialConfig::MultiIndex);
//			NewMaterial.AddMultiIndex((int)CookedGround.SurfaceType, 1.0);
//			return NewMaterial.Build();
//		}
//		else {
//			NewMaterial.SetMaterialConfig(EVoxelMaterialConfig::MultiIndex);
//			NewMaterial.AddMultiIndex((int)CookedGround.SurfaceType, 0.7);
//			NewMaterial.AddMultiIndex((int)CookedGround.SurfaceTypeAdditional, 0.3);
//			return NewMaterial.Build();
//		}
//	}
//	else {
//		FVector2D NormalAsColor = CookedGround.Stream;
//		//NormalAsColor.X = 0.1 * FMath::RoundToFloat(NormalAsColor.X * 10);
//		NormalAsColor.Y *= -1;
//		NormalAsColor = (NormalAsColor + FVector2D(1.0, 1.0)) / 2;
//
//		float Deep = FMath::Clamp(
//			(float)sqrt((CookedGround.WaterSurface - CookedGround.GroundSurface) / OpenWorldBakery->CellWidth) / 10
//			, 0.0f, 1.0f
//		);
//
//		FColor Color;
//
//		Color.R = 255 * NormalAsColor.X;
//		Color.G = 255 * NormalAsColor.Y;
//		Color.B = 255 * Deep;
//		Color.A = 255;
//		return FVoxelMaterial::CreateFromColor(Color);
//	}
//}
//
//TVoxelRange<v_flt> FOWB_VoxelWorldGeneratorInstance::GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const
//{
//	if (!IsValid(OpenWorldBakery) || OpenWorldBakery->Chunks.Num() == 0)
//		return TVoxelRange<v_flt>::Infinite();
//
//	FVoxelIntBox ABounds = Bounds;
//
//	//UE_LOG(LogTemp, Log, TEXT("GetValueRangeImpl %i:%i:%i %i:%i:%i"),
//	//	ABounds.Min.X, ABounds.Min.Y, ABounds.Min.Z,
//	//	ABounds.Max.X, ABounds.Max.Y, ABounds.Max.Z
//	//);
//
//	ABounds.Min.X = VoxelXToOWBX(ABounds.Min.X);
//	ABounds.Max.X = VoxelXToOWBX(ABounds.Max.X);
//
//	ABounds.Min.Y = VoxelYToOWBY(ABounds.Min.Y);
//	ABounds.Max.Y = VoxelYToOWBY(ABounds.Max.Y);
//
//	//ABounds.Min.Z = VoxelZToOWBZ(ABounds.Min.Z);
//	//ABounds.Max.Z = VoxelZToOWBZ(ABounds.Max.Z);
//
//	TVoxelRange<v_flt> Out = { 10.0, 10.0 };
//
//	if (
//		ABounds.Min.X > OpenWorldBakery->MapWidth || ABounds.Max.X < 0
//		|| ABounds.Min.Y > OpenWorldBakery->MapHeight || ABounds.Max.Y < 0
//		|| ABounds.Min.Z > 2*OpenWorldBakery->ChunksLayout.MaxZVoxelOnMap)
//	{
//		//UE_LOG(LogTemp, Log, TEXT("EMPTY 1"));
//		return Out;
//	}
//
//	//return TVoxelRange<v_flt>::Infinite();
//
//	if (ABounds.Max.Z < MyOceanDeep) {
//		//UE_LOG(LogTemp, Log, TEXT("EMPTY 2"));
//		return Out;
//	}
//
//	FVoxelIntBox ChunkBounds = ABounds;
//	ChunkBounds.Min.X /= OpenWorldBakery->ChunksLayout.ChunkWidth;
//	ChunkBounds.Max.X /= OpenWorldBakery->ChunksLayout.ChunkWidth;
//	ChunkBounds.Min.Y /= OpenWorldBakery->ChunksLayout.ChunkHeight;
//	ChunkBounds.Max.Y /= OpenWorldBakery->ChunksLayout.ChunkHeight;
//
//	ChunkBounds.Min.X = FMath::Clamp(ChunkBounds.Min.X, 0, OpenWorldBakery->ChunksLayout.XChunks - 1);
//	ChunkBounds.Max.X = FMath::Clamp(ChunkBounds.Max.X, 0, OpenWorldBakery->ChunksLayout.XChunks - 1);
//	ChunkBounds.Min.Y = FMath::Clamp(ChunkBounds.Min.Y, 0, OpenWorldBakery->ChunksLayout.YChunks - 1);
//	ChunkBounds.Max.Y = FMath::Clamp(ChunkBounds.Max.Y, 0, OpenWorldBakery->ChunksLayout.YChunks - 1);
//
//	for (int x = ChunkBounds.Min.X; x <= ChunkBounds.Max.X; x++) {
//		for (int y = ChunkBounds.Min.Y; y <= ChunkBounds.Max.Y; y++) {
//			const FOWBMeshBlocks_set& ChunkData = OpenWorldBakery->Chunks[x + y * OpenWorldBakery->ChunksLayout.XChunks];
//			if (ChunkData.ChunkContents.Contains(Generator.Layer)) {
//				const FOWBMeshChunk& MeshChunk = ChunkData.ChunkContents[Generator.Layer];
//				if (MeshChunk.BlocksType == Generator.Layer) {
//					if (!(MeshChunk.MinPoint.X >= ABounds.Max.X
//						|| MeshChunk.MaxPoint.X <= ABounds.Min.X
//						|| MeshChunk.MinPoint.Y >= ABounds.Max.Y
//						|| MeshChunk.MaxPoint.Y <= ABounds.Min.Y)) {
//
//						float MinVal = (float)(ChunkBounds.Min.Z - MeshChunk.MaxPoint.Z) / 5.0;
//						float MaxVal = (float)(ChunkBounds.Max.Z - MeshChunk.MinPoint.Z) / 5.0;
//
//						// Ok, this one incide Bounds
//						if (Out.Min > MinVal)
//							Out.Min = MinVal;
//						if (Out.Max < MaxVal)
//							Out.Max = MaxVal;
//					}
//				}
//			}
//		}
//	}
//	//UE_LOG(LogTemp, Log, TEXT("CALC %1.1f - %1.1f"),Out.Min, Out.Max);
//
//	return Out;
//}
