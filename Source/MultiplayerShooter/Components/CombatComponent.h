// Hakan Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	friend class APlayerCharacter;

	void EquipWeapon(class AWeapon* WeaponToEquip);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	class APlayerCharacter* Character;
	class AWeapon* EquippedWeapon;

public:	
	
		
};
