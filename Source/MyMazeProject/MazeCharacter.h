

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MazeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
class UNiagaraSystem;

UCLASS()
class MYMAZEPROJECT_API AMazeCharacter : public ACharacter
{
    GENERATED_BODY()

public:
       AMazeCharacter();

    // Maximum player health
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float maxHealth = 100.0f;

    // Restore player health without going over max health
    // Returns how much health was actually restored
    UFUNCTION(BlueprintCallable, Category = "Combat")
    float Heal(float HealAmount);


    // Give the player a temporary speed boost from bread
    UFUNCTION(BlueprintCallable, Category = "Pickup")
    void ApplySpeedBoost(float SpeedMultiplier, float Duration);

    // Put the player's movement speed back to normal
    void ResetMovementSpeed();


    // Apply incoming damage and return the actual damage taken
    virtual float TakeDamage(
        float DamageAmount,
        struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator,
        AActor* DamageCauser) override;

    // Animation played once when the player dies
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* DeathMontage = nullptr;

    // ===== Stun Ability =====

// How far the stun reaches around the player
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun Ability")
    float StunRadius = 400.0f;

    // How long enemies stay stunned
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun Ability")
    float StunDuration = 3.0f;

    // Time before the player can stun again
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun Ability")
    float StunCooldown = 10.0f;

    // Particle effect played when the player uses stun
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun Ability")
    UNiagaraSystem* StunEffect = nullptr;

protected:
    virtual void BeginPlay() override;

    virtual void SetupPlayerInputComponent(
        UInputComponent* PlayerInputComponent) override;

    // Current player health
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    float _currentHealth = 100.0f;

    // Prevent repeated death processing
    bool bIsDead = false;

    // Stop player movement and play the death animation
    void Die();

    // Activate the stun ability when the player presses Q
    void ActivateStun();

    // Track when the stun was last used
    float LastStunTime = -1000.0f;

    // Player movement
    void MoveFB(float Value);
    void MoveLR(float Value);

    // Camera rotation
    void Rotate(float Value);
    void LookUp(float Value);

    // Third-person camera boom
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* CameraBoom;

    // Camera attached to the boom
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FollowCamera;


    // Remember the player's normal movement speed
    float OriginalWalkSpeed = 0.0f;

    // Timer for when the bread boost wears off
    FTimerHandle SpeedBoostTimerHandle;

};




