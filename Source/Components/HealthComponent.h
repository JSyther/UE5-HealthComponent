#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

// Delegate invoked whenever health changes, providing current and max health values
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, CurrentHealth, float, MaxHealth);

// Delegate called upon death event
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

/**
 * UHealthComponent
 * 
 * Modular component responsible for managing an actor's health state.
 * 
 * - Maintains current and maximum health values.
 * - Facilitates damage intake and healing processes.
 * - Broadcasts health change and death events, enabling Blueprint integration.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE5HEALTHCOMPONENT_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHealthComponent();

    /** Current health value, editable and accessible in Blueprint */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health", meta=(ClampMin="0"))
    float Health;

    /** Maximum health cap, read-only in Blueprint */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health", meta=(ClampMin="1"))
    float MaxHealth;

    /** Indicates whether the actor is deceased */
    UPROPERTY(BlueprintReadOnly, Category="Health")
    bool bIsDead;

    /** Event triggered when health changes, providing updated health data */
    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnHealthChangedSignature OnHealthChanged;

    /** Event triggered when the actor dies */
    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnDeathSignature OnDeath;

    /** Applies damage to the health component */
    UFUNCTION(BlueprintCallable, Category="Health")
    virtual void TakeDamage(float DamageAmount);

    /** Applies healing to the health component */
    UFUNCTION(BlueprintCallable, Category="Health")
    virtual void Heal(float HealAmount);

    /** Resets health back to maximum */
    UFUNCTION(BlueprintCallable, Category="Health")
    virtual void ResetHealth();

protected:
    virtual void BeginPlay() override;

    /** Handles logic related to actor death */
    virtual void HandleDeath();

    /** Clamps health between zero and maximum health */
    void ClampHealth();

    /** Updates the health value and triggers relevant events */
    virtual void UpdateHealth(float NewHealth);
};
