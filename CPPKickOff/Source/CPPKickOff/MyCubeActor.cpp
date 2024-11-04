// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCubeActor.h"

// Sets default values
AMyCubeActor::AMyCubeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyCubeActor::BeginPlay()
{
	Super::BeginPlay();

	RandomNumber = FMath::RandRange(0, 100);
	
}

// Called every frame
void AMyCubeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

