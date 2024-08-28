/*
	대기 파트너 위치 
	대기 중인 파트너가 모두 컨트롤러가 필요한가? - 데리고 갈 녀석만 가능하면 되지 않을까?


*/
#pragma once

#include "CoreMinimal.h"
#include "Npc.h"
#include "NpcPartnerManager.generated.h"

#define PARTNER_GAP 200.f
/**
 * 
 */
UCLASS()
class POKEHUNTER_API ANpcPartnerManager : public ANpc
{
	GENERATED_BODY()

public:
	ANpcPartnerManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Interact_Implementation(AHunter* Hunter) override;

	UFUNCTION(BlueprintCallable)
	void SelectPartner();
	UFUNCTION(BlueprintCallable)
	void AddIndex(int Val);
	UFUNCTION(BlueprintCallable)
	void ShowPartner(int Val);
	UFUNCTION(BlueprintCallable)
	void ChangePartner();
	UFUNCTION(BlueprintCallable)
	void LeavePartner();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Partner")
	TArray<TSubclassOf <class APartner>> PartnerClassArray;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Partner")
	TArray<class APartner*> PartnerArray;
	TArray<FVector>PositionArray;
	FVector PosePos;
	int CurrentIndex = 1;
};
