// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIGuard.generated.h"

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EAIState : uint8
{
    Idle,
    Suspicious,
    Alerted
};

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AFPSAIGuard();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UPawnSensingComponent* PawnSensingComponent;

    UFUNCTION()
    void OnPawnSeen(APawn* SeenPawn);

    UFUNCTION()
    void OnHearNoise(APawn* HeardPawn, const FVector& Location, float Volume);

    FRotator OriginalRotation;

    UFUNCTION()
    void ResetOrientation();

    FTimerHandle TimerHandle_ResetOrientation;

    UPROPERTY(ReplicatedUsing=OnRep_GuardState)
    EAIState GuardState;

    UFUNCTION()
    void OnRep_GuardState();

    void SetGuardState(EAIState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "AI")
    void OnStateChanged(EAIState NewState);

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    /** let the guard go on patrol */
    UPROPERTY(EditInstanceOnly, Category = "AI")
    bool bPatrol;

    /** patrol points */
    UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition="bPatrol"))
    AActor* FirstPatrolPoint;
    
    UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition="bPatrol"))
    AActor* SecondPatrolPoint;

    /** the current point the guard is moving to or standing at */
    UPROPERTY()
    AActor* CurrentPatrolPoint;

    void MoveToNextPatrolPoint();
};
