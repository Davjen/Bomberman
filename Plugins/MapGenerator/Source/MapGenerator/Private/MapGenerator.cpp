// Copyright Epic Games, Inc. All Rights Reserved.

#include "MapGenerator.h"
#include "AssetRegistryModule.h"
#include "Engine/StaticMeshActor.h"
#include "Factories/WorldFactory.h"
#include "ImageUtils.h"
#include "Misc/FileHelper.h"


#define LOCTEXT_NAMESPACE "FMapGeneratorModule"

void FMapGeneratorModule::StartupModule()
{
	MapCreatorFunctions.Add(FColor::FromHex(FString("000000FF")), &FMapGeneratorModule::CreateWall);
	MapCreatorFunctions.Add(FColor::FromHex(FString("FF0000FF")), &FMapGeneratorModule::CreateDamageableWall);
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FMapGeneratorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FMapGeneratorModule::LevelInitFromTxT(const FString& Path)
{

	TArray<FString>Datas;
	FFileHelper::LoadFileToStringArray(Datas, *Path);

	UWorldFactory* NewWorld = NewObject<UWorldFactory>();
	uint64 SuffixName = FPlatformTime::Cycles64();
	FString AssetName = FString::Printf(TEXT("M_New_Level_%llu"), SuffixName);

	UPackage* Package = CreatePackage(*FString::Printf(TEXT("/Game/Level/%s"), *AssetName));
	UObject* NewWorldObj = NewWorld->FactoryCreateNew(NewWorld->SupportedClass, Package, *AssetName, EObjectFlags::RF_Standalone | EObjectFlags::RF_Public, nullptr, GWarn);
	FAssetRegistryModule::AssetCreated(NewWorldObj);
	UWorld* WorldCasted = Cast<UWorld>(NewWorldObj);


	WorldCasted->Modify();
	int32 StartX = 0;
	int32 StartY = 0;
	float DeltaIncrement = 50.f;
	float PlaneDimensionX = 0;
	float PlaneDimensionY = Datas.Num();

	UE_LOG(LogTemp, Warning, TEXT("DataNum : %d"), Datas.Num());

	//CreateWall(StartX, StartY, WorldCasted);

	/*Instantiate Floor*/


	for (int32 Y = 0; Y < Datas.Num(); Y++)
	{
		TArray<FString> Row;
		Datas[Y].ParseIntoArray(Row, TEXT(","));
		for (int32 X = 0; X < Row.Num(); X++)
		{
			if (Row[X].Equals(FString("1")))
			{
				CreateWall(StartX, StartY, WorldCasted);
			}
			StartX += 100;
		}
		StartX = 0;
		StartY += 100;
	}

	WorldCasted->PostEditChange();
	WorldCasted->MarkPackageDirty();
}

bool FMapGeneratorModule::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	if (FParse::Command(&Cmd, TEXT("InitWorld")))
	{
		FString Path = FParse::Token(Cmd, true);
		UE_LOG(LogTemp, Warning, TEXT("path: %s"), *Path);

		LevelInitFromTxT(Path);
		return true;
	}
	if (FParse::Command(&Cmd, TEXT("InitWorldpng")))
	{
		FString Path = FParse::Token(Cmd, true);
		UE_LOG(LogTemp, Warning, TEXT("path: %s"), *Path);

		LevelInitFromPng(Path);
		return true;
	}
	if (FParse::Command(&Cmd, TEXT("HI")))
	{
		FString TestWall = FString("Blueprint'/Game/DamageableWall.DamageableWall'");
		UBlueprint* Bp = LoadObject<UBlueprint>(nullptr,*TestWall);
		AActor* MyActor = InWorld->SpawnActor<AActor>(Bp->GeneratedClass);
	}
	return false;
}

