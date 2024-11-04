// Copyright Epic Games, Inc. All Rights Reserved.

#include "CPPKickOffCharacter.h"
#include "CPPKickOffProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "MyCubeActor.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ACPPKickOffCharacter

ACPPKickOffCharacter::ACPPKickOffCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void ACPPKickOffCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////// Input



void ACPPKickOffCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACPPKickOffCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACPPKickOffCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void ACPPKickOffCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ACPPKickOffCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACPPKickOffCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (FirstPersonCameraComponent && GetWorld())
	{
		// Do a line trace in front of the actor
		FHitResult Hit;
		FVector Start = FirstPersonCameraComponent->GetComponentLocation();
		FVector End = Start + FirstPersonCameraComponent->GetForwardVector() * 1000;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams);
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0, 0, 1);

		// If we hit something, log it
		if (Hit.bBlockingHit)
		{
			UE_LOG(LogTemplateCharacter, Log, TEXT("Hit: %s"), *Hit.GetActor()->GetActorLabel());

			// If we hit a MyCubeActor, log the RandomNumber
			if (Hit.GetActor())
			{
				AMyCubeActor* CubeActor = Cast<AMyCubeActor>(Hit.GetActor());

				if (CubeActor)
				{
					UE_LOG(LogTemplateCharacter, Log, TEXT("RandomNumber %i"), CubeActor->RandomNumber);

					// Destroy the CubeActor if it has been in the crosshair for 200 frames
					if (PreviousCubeActor && CubeActor == PreviousCubeActor)
					{
						FrameCounter++;
						if (FrameCounter >= 200)
						{
							CubeActor->Destroy();
							ResetCubeActor();
						}
						else
						{
							UE_LOG(LogTemplateCharacter, Log, TEXT("FrameCounter %i"), FrameCounter);
						}
					}
					else
					{
						PreviousCubeActor = CubeActor;
						FrameCounter = 0;
					}
				}
				else
				{
					ResetCubeActor();
				}
			}
		}
	}
}

void ACPPKickOffCharacter::ResetCubeActor()
{
	PreviousCubeActor = nullptr;
	FrameCounter = 0;
}
