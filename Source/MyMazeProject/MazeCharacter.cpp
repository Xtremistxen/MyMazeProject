
#include "MazeCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"

AMazeCharacter::AMazeCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    // Character follows movement direction
    bUseControllerRotationYaw = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate =
        FRotator(0.0f, 500.0f, 0.0f);

    GetCharacterMovement()->MaxWalkSpeed = 500.0f;

    // Create the third-person camera boom
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(
        TEXT("CameraBoom"));

    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 350.0f;
    CameraBoom->bUsePawnControlRotation = true;

    // Create the follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(
        TEXT("FollowCamera"));

    FollowCamera->SetupAttachment(
        CameraBoom, USpringArmComponent::SocketName);

    FollowCamera->bUsePawnControlRotation = false;
}

void AMazeCharacter::BeginPlay()
{
    Super::BeginPlay();
}

// Bind the keyboard and mouse input mappings
void AMazeCharacter::SetupPlayerInputComponent(
    UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(
        TEXT("MoveFB"), this, &AMazeCharacter::MoveFB);

    PlayerInputComponent->BindAxis(
        TEXT("MoveLR"), this, &AMazeCharacter::MoveLR);

    PlayerInputComponent->BindAxis(
        TEXT("Rotate"), this, &AMazeCharacter::Rotate);

    PlayerInputComponent->BindAxis(
        TEXT("LookUp"), this, &AMazeCharacter::LookUp);
}

// Move forward or backward
void AMazeCharacter::MoveFB(float Value)
{
    if (Controller && !FMath::IsNearlyZero(Value))
    {
        const FRotator ControlRotation =
            Controller->GetControlRotation();

        const FRotator YawRotation(
            0.0f, ControlRotation.Yaw, 0.0f);

        const FVector Direction =
            FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        AddMovementInput(Direction, Value);
    }
}

// Move left or right
void AMazeCharacter::MoveLR(float Value)
{
    if (Controller && !FMath::IsNearlyZero(Value))
    {
        const FRotator ControlRotation =
            Controller->GetControlRotation();

        const FRotator YawRotation(
            0.0f, ControlRotation.Yaw, 0.0f);

        const FVector Direction =
            FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        AddMovementInput(Direction, Value);
    }
}

// Rotate camera horizontally
void AMazeCharacter::Rotate(float Value)
{
    AddControllerYawInput(Value);
}

// Rotate camera vertically
void AMazeCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}


