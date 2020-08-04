// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSExtractionZone.h"


#include "FPSCharacter.h"
#include "FPSGameMode.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSExtractionZone::AFPSExtractionZone()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    OverlapComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComponent"));
    OverlapComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    OverlapComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    OverlapComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    OverlapComponent->SetBoxExtent(FVector(200.f));
    RootComponent = OverlapComponent;

    OverlapComponent->SetHiddenInGame(false);

    OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &AFPSExtractionZone::HandleOverlap);

    DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
    DecalComponent->DecalSize = FVector(200.f);
    DecalComponent->SetupAttachment(RootComponent);
}

void AFPSExtractionZone::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (AFPSCharacter* Player = Cast<AFPSCharacter>(OtherActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Overlapped with extraction zone"));
        
        if (Player->bIsCarryingObjective)
        {
            if (AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode()))
            {
                GM->CompleteMission(Player, true);
            }
        }
        else
        {
            UGameplayStatics::PlaySound2D(this, ObjectiveMissingSound);
        }
    }
}