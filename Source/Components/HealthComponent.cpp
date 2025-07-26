#include "HealthComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // Initialize default values
    MaxHealth = 100.0f;
    Health = MaxHealth;
    bIsDead = false;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    // Ensure health is clamped correctly at start
    ClampHealth();
    UpdateHealth(Health);
}

void UHealthComponent::TakeDamage(float DamageAmount)
{
    if (bIsDead || DamageAmount <= 0.f)
    {
        return;
    }

    float NewHealth = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);

    UpdateHealth(NewHealth);

    if (Health <= 0.f && !bIsDead)
    {
        bIsDead = true;
        HandleDeath();
    }
}


void UHealthComponent::Heal(float HealAmount)
{
    if (bIsDead || HealAmount <= 0.f)
    {
        return;
    }

    float NewHealth = FMath::Clamp(Health + HealAmount, 0.f, MaxHealth);
    UpdateHealth(NewHealth);
}

void UHealthComponent::ResetHealth()
{
    bIsDead = false;
    UpdateHealth(MaxHealth);
}

void UHealthComponent::ClampHealth()
{
    Health = FMath::Clamp(Health, 0.f, MaxHealth);
}

void UHealthComponent::UpdateHealth(float NewHealth)
{
    // Clamp health within valid range
    NewHealth = FMath::Clamp(NewHealth, 0.f, MaxHealth);

    if (FMath::IsNearlyEqual(Health, NewHealth))
    {
        // No change in health, skip
        return;
    }

    Health = NewHealth;

    // Broadcast health change event with current and max health values
    OnHealthChanged.Broadcast(Health, MaxHealth);
}

void UHealthComponent::HandleDeath()
{
    // Broadcast the death event to any bound listeners
    OnDeath.Broadcast();

    // Attempt to enable ragdoll physics on the owning actor
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("HealthComponent: Owner is invalid in HandleDeath"));
        return;
    }

    // Optional toggle to activate ragdoll physics upon death (set to false to disable)
    bool bRagDoll = true;

    if(bRagDoll)
    {
        // Try to get the SkeletalMeshComponent to simulate physics (ragdoll) 
        USkeletalMeshComponent* SkeletalMeshComp = OwnerActor->FindComponentByClass<USkeletalMeshComponent>();
        if (!SkeletalMeshComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("HealthComponent: SkeletalMeshComponent not found on owner"));
            return;
        }
    
        // Enable physics simulation on skeletal mesh to trigger ragdoll effect
        SkeletalMeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
        SkeletalMeshComp->SetAllBodiesSimulatePhysics(true);
        SkeletalMeshComp->SetSimulatePhysics(true);
        SkeletalMeshComp->WakeAllRigidBodies();
        SkeletalMeshComp->bBlendPhysics = true;
    
        // Disable character movement and collision (if owner is a Character)
        if (ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor))
        {
            OwnerCharacter->GetCharacterMovement()->DisableMovement();
            OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    
        UE_LOG(LogTemp, Log, TEXT("HealthComponent: Ragdoll activated on death"));
    }
}
