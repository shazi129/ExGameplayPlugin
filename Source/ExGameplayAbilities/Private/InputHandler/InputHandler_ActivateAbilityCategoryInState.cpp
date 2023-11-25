//#include "InputHandler/InputHandler_ActivateAbilityCategoryInState.h"
//#include "ExGameplayAbilitiesModule.h"
//#include "PawnStateLibrary.h"
//
//void UInputHandler_ActivateAbilityCategoryInState::NativeExecute(const FInputActionValue& inputValue)
//{
//	if (HandlerData.GetScriptStruct() != FActivateAbilityCategoryData::StaticStruct())
//	{
//		EXABILITY_LOG(Error, TEXT("%s error, data format error"), *FString(__FUNCTION__), *GetNameSafe(HandlerData.GetScriptStruct()));
//		return;
//	}
//
//	UExAbilitySystemComponent* ASC = AbilityInpuHandlerHelper::GetAbilitySystem(SourceObject);
//	if (ASC == nullptr)
//	{
//		EXABILITY_LOG(Error, TEXT("%s error, cannot get ASC"), *FString(__FUNCTION__));
//		return;
//	}
//	UPawnStateComponent* PawnStateComponent = UPawnStateLibrary::GetPawnStateComponent(ASC->GetOwner());
//	if (PawnStateComponent == nullptr)
//	{
//		EXABILITY_LOG(Error, TEXT("%s error, cannot get PawnStateComponent in %s"), *FString(__FUNCTION__), *GetNameSafe(ASC->GetOwner()));
//		return;
//	}
//
//	FActivateAbilityCategoryData& Data = HandlerData.GetMutable<FActivateAbilityCategoryData>();
//
//	FGameplayTag AbilityCategory;
//	for (FPawnStateAbilityCategory& CategoryInfo : Data.CategoryList)
//	{
//		if (PawnStateComponent->HasPawnStateAsset(CategoryInfo.PawnState) && CategoryInfo.CategoryTag.IsValid())
//		{
//			AbilityInpuHandlerHelper::ActivateAbilityByCategory(ASC, CategoryInfo.CategoryTag, Data.ToServer);
//			return;
//		}
//	}
//
//	AbilityInpuHandlerHelper::ActivateAbilityByCategory(ASC, Data.DefaultAbilityCatory, Data.ToServer);
//}
