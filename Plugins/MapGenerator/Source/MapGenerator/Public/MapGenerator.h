// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"


class FMapGeneratorModule : public IModuleInterface,FSelfRegisteringExec
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void LevelInitFromTxT(const FString& Path);

	typedef AActor* (FMapGeneratorModule::* CreateObj)(const int32 X, const int32 Y, UWorld* InWorld);

	void LevelInitFromPng(const FString& Path);
	virtual bool Exec(class UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

	
	AActor* CreateWall(const int32 X, const int32 Y, UWorld* InWorld);
	AActor* CreateDamageableWall(const int32 X, const int32 Y, UWorld* InWorld);

	TMap<FColor, CreateObj> MapCreatorFunctions;

};
