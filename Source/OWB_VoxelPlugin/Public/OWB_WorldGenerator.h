// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "OpenWorldBakery.h"
#include "VoxelWorldGenerators/VoxelWorldGeneratorHelpers.h"
#include "OWB_WorldGenerator.generated.h"

 UCLASS(Blueprintable)
 class UOWB_WorldGenerator : public UVoxelWorldGenerator
 {
	 GENERATED_BODY()

 public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Open World Bakery")
	UOpenWorldBakery* OpenWorldBakery = NULL;

	//~ Begin UVoxelWorldGenerator Interface
	virtual TVoxelSharedRef<FVoxelWorldGeneratorInstance> GetInstance() override;
	//~ End UVoxelWorldGenerator Interface

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Open World Bakery")
	TEnumAsByte<EOWBMeshBlockTypes> Layer = EOWBMeshBlockTypes::Ground;
 };


 class FOWB_VoxelWorldGeneratorInstance : public TVoxelWorldGeneratorInstanceHelper<FOWB_VoxelWorldGeneratorInstance, UOWB_WorldGenerator>
 {
 public:
	 explicit FOWB_VoxelWorldGeneratorInstance(const UOWB_WorldGenerator& MyGenerator);

	 //~ Begin FVoxelWorldGeneratorInstance Interface
	 virtual void Init(const FVoxelWorldGeneratorInit& InitStruct) override;

	 v_flt GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const;
	 FVoxelMaterial GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const;

	 TVoxelRange<v_flt> GetValueRangeImpl(const FIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const;

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
 };