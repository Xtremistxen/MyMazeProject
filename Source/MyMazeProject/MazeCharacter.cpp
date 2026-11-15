
#include "MazeCharacter.h"
#include "Engine/Engine.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/DamageEvents.h"


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

    // Start the player at full health
    _currentHealth = maxHealth;
    bIsDead = false;
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
    if (bIsDead)
    {
        return;
    }

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
    if (bIsDead)
    {
        return;
    }

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
    if (bIsDead)
    {
        return;
    }

    AddControllerYawInput(Value);
}

// Rotate camera vertically
void AMazeCharacter::LookUp(float Value)
{
    if (bIsDead)
    {
        return;
    }

    AddControllerPitchInput(Value);
}


float AMazeCharacter::TakeDamage(
    float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    const float ActualDamage = Super::TakeDamage(
        DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // Ignore damage after death or non-positive damage
    if (bIsDead || ActualDamage <= 0.0f)
    {
        return 0.0f;
    }

    // Subtract damage without allowing negative health
    const float DamageTaken = FMath::Min(_currentHealth, ActualDamage);
    _currentHealth = FMath::Max(0.0f, _currentHealth - DamageTaken);

    UE_LOG(LogTemp, Warning, TEXT("Player Health: %.0f / %.0f"),
        _currentHealth, maxHealth);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            5.0f,
            FColor::Green,
            FString::Printf(
                TEXT("Player Health: %.0f / %.0f"),
                _currentHealth,
                maxHealth
            )
        );
    }

    // Die when health reaches zero
    if (_currentHealth <= 0.0f)
    {
        Die();
    }

    return DamageTaken;
}

void AMazeCharacter::Die()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;

    // Stop movement and prevent further movement
    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();

    // Freeze player rotation
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;

    // Play the death animation once if assigned
    if (DeathMontage && GetMesh())
    {
        if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
        {
            AnimInstance->Montage_Play(DeathMontage);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Player has died!"));
}



