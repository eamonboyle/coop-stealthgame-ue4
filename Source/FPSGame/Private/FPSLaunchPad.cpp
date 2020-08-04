// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSLaunchPad.h"


#include "FPSCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSLaunchPad::AFPSLaunchPad()
{
    OverlapComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComponent"));
    OverlapComponent->SetBoxExtent(FVector(75, 75, 50));
    RootComponent = OverlapComponent;
    
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(OverlapComponent);

    OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &AFPSLaunchPad::OverlapLaunchPad);

    LaunchStrength = 1500;
    LaunchPitchAngle = 35.0f;
}

void AFPSLaunchPad::OverlapLaunchPad(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("OVERLAP LAUNCHPAD"));
    FRotator LaunchDirection = GetActorRotation();
    LaunchDirection.Pitch += LaunchPitchAngle;
    FVector LaunchVelocity = LaunchDirection.Vector() * LaunchStrength;

    if (ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor))
    {
        OtherCharacter->LaunchCharacter(LaunchVelocity, true, true);
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActivateLaunchPadEffect, GetActorLocation());
    }
    else if (OtherComp && OtherComp->IsSimulatingPhysics())
    {
        OtherComp->AddImpulse(LaunchVelocity, NAME_None, true);
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActivateLaunchPadEffect, GetActorLocation());
    }
}
