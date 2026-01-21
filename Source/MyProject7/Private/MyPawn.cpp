// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"
#include "MyPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);
	CapsuleComp->SetSimulatePhysics(false);
	
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetupAttachment(CapsuleComp);
	SkeletalMeshComp->SetSimulatePhysics(false);
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(CapsuleComp);
	SpringArmComp->TargetArmLength = 150.0f;
	SpringArmComp->bUsePawnControlRotation = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	MaxSpeed = 100.0f;
	SprintSpeed = 200.0f;
	bIsSprinting = false;
	bIsFlying = false;
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyPawn::Move(const FInputActionValue& value)
{
	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X) || !FMath::IsNearlyZero(MoveInput.Y))
	{
		float DeltaTime = GetWorld()->GetDeltaSeconds();

		float TagetSpeed = bIsSprinting ? SprintSpeed : MaxSpeed;

		FVector DeltaLocation(MoveInput.X * TagetSpeed * DeltaTime, MoveInput.Y * TagetSpeed * DeltaTime, 0.f);

		AddActorLocalOffset(DeltaLocation, true);
		
		CurrentGroundSpeed = TagetSpeed;
	}
	else
	{
		CurrentGroundSpeed = 0.0f;
	}
}

void AMyPawn::Look(const FInputActionValue& value)
{
	const FVector2D LookInput = value.Get<FVector2D>();
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	float RotationSpeed = 45.0f;

	if (!FMath::IsNearlyZero(LookInput.X))
	{
		FRotator NewRotation(0.f, LookInput.X * RotationSpeed * DeltaTime, 0.f);
		AddActorLocalRotation(NewRotation);
	}

	if (!FMath::IsNearlyZero(LookInput.Y))
	{
		float NewPitch = SpringArmComp->GetRelativeRotation().Pitch - (LookInput.Y * RotationSpeed * DeltaTime);
		NewPitch = FMath::Clamp(NewPitch, -60.f, 60.f);
		SpringArmComp->SetRelativeRotation(FRotator(NewPitch, 0.f, 0.f));
	}

}

void AMyPawn::StartSprint(const FInputActionValue& value)
{
	bIsSprinting = true;
}

void AMyPawn::StopSprint(const FInputActionValue& value)
{
	bIsSprinting = false;
}

void AMyPawn::VerticalMove(const FInputActionValue& value)
{
	const float VerticalInput = value.Get<float>();

	if (!bIsFlying && VerticalInput < 0.f)
	{
		FVector CurrentLocation = GetActorLocation();
		if (CurrentLocation.Z > 20.0f)
		{
			CurrentLocation.Z = 20.0f;
			SetActorLocation(CurrentLocation);
		}
		CurrentVerticalSpeed = 0.0f;
		return;
	}

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	float CurrentSpeed = bIsSprinting ? SprintSpeed : MaxSpeed;	
	FVector DeltaLocation(0.f, 0.f, VerticalInput * CurrentSpeed * DeltaTime);

	AddActorLocalOffset(DeltaLocation, true);
	CurrentVerticalSpeed = FMath::Abs(VerticalInput * CurrentSpeed);
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult GroundHit;
	FVector Start = GetActorLocation();
	float TraceLength = CapsuleComp->GetScaledCapsuleHalfHeight() + 5.0f;
	FVector End = Start - FVector(0, 0, TraceLength);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bIsFlying = !GetWorld()->LineTraceSingleByChannel(GroundHit, Start, End, ECC_Visibility, Params);
}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &AMyPawn::Move);
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Completed, this, &AMyPawn::Move);
			}
			
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &AMyPawn::Look);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this, &AMyPawn::StartSprint);
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &AMyPawn::StopSprint);
			}
			if (PlayerController->VerticalMoveAction)
			{
				EnhancedInput->BindAction(PlayerController->VerticalMoveAction, ETriggerEvent::Triggered, this, &AMyPawn::VerticalMove);
			}
		}
	}

}

