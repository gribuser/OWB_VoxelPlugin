// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "OWB_VoxelPlugin.h"

#define LOCTEXT_NAMESPACE "FOWB_VoxelPluginModule"

void FOWB_VoxelPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FOWB_VoxelPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOWB_VoxelPluginModule, OWB_VoxelPlugin)