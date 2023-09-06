#pragma once
#include "OWB_WorldGenerator.h"


FVoxelFloatBuffer UVoxelOWBFunctionLibrary::SampleOWBHeight(
		const FVoxelOWBHeightmap& OWBHeightmap,
		const FVoxelIntPointBuffer& Position,
		bool bWaterChannel) const
{
	FVoxelFloatBufferStorage ReturnValue;
	ReturnValue.Allocate(Position.Num());
	const UOpenWorldBakery* OWB = OWBHeightmap.OpenWorldBakery;
	ensure(OWB);
	if (OWB == NULL) {
		// No OWB set up here, quit!
		ForeachVoxelBufferChunk(Position.Num(), [&](const FVoxelBufferIterator& Iterator) {
			float* iO = ReturnValue.GetData(Iterator);
			for (int i = 0; i < Iterator.Num(); ++i) {
				iO[i] = 0.0;
			}
		});
		return FVoxelFloatBuffer::Make(ReturnValue);
	}
	const int Width = OWB->MapWidth;
	const int Height = OWB->MapHeight;
	const int HalfWidth = Width / 2;
	const int HalfHeight = Height / 2;
	const float CellWidth = OWB->CellWidth;

	ForeachVoxelBufferChunk(Position.Num(), [&](const FVoxelBufferIterator& Iterator) {
		const int* iX = Position.X.GetData(Iterator);
		const int* iY = Position.Y.GetData(Iterator);
		float* iO = ReturnValue.GetData(Iterator);
		for (int i = 0; i < Iterator.Num(); ++i) {
			const int X = iX[i] + HalfWidth;
			const int Y = iY[i] + HalfHeight;

			if (X < 0 || X >= Width || Y < 0 || Y >= Height) {
				iO[i] = NoMapHeight;
				continue;
			}

			const FOWBSquareMeter& CookedGround = OWB->BakedHeightMap[X + Y * Width];

			const double Elevation = CookedGround.HeightByType(
				bWaterChannel ? EOWBMeshBlockTypes::FreshWater : EOWBMeshBlockTypes::Ground);

			const float HeightInVoxels = Elevation / CellWidth;
			iO[i] = HeightInVoxels;
		}
	});
	return FVoxelFloatBuffer::Make(ReturnValue);
}

FVoxelHeightmapRef UVoxelOWBFunctionLibrary::OWBLandmassHeightmap(const FVoxelOWBHeightmap& OWBHeightmap) {
	ensure(OWBHeightmap.OpenWorldBakery && OWBHeightmap.OpenWorldBakery->bCookedMapReady);

	if (OWBHeightmap.OpenWorldBakery == NULL || !OWBHeightmap.OpenWorldBakery->bCookedMapReady) {
		return {};
	}

	if (CachedUVHM == NULL) {
		CachedUVHM = NewObject<UVoxelHeightmap>(this);
		FVoxelHeightmap* ACachedVHM = new FVoxelHeightmap();
		TVoxelArray<uint16> Heights16;
		Heights16.SetNumUninitialized(OWBHeightmap.OpenWorldBakery->MapWidth * OWBHeightmap.OpenWorldBakery->MapHeight);
		const double MaxH = OWBHeightmap.OpenWorldBakery->OceanDeep + 1.0;
		for (int y = 0; y < OWBHeightmap.OpenWorldBakery->MapHeight; ++y) {
			for (int x = 0; x < OWBHeightmap.OpenWorldBakery->MapWidth; ++x) {
				const int N = y * OWBHeightmap.OpenWorldBakery->MapWidth + x;
				const double H = OWBHeightmap.OpenWorldBakery->BakedHeightMap[N].HeightByType(EOWBMeshBlockTypes::Ground);
				Heights16[N]
					= (H + OWBHeightmap.OpenWorldBakery->OceanDeep) / MaxH * MAX_uint16;
			}
		}
		ACachedVHM->Initialize(OWBHeightmap.OpenWorldBakery->MapWidth,
													 OWBHeightmap.OpenWorldBakery->MapHeight,
													 MoveTemp(Heights16));
		CachedVHM = TSharedPtr<const FVoxelHeightmap>(ACachedVHM);
	}
	FVoxelHeightmapRef Out{ CachedUVHM, {}, CachedVHM };

	return Out;
}

//
//void FOWB_WorldGenerator::Initialize(FVoxelRuntime* Runtime) {
//	TVoxelGeneratorInitializer<FOWB_WorldGenerator> Initializer(this);
//
//	Initializer.AddChannel<FVoxelDensityType>(FVoxelChannelNames::Density)
//		.Getter<FVoxelFloatDensity, &FOWB_WorldGenerator::GetDensity>()
//		.RangeGetter<FVoxelFloatDensity, &FOWB_WorldGenerator::GetDensityRange>();
//	Initializer.AddChannel<FVoxelColorType>(FVoxelChannelNames::Color)
//		.Getter<FColor, &FOWB_WorldGenerator::GetMaterialColor>();
//	//Initializer.AddChannel<FVoxelMaterialIdType>(FVoxelChannelNames::MaterialId)
//	//	.Getter<FVoxelMaterialId8, &FOWB_WorldGenerator::GetMaterialColor>();
//}
//
//FOWB_WorldGenerator::FOWB_WorldGenerator(UOpenWorldBakery* AOpenWorldBakery):
//	OpenWorldBakery(AOpenWorldBakery)
//{
//	check(OpenWorldBakery);
//	Prepare();
//}
//void FOWB_WorldGenerator::Prepare() {
//	MyOceanDeep = OWBHeightToVoxelHeight(OpenWorldBakery->OceanDeep);
//}
//
//int FOWB_WorldGenerator::VoxelXToOWBX(const float X) const {
//	return FMath::RoundToInt(X + OpenWorldBakery->MapWidth / 2);
//}
//int FOWB_WorldGenerator::VoxelYToOWBY(const float Y) const {
//	return FMath::RoundToInt(Y + OpenWorldBakery->MapHeight / 2);
//}
//
//double FOWB_WorldGenerator::VoxelZToOWBZ(const float Z) const {
//	return Z;
//}
//float FOWB_WorldGenerator::OWBZToVoxelZ(const double Z) const {
//	return Z;
//}
//
//FVoxelFloatDensity FOWB_WorldGenerator::GetDensity(FVector3d Position) const
//{
//	if (!IsValid(OpenWorldBakery) || OpenWorldBakery->Chunks.Num() == 0) {
//		return 10.0;
//	}
//
//	int iX = VoxelXToOWBX(Position.X);
//	int iY = VoxelYToOWBY(Position.Y);
//	double iZ = VoxelZToOWBZ(Position.Z);
//
//	if (iX < 1 || iX >= OpenWorldBakery->MapWidth || iY < 1 || iY >= OpenWorldBakery->MapHeight) {
//		return 10.0;
//	}
//
//	if (Position.Z <= MyOceanDeep) {
//		return Layer == EOWBMeshBlockTypes::Ground ? -10.0 : 10.0;
//	}
//
//	const FOWBSquareMeter& CookedGround = OpenWorldBakery->BakedHeightMap[iX + iY * OpenWorldBakery->MapWidth];
//
//	double Elevation = CookedGround.HeightByType(Layer);
//
//	const float HeightInVoxels = OWBHeightToVoxelHeight(Elevation);
//
//	// Positive value -> empty voxel
//	// Negative value -> full voxel
//	// The voxel value is clamped between -1 and 1. That can result in a bad gradient/normal. To solve that we divide it
//	FVoxelFloatDensity RetVal = (iZ - HeightInVoxels) / 5;
//	if (Layer == EOWBMeshBlockTypes::FreshWater && RetVal < -1.0) {
//		RetVal = 10.0;
//	}
//	return RetVal;
//}
//
//
//TVoxelRange<FVoxelFloatDensity> FOWB_WorldGenerator::GetDensityRange(const FVoxelBox& Bounds) const
//{
//	if (!IsValid(OpenWorldBakery) || OpenWorldBakery->Chunks.Num() == 0) {
//		return TVoxelRange<FVoxelFloatDensity>::Infinite();
//	}
//
//	FIntBox ABounds;
//
//
//	//UE_LOG(LogTemp, Log, TEXT("GetValueRangeImpl %i:%i:%i %i:%i:%i"),
//	//	ABounds.Min.X, ABounds.Min.Y, ABounds.Min.Z,
//	//	ABounds.Max.X, ABounds.Max.Y, ABounds.Max.Z
//	//);
//
//	ABounds.Min.X = VoxelXToOWBX(Bounds.Min.X);
//	ABounds.Max.X = VoxelXToOWBX(Bounds.Max.X);
//
//	ABounds.Min.Y = VoxelYToOWBY(Bounds.Min.Y);
//	ABounds.Max.Y = VoxelYToOWBY(Bounds.Max.Y);
//
//	ABounds.Min.Z = VoxelZToOWBZ(Bounds.Min.Z);
//	ABounds.Max.Z = VoxelZToOWBZ(Bounds.Max.Z);
//
//	TVoxelRange<FVoxelFloatDensity> Out = { 10.0, 10.0 };
//
//	if (
//		ABounds.Min.X >= OpenWorldBakery->MapWidth || ABounds.Max.X < 0
//		|| ABounds.Min.Y >= OpenWorldBakery->MapHeight || ABounds.Max.Y < 0
//		|| ABounds.Min.Z > OpenWorldBakery->ChunksLayout.MaxZVoxelOnMap
//		|| ABounds.Max.Z < MyOceanDeep)
//	{
//		return Out;
//	}
//
//	FIntVector2 Min(
//		FMath::Clamp(ABounds.Min.X - 1, 0, OpenWorldBakery->MapWidth - 1),
//		FMath::Clamp(ABounds.Min.Y - 1, 0, OpenWorldBakery->MapWidth - 1)
//	);
//	FIntVector2 Max(
//		FMath::Clamp(ABounds.Max.X + 1, 0, OpenWorldBakery->MapWidth - 1),
//		FMath::Clamp(ABounds.Max.Y + 1, 0, OpenWorldBakery->MapWidth - 1)
//	);
//
//	FIntVector2 MinChunk = {
//		Min.X / OpenWorldBakery->ChunksLayout.ChunkWidth,
//		Min.Y / OpenWorldBakery->ChunksLayout.ChunkHeight };
//	FIntVector2 MaxChunk = {
//		Max.X / OpenWorldBakery->ChunksLayout.ChunkWidth,
//		Max.Y / OpenWorldBakery->ChunksLayout.ChunkHeight };
//
//	Out = { -10.0, 10.0 };
//	float HighestPoint = -100000.0;
//	for (int x = MinChunk.X; x <= MaxChunk.X; x++) {
//		for (int y = MinChunk.Y; y <= MaxChunk.Y; y++) {
//			const FOWBMeshBlocks_set& ChunkData = OpenWorldBakery->Chunks[x + y * OpenWorldBakery->ChunksLayout.XChunks];
//			const FOWBMeshChunk* MeshChunk = ChunkData.ChunkContents.Find(Layer);
//			if (MeshChunk != NULL && MeshChunk->BlocksType == Layer) {
//				if ((ABounds.Min.Z >= MeshChunk->MinPoint.Z && ABounds.Min.Z < MeshChunk->MaxPoint.Z)
//						|| (ABounds.Max.Z >= MeshChunk->MinPoint.Z && ABounds.Max.Z < MeshChunk->MaxPoint.Z)
//						|| (ABounds.Min.Z < MeshChunk->MinPoint.Z && ABounds.Max.Z >= MeshChunk->MaxPoint.Z))
//				{
//					return Out;
//				}
//				if (HighestPoint < MeshChunk->MaxPoint.Z) {
//					HighestPoint = MeshChunk->MaxPoint.Z;
//				}
//			}
//		}
//	}
//	if (HighestPoint < ABounds.Min.Z) {
//		Out = { -10.0, -10.0 };
//	} else {
//		Out = { 10.0, 10.0 };
//	}
//	return Out;
//
//	//float HeightInVoxels;
//
//	//for (int x = Min.X; x < Max.X; x++) {
//	//	for (int y = Min.Y; y < Max.Y; y++) {
//	//		const FOWBSquareMeter& CookedGround = OpenWorldBakery->BakedHeightMap[x + y * OpenWorldBakery->MapWidth];
//
//	//		HeightInVoxels = OWBHeightToVoxelHeight(CookedGround.HeightByType(Layer));
//	//		if (HeightInVoxels >= ABounds.Min.Z && HeightInVoxels < Bounds.Max.Z) {
//	//			return Out;
//	//		}
//	//	}
//	//}
//	//if (HeightInVoxels < ABounds.Min.Z) {
//	//	Out = { -10.0, -10.0 };
//	//} else {
//	//	Out = { 10.0, 10.0 };
//	//}
//	//return Out;
//////	if (Out.Min > 0.0 || Out.Max < 0.0) {
////		FVector VMin = Bounds.Min;
////		FVector Vax = Bounds.Max;
////		float Min = Out.Min.GetStorage();
////		float Max = Out.Max.GetStorage();
////		FString Title = (Out.Min < 0.0 && Out.Max > 0.0) ? "*" : " ";
////		UE_LOG(LogTemp, Log, TEXT("%s %1.0f:%1.0f:%1.0f %1.0f:%1.0f:%1.0f %1.1f-%1.1f"),
////					 *Title, VMin.X, VMin.Y, VMin.Z,
////					 Vax.X, Vax.Y, Vax.Z,
////					 Min, Max);
//////	}
////
////	return Out;
//}
//
//float FOWB_WorldGenerator::OWBHeightToVoxelHeight(double GroundElevation) const {
//	return GroundElevation / OpenWorldBakery->CellWidth;
//}
//
//FColor FOWB_WorldGenerator::GetMaterialColor(FVector3d Position) const {
//	if (!IsValid(OpenWorldBakery) || OpenWorldBakery->Chunks.Num() == 0) {
//		return { 0,0,0 };
//	}
//
//	int iX = VoxelXToOWBX(Position.X);
//	int iY = VoxelYToOWBY(Position.Y);
//	double iZ = VoxelZToOWBZ(Position.Z);
//
//	if (iX < 1 || iX >= OpenWorldBakery->MapWidth || iY < 1 || iY >= OpenWorldBakery->MapHeight) {
//		return { 0,0,0 };
//	}
//
//	if (Position.Z <= MyOceanDeep) {
//		return { 0,0,0 };
//	}
//
//	const FOWBSquareMeter& CookedGround = OpenWorldBakery->BakedHeightMap[iX + iY * OpenWorldBakery->MapWidth];
//
//	return OpenWorldBakery->TerrainVoxelColor(CookedGround).ToFColor(false);
//}
//
//////
//////uint8 UOWB_WorldGenerator::MaterialID_FromSUrfaceType(EOWBGroundSurfaceTypes SurfaceType) {
//////	uint8 Out = 0;
//////	if (SurfaceTypeMapping.Contains(SurfaceType)) {
//////		Out = SurfaceTypeMapping[SurfaceType];
//////	}
//////	return Out;
//////}
////
////FVoxelMaterial FOWB_VoxelWorldGeneratorInstance::GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
////{
////	int iX = VoxelXToOWBX(X);
////	int iY = VoxelYToOWBY(Y);
////	const FOWBSquareMeter& CookedGround = OpenWorldBakery->BakedHeightMap[iX + iY * OpenWorldBakery->MapWidth];
////
//////	const OpenWorldBakery::FSquareMeter& Ground = OpenWorldBakery->Ground(iX, iY);
////
////	// return FVoxelMaterial::CreateFromColor(FColor::Red);
////	FVoxelMaterialBuilder NewMaterial;
////	if (Generator.Layer == EOWBMeshBlockTypes::Ground) {
////		if (MaterialConfig == EVoxelMaterialConfig::RGB)
////			return FVoxelMaterial::CreateFromColor(OpenWorldBakery->TerrainVoxelColor(CookedGround));
////		else if (MaterialConfig == EVoxelMaterialConfig::SingleIndex) {
////			NewMaterial.SetMaterialConfig(EVoxelMaterialConfig::SingleIndex);
////			NewMaterial.SetSingleIndex((uint8)CookedGround.SurfaceType);
////			return NewMaterial.Build();
////		}
////		else if (CookedGround.SurfaceType == CookedGround.SurfaceTypeAdditional) {
////			NewMaterial.SetMaterialConfig(EVoxelMaterialConfig::MultiIndex);
////			NewMaterial.AddMultiIndex((int)CookedGround.SurfaceType, 1.0);
////			return NewMaterial.Build();
////		}
////		else {
////			NewMaterial.SetMaterialConfig(EVoxelMaterialConfig::MultiIndex);
////			NewMaterial.AddMultiIndex((int)CookedGround.SurfaceType, 0.7);
////			NewMaterial.AddMultiIndex((int)CookedGround.SurfaceTypeAdditional, 0.3);
////			return NewMaterial.Build();
////		}
////	}
////	else {
////		FVector2D NormalAsColor = CookedGround.Stream;
////		//NormalAsColor.X = 0.1 * FMath::RoundToFloat(NormalAsColor.X * 10);
////		NormalAsColor.Y *= -1;
////		NormalAsColor = (NormalAsColor + FVector2D(1.0, 1.0)) / 2;
////
////		float Deep = FMath::Clamp(
////			(float)sqrt((CookedGround.WaterSurface - CookedGround.GroundSurface) / OpenWorldBakery->CellWidth) / 10
////			, 0.0f, 1.0f
////		);
////
////		FColor Color;
////
////		Color.R = 255 * NormalAsColor.X;
////		Color.G = 255 * NormalAsColor.Y;
////		Color.B = 255 * Deep;
////		Color.A = 255;
////		return FVoxelMaterial::CreateFromColor(Color);
////	}
////}
////
//
