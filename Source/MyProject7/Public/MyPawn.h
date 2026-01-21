// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class MYPROJECT7_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);
	UFUNCTION()
	void VerticalMove(const FInputActionValue& value);
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float MaxSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float CurrentGroundSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float CurrentVerticalSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFlying;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, category = "Capsule")
	UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, category = "SkeletalMesh")
	USkeletalMeshComponent* SkeletalMeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, category = "Camera")
	UCameraComponent* CameraComp;

};
