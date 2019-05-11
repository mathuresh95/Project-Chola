// Copy Pasta no Jutsu

#include "VRCharacter.h"
#include"Camera/CameraComponent.h"
#include"Components/SkeletalMeshComponent.h"
#include"Components/StaticMeshComponent.h"
#include"DrawDebugHelpers.h"
#include"TimerManager.h"
#include"AI/Navigation/NavigationSystem.h"
#include"Components/PostProcessComponent.h"
#include"Materials/MaterialInstanceDynamic.h"


// Sets default values
AVRCharacter::AVRCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	Camera->SetupAttachment(VRRoot);
	Camera->GetForwardVector();

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestinationMarker"));
	//CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Character Mesh"));
	//CharacterMesh->SetupAttachment(GetRootComponent());

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("Post Process Component"));
	PostProcessComponent->SetupAttachment(GetRootComponent());
	//PostProcessComponent->AddOrUpdateBlendable()

}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	DestinationMarker->SetVisibility(false);
	if (BlinkerBaseMaterial!=nullptr)
	{
		BlinkerMaterialInstance = UMaterialInstanceDynamic::Create(BlinkerBaseMaterial, this);
		PostProcessComponent->AddOrUpdateBlendable(BlinkerMaterialInstance);
		BlinkerMaterialInstance->SetScalarParameterValue(FName("Radius"), 0.2);
	}
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewCameraOffset;

	NewCameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	NewCameraOffset.Z = 0;
	AddActorWorldOffset(NewCameraOffset);
	VRRoot->AddWorldOffset(-NewCameraOffset);

	UpdateDestinationMarker();
}
bool AVRCharacter::FindTeleportDestination(FVector & OutLocation)
{
	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetForwardVector()*TeleportRange;

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
	DrawDebugLine(GetWorld(), Start, End, FColor::Black, false, 1.0f, 1.0f, 1.0f);

	if (!bHit)
	{
		return false;
	}
	FNavLocation NavLocation;
	bool bOnNavMesh = GetWorld()->GetNavigationSystem()->ProjectPointToNavigation(HitResult.Location, NavLocation, TeleportProjectionExtent);

	if (!bOnNavMesh)
	{
		return false;
	}

	OutLocation = NavLocation.Location;
	return bHit && bOnNavMesh;
}
void AVRCharacter::UpdateDestinationMarker()
{

	FVector OutLocation;
	bool bHasDestination = FindTeleportDestination(OutLocation);
	if (bHasDestination)
	{
		DestinationMarker->SetWorldLocation(OutLocation); //HitResult.Location
		DestinationMarker->SetVisibility(true);
	}
	else
	{
		DestinationMarker->SetVisibility(false);
	}

}

void AVRCharacter::StartCameraFade(float FromAlpha, float ToAlpha)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->PlayerCameraManager->StartCameraFade(FromAlpha, ToAlpha, TeleportFadeTime, FColor::Black);
	}
}

void AVRCharacter::BeginTeleport()
{
	StartCameraFade(0, 1);

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AVRCharacter::EndTeleport, TeleportFadeTime);

}

void AVRCharacter::EndTeleport()
{
	StartCameraFade(1, 0);
	SetActorLocation(DestinationMarker->GetComponentLocation());
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AVRCharacter::MoveRight);
	PlayerInputComponent->BindAction(TEXT("Teleport"), EInputEvent::IE_Released, this, &AVRCharacter::BeginTeleport);

}

void AVRCharacter::MoveForward(float Throttle)
{
	AddMovementInput(Camera->GetForwardVector()*Throttle);
}

void AVRCharacter::MoveRight(float Throttle)
{
	AddMovementInput(Camera->GetRightVector()*Throttle);
}


