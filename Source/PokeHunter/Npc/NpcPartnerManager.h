/*
	��� ��Ʈ�� ��ġ 
	��� ���� ��Ʈ�ʰ� ��� ��Ʈ�ѷ��� �ʿ��Ѱ�? - ������ �� �༮�� �����ϸ� ���� ������?


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
	virtual void interact_Implementation(AHunter* Hunter) override;

	UFUNCTION(BlueprintCallable)
	void SelectPartner();
	UFUNCTION(BlueprintCallable)
	void AddIndex(int Val);
	UFUNCTION(BlueprintCallable)
	void ChangePartner();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Partner")
	TArray<TSubclassOf <class APartner>> PartnerClassArray;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Partner")
	TArray<class APartner*> PartnerArray;
	TArray<FVector>PositionArray;
	FVector PosePos;
	int CurrentIndex = 0;
};
