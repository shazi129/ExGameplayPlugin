#pragma once 

#include "CoreMinimal.h"
#include "ExAbilitySystemComponent.h"
#include "Tasks/AbilityTask_WaitFrames.h"
#include "AbilityTask_ActivateAndWaitAbilityEnd.generated.h"

UCLASS()
class EXGAMEPLAYABILITIES_API UAbilityTask_ActivateAndWaitAbilityEnd : public UAbilityTask_WaitFramesBase
{
	GENERATED_BODY()

public:
	/**
	 * @brief 激发一个技能，并等待它结束
	 * @param OwningAbility  
	 * @param AbilityCase  //技能配置
	 * @param SourceObject  //SourceObject
	 * @param RemoveWhenEnd  //结束后是否需要将技能移除
	 * @param WaitFrames  //因为在技能的Activate的过程中，是不能激发其他技能的，所以这有个延迟
	 * @return 
	*/
	UFUNCTION(BlueprintCallable, Category = "Ex Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_ActivateAndWaitAbilityEnd* ActivateAndWaitAbilityEnd(UGameplayAbility* OwningAbility, const FExAbilityCase& AbilityCase, UObject* SourceObject, bool RemoveWhenEnd=false, int WaitFrames=1);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	virtual bool DoTaskWork() override;

	UPROPERTY(BlueprintAssignable)
	FExAbilityDelegate ActivateFailedDelegate;

	UPROPERTY(BlueprintAssignable)
	FExAbilityDelegate AbilityEndDelegate;

protected:
	void OnAbilityFailed(const UGameplayAbility* InAbility, const FGameplayTagContainer& TagContainer);
	void OnAbilityEnd(UGameplayAbility* InAbility);

private:
	FExAbilityCase AbilityCase;
	bool RemoveWhenEnd;

	FDelegateHandle AbilityEndDelegateHandle;
	FDelegateHandle AbilityFailedDelegateHandle;
	UExAbilitySystemComponent* ASC;
};