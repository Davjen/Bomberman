// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageableWall.h"
#include "Factories/MaterialFactoryNew.h"

// Sets default values
ADamageableWall::ADamageableWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UE_LOG(LogTemp, Error, TEXT("SonoNAto"));


}

// Called when the game starts or when spawned
void ADamageableWall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADamageableWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

