
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MazeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class MYMAZEPROJECT_API AMazeCharacter : public ACharacter
{
    GENERATED_BODY()


public:
    AMazeCharacter();

protected:
    virtual void BeginPlay() override;

    virtual void SetupPlayerInputComponent(
        UInputComponent* PlayerInputComponent) override;

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
};


