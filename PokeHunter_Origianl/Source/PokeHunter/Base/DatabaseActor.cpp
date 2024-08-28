// Fill out your copyright notice in the Description page of Project Settings.


#include "DatabaseActor.h"

// Sets default values
ADatabaseActor::ADatabaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADatabaseActor::BeginPlay()
{
	Super::BeginPlay();

	// -> InventoryServerManager.cpp에서 실행
	for (auto& ItemDataClass : ItemDataClassMap)
	{
		auto ItemData = NewObject<UItemData>(this, ItemDataClass.Value);
		ItemDataObjectMap.Add(ItemDataClass.Key, ItemData);
	}

	for (auto& SkillInfo : SkillInfoArray)
	{
		auto SkillData = NewObject<USkillData>(this);
		SkillData->SkillInfo = SkillInfo;
		SkillDataObjectMap.Add(SkillInfo.ID, SkillData);
	}

	// BP_DatabaseActor의 상세페이지에서 직접 넣었음.
	//PartnerDataObjectMap.Add(EPartnerType::WolfPartner, APartner::StaticClass());
	//PartnerDataObjectMap.Add(EPartnerType::GolemPartner, APartner::StaticClass());
}

// Called every frame
void ADatabaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UItemData* ADatabaseActor::FindItem(FName ItemID)
{
	if (ItemID == "None")							return nullptr;
	if (ItemDataClassMap.IsEmpty())					return nullptr;
	if (ItemDataObjectMap.IsEmpty())				return nullptr;
	if (0 == ItemDataClassMap.Num())				return nullptr;
	if (0 == ItemDataObjectMap.Num())				return nullptr;
	if (nullptr == ItemDataClassMap.Find(ItemID))	return nullptr;
	if (nullptr == ItemDataObjectMap.Find(ItemID))	return nullptr;

	UItemData* ItemData = *ItemDataObjectMap.Find(ItemID);

	if (ItemData != nullptr)						return ItemData;
	else											return nullptr;
}

FSkillInfo ADatabaseActor::FindSkill(ESkillID SkillID)
{
	for (auto& Skill : SkillInfoArray)
	{
		if (SkillID == Skill.ID)
		{
			return Skill;
		}
	
	}

	FSkillInfo info;
	return info;

}

TSubclassOf<APartner> ADatabaseActor::FindPartner(EPartnerType input_type)
{
	if (PartnerDataObjectMap.IsEmpty())						return nullptr;
	if (0 == PartnerDataObjectMap.Num())					return nullptr;

	if (nullptr == PartnerDataObjectMap.Find(input_type)) {
		input_type = EPartnerType::WolfPartner;
		UE_LOG(LogTemp, Warning, TEXT("FindPartner() : input_type is nullptr"));
	}

	auto get_partner = *PartnerDataObjectMap.Find(input_type);

	if (get_partner != nullptr)								return get_partner;
	else													return nullptr;
}

