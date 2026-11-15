
#include "MazeAIController.h"

#include "MazeCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"

AMazeAIController::AMazeAIController()
{
    // Create the AI Perception component
    EnemyPerception =
        CreateDefaultSubobject<UAIPerceptionComponent>(
            TEXT("EnemyPerception"));

    SetPerceptionComponent(*EnemyPerception);

    // Configure sight detection
    SightConfig =
        CreateDefaultSubobject<UAISenseConfig_Sight>(
            TEXT("SightConfig"));

    SightConfig->SightRadius = 1200.0f;
    SightConfig->LoseSightRadius = 1500.0f;
    SightConfig->PeripheralVisionAngleDegrees = 70.0f;

    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

    EnemyPerception->ConfigureSense(*SightConfig);

    EnemyPerception->SetDominantSense(
        UAISense_Sight::StaticClass());

    // Bind the sight detection event
    EnemyPerception->OnTargetPerceptionUpdated.AddDynamic(
        this,
        &AMazeAIController::OnTargetPerceptionUpdated);
}

void AMazeAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }
}

void AMazeAIController::OnTargetPerceptionUpdated(
    AActor* Actor,
    FAIStimulus Stimulus)
{
    // Only respond to our player character
    if (!Cast<AMazeCharacter>(Actor))
    {
        return;
    }

    UBlackboardComponent* BlackboardComp =
        GetBlackboardComponent();

    if (!BlackboardComp)
    {
        return;
    }

    if (Stimulus.WasSuccessfullySensed())
    {
        // Remember the player as our target
        BlackboardComp->SetValueAsObject(
            TEXT("TargetActor"), Actor);
    }
    else
    {
        // Forget the player when sight is lost
        if (BlackboardComp->GetValueAsObject(
            TEXT("TargetActor")) == Actor)
        {
            BlackboardComp->ClearValue(
                TEXT("TargetActor"));
        }
    }
}

