
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
#include "MazeEnemyCharacter.h"
#include "EngineUtils.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"


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

    // Press Q to activate our stun ability
    PlayerInputComponent->BindAction(
        TEXT("Stun"), IE_Pressed, this, &AMazeCharacter::ActivateStun);
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

// Activate the stun ability if it's off cooldown
void AMazeCharacter::ActivateStun()
{
    // Dead players can't use abilities
    if (bIsDead)
    {
        return;
    }

    const float CurrentTime = GetWorld()->GetTimeSeconds();

    // Check if our stun is still on cooldown
    if (CurrentTime - LastStunTime < StunCooldown)
    {
        const float TimeRemaining =
            StunCooldown - (CurrentTime - LastStunTime);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1, 2.0f, FColor::Yellow,
                FString::Printf(
                    TEXT("Stun Cooldown: %.1f seconds"),
                    TimeRemaining));
        }

        return;
    }

    // Start the cooldown
    LastStunTime = CurrentTime;

    // Show a message so we know the ability fired
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1, 2.0f, FColor::Cyan,
            TEXT("STUN ACTIVATED!"));
    }

    UE_LOG(LogTemp, Warning, TEXT("Player activated stun ability!"));

    // Check all enemies in the level
    for (TActorIterator<AMazeEnemyCharacter> It(GetWorld()); It; ++It)
    {
        AMazeEnemyCharacter* Enemy = *It;

        if (!IsValid(Enemy))
        {
            continue;
        }

        // Check how close the enemy is to the player
        const float Distance = FVector::Dist(
            GetActorLocation(),
            Enemy->GetActorLocation());


        if (Distance <= StunRadius)
        {
            // Enemy is close enough to receive the stun
            UE_LOG(LogTemp, Warning,
                TEXT("STUN HIT! Enemy distance: %.0f"), Distance);

            Enemy->ApplyStun(StunDuration);
        }
        else
        {
            // Enemy is outside our stun range
            UE_LOG(LogTemp, Warning,
                TEXT("STUN MISSED! Distance: %.0f | Radius: %.0f"),
                Distance, StunRadius);
        }

    }

    // Spawn our magical stun burst around the player
    if (StunEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            StunEffect,
            GetActorLocation(),
            GetActorRotation()
        );
    }
}


float AMazeCharacter::Heal(float HealAmount)
{
    // Don't heal dead players or accept negative healing
    if (bIsDead || HealAmount <= 0.0f)
    {
        return 0.0f;
    }

    // Remember our health before healing
    float OldHealth = _currentHealth;

    // Add health, but never go over max health
    _currentHealth = FMath::Clamp(
        _currentHealth + HealAmount,
        0.0f,
        maxHealth
    );

    // Figure out how much health we actually gained
    float ActualHealing = _currentHealth - OldHealth;

    // Print the new health so we can test our meat pickup
    UE_LOG(LogTemp, Warning,
        TEXT("Player healed for %.0f HP | Health: %.0f / %.0f"),
        ActualHealing,
        _currentHealth,
        maxHealth
    );

    return ActualHealing;
}


void AMazeCharacter::ApplySpeedBoost(float SpeedMultiplier, float Duration)
{
    // Make sure our speed boost values are valid
    if (SpeedMultiplier <= 1.0f || Duration <= 0.0f || bIsDead)
    {
        return;
    }

    UCharacterMovementComponent* Movement = GetCharacterMovement();

    if (!Movement)
    {
        return;
    }

    // Only save our normal speed when the first boost starts
    if (!GetWorldTimerManager().IsTimerActive(SpeedBoostTimerHandle))
    {
        OriginalWalkSpeed = Movement->MaxWalkSpeed;
    }

    // Give the player a temporary speed boost
    Movement->MaxWalkSpeed = OriginalWalkSpeed * SpeedMultiplier;

    // Restart the timer if another bread pickup is collected
    GetWorldTimerManager().ClearTimer(SpeedBoostTimerHandle);

    GetWorldTimerManager().SetTimer(
        SpeedBoostTimerHandle,
        this,
        &AMazeCharacter::ResetMovementSpeed,
        Duration,
        false
    );

    UE_LOG(LogTemp, Warning,
        TEXT("Bread speed boost activated! Speed: %.0f for %.1f seconds"),
        Movement->MaxWalkSpeed,
        Duration);
}

void AMazeCharacter::ResetMovementSpeed()
{
    // Bread wore off, so put our movement speed back
    UCharacterMovementComponent* Movement = GetCharacterMovement();

    if (Movement && OriginalWalkSpeed > 0.0f)
    {
        Movement->MaxWalkSpeed = OriginalWalkSpeed;

        UE_LOG(LogTemp, Warning,
            TEXT("Bread speed boost ended! Speed back to %.0f"),
            OriginalWalkSpeed);
    }
}





