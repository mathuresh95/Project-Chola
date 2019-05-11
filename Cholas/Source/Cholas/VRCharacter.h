// Copy Pasta no Jutsu

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRCharacter.generated.h"

UCLASS()
class CHOLAS_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	//CAMERA AND VR ROOT PARAMETERS
	UPROPERTY(VisibleAnywhere, Category = Setup)
		class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, Category = Setup)
		class USceneComponent* VRRoot;
	UPROPERTY(VisibleAnywhere, Category = Setup)
		class UStaticMeshComponent* DestinationMarker;
	UPROPERTY(EditAnywhere, Category = Setup)
		class USkeletalMeshComponent* CharacterMesh;

	//BLINKER PARAMETERS
	UPROPERTY(VisibleAnywhere, Category = Setup)
		class UPostProcessComponent* PostProcessComponent;
	UPROPERTY(EditAnywhere, Category = Setup)
		class UMaterialInterface* BlinkerBaseMaterial;
	UPROPERTY(VisibleAnywhere, Category = Setup)
		class UMaterialInstanceDynamic* BlinkerMaterialInstance;

	//TELEPORT PARAMETERS
	UPROPERTY(EditAnywhere, Category = Setup)
		float TeleportRange = 1000.0f;
	UPROPERTY(EditAnywhere, Category = Setup)
		float TeleportFadeTime = 1.0f;
	UPROPERTY(EditAnywhere, Category = Setup)
		FVector TeleportProjectionExtent = FVector(100, 100, 100);
	
	


private:
	void MoveForward(float Throttle);
	void MoveRight(float Throttle);

	bool FindTeleportDestination(FVector &OutLocation);
	void UpdateDestinationMarker();
	void StartCameraFade(float FromAlpha, float ToAlpha);
	void BeginTeleport();
	void EndTeleport();


};