void FMapGeneratorModule::LevelInitFromPng(const FString& Path)
{
	UWorldFactory* NewWorld = NewObject<UWorldFactory>();
	uint64 SuffixName = FPlatformTime::Cycles64();
	FString AssetName = FString::Printf(TEXT("M_New_Level_%llu"), SuffixName);

	UPackage* Package = CreatePackage(*FString::Printf(TEXT("/Game/Level/%s"), *AssetName));
	UObject* NewWorldObj = NewWorld->FactoryCreateNew(NewWorld->SupportedClass, Package, *AssetName, EObjectFlags::RF_Standalone | EObjectFlags::RF_Public, nullptr, GWarn);
	FAssetRegistryModule::AssetCreated(NewWorldObj);
	UWorld* WorldCasted = Cast<UWorld>(NewWorldObj);


	WorldCasted->Modify();

	UTexture2D* Texture = FImageUtils::ImportFileAsTexture2D(Path);
	int32 PlatformSizeX = Texture->GetSizeX();
	int32 PlatformSizeY = Texture->GetSizeY();
	int32 StartX = 0;
	int32 StartY = 0;

	const FColor* Pixels = static_cast<const FColor*>(Texture->PlatformData->Mips[0].BulkData.LockReadOnly());
	//LOCK DELLA MIPS
	for (int32 IndexY = 0; IndexY < PlatformSizeY; IndexY++)
	{
		for (int32 IndexX = 0; IndexX < PlatformSizeX; IndexX++)
		{
			const FColor CurrentPixel = Pixels[IndexX * PlatformSizeY + IndexY];
			FString Hex = CurrentPixel.ToHex();
			UE_LOG(LogTemp, Warning, TEXT("COLORE:%s"), *Hex);
			
			
			if (MapCreatorFunctions.Find(CurrentPixel))
			{
				(this->** MapCreatorFunctions.Find(CurrentPixel))(StartX, StartY, WorldCasted);
			}
			/*if (CurrentPixel.ToHex().Equals(FString("000000FF")))
			{
				UE_LOG(LogTemp, Warning, TEXT("COLOREadsasdasd:%s"), *Hex);
				CreateWall(StartX, StartY, WorldCasted);
			}
			else if (CurrentPixel.ToHex().Equals(FString("FF0000FF")))
			{
				UE_LOG(LogTemp, Warning, TEXT("COLORE muro distruttibile:%s"), *Hex);
				CreateDamageableWall(StartX, StartY, WorldCasted);
			}*/
			StartX += 100;
		}
		StartX = 0;
		StartY += 100;
	}
	Texture->PlatformData->Mips[0].BulkData.Unlock();

	WorldCasted->PostEditChange();
	WorldCasted->MarkPackageDirty();
}

AActor* FMapGeneratorModule::CreateWall(const int32 X, const int32 Y, UWorld* InWorld)
{
	UE_LOG(LogTemp, Warning, TEXT("CREATING A WALL"));
	FString WallPath = FString("StaticMesh'/Engine/BasicShapes/Cube.Cube'");
	UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *WallPath);
	if (StaticMesh)
	{
		AStaticMeshActor* Wall = InWorld->SpawnActor<AStaticMeshActor>();
		FVector Position;
		FTransform WallTransform;
		if (Wall->GetRootComponent())
		{
			Position = FVector(X, Y, Wall->GetActorRelativeScale3D().Z * 50);
			WallTransform.AddToTranslation(Position);
			Wall->SetActorTransform(WallTransform);
			Wall->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
			return Wall;
		}
	}
	return nullptr;
}


AActor* FMapGeneratorModule::CreateDamageableWall(const int32 X, const int32 Y, UWorld* InWorld)
{
	UE_LOG(LogTemp, Warning, TEXT("CREATING A DamageableWALL"));

	FString DestruWallPath = FString("Blueprint'/Game/DamageableWall.DamageableWall'");
	UBlueprint* Bp = LoadObject<UBlueprint>(nullptr, *DestruWallPath);
	if (Bp && Bp->GeneratedClass->IsChildOf<AActor>())
	{
		AActor* DestruWall = InWorld->SpawnActor<AActor>(Bp->GeneratedClass);
		FVector Position;
		FTransform WallTransform;
		if (DestruWall->GetRootComponent())
		{

			Position = FVector(X, Y, DestruWall->GetActorRelativeScale3D().Z * 50);
			WallTransform.AddToTranslation(Position);
			DestruWall->SetActorTransform(WallTransform);
			return DestruWall;
		}
	}
	return nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMapGeneratorModule, MapGenerator)