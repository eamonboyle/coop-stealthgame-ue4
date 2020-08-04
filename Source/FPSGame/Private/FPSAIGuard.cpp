// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "FPSGameMode.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"
#include "Perception/PawnSensingComponent.h"

class AFPSGameMode;
// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing Component"));

    PawnSensingComponent->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
    PawnSensingComponent->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnHearNoise);

    GuardState = EAIState::Idle;
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
    Super::BeginPlay();

    OriginalRotation = GetActorRotation();

    if (bPatrol)
    {
        MoveToNextPatrolPoint();
    }
}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
    UE_LOG(LogTemp, Warning, TEXT("SEEN A PAWN"));
    if (SeenPawn == nullptr) return;

    DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.f);

    if (AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode()))
    {
        GM->CompleteMission(SeenPawn, false);
    }

    SetGuardState(EAIState::Alerted);

    // stop movement if patrolling
    if (AController* AIC = GetController())
    {
        AIC->StopMovement();
    }
}

void AFPSAIGuard::OnHearNoise(APawn* HeardPawn, const FVector& Location, float Volume)
{
    if (GuardState == EAIState::Alerted)
    {
        return;
    }

    DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.f);

    FVector Direction = Location - GetActorLocation();
    Direction.Normalize();

    FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
    NewLookAt.Pitch = 0.f;
    NewLookAt.Roll = 0.f;

    SetActorRotation(NewLookAt);

    GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ResetOrientation);
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);

    SetGuardState(EAIState::Suspicious);
    
    // stop movement if patrolling
    if (AController* AIC = GetController())
    {
        AIC->StopMovement();
    }
}

void AFPSAIGuard::ResetOrientation()
{
    if (GuardState == EAIState::Alerted)
    {
        return;
    }
    
    SetActorRotation(OriginalRotation);

    SetGuardState(EAIState::Idle);

    if (bPatrol)
    {
        MoveToNextPatrolPoint();
    }
}


void AFPSAIGuard::OnRep_GuardState()
{
    OnStateChanged(GuardState);
}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
    if (GuardState == NewState)
    {
        return;
    }

    GuardState = NewState;
    OnRep_GuardState();
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentPatrolPoint)
    {
        FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
        float DistanceToGoal = Delta.Size();

        if (DistanceToGoal < 80)
        {
            MoveToNextPatrolPoint();
        }
    }
}

void AFPSAIGuard::MoveToNextPatrolPoint()
{
    if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == SecondPatrolPoint)
    {
        CurrentPatrolPoint = FirstPatrolPoint;
    }
    else
    {
        CurrentPatrolPoint = SecondPatrolPoint;
    }

    UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
}

void AFPSAIGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFPSAIGuard, GuardState);
}