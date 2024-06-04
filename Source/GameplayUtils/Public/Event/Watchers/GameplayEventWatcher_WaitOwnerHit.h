#pragma

/**
 * 等待所有者的受击事件
 */
#include "Event/GameplayEventWatcher.h"
#include "GameplayEventWatcher_WaitOwnerHit.generated.h"

USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FHitResultContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	UPrimitiveComponent* HitComp;

	UPROPERTY(BlueprintReadOnly)
	AActor* OtherActor;

	UPROPERTY(BlueprintReadOnly)
	UPrimitiveComponent* OtherComp;

	UPROPERTY(BlueprintReadOnly)
	FVector NormalImpulse;

	UPROPERTY(BlueprintReadOnly)
	FName HitBoneName;

	static FInstancedStruct MakeContext(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};

UCLASS(BlueprintType, Blueprintable)
class GAMEPLAYUTILS_API UGameplayEventWatcher_WaitOwnerHit : public UGameplayEventWatcher
{
	GENERATED_BODY()

public:
	virtual void Activate() override;
	virtual void Deactivate() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* InHitComp, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, FVector InNormalImpulse, const FHitResult& InHit);

	UFUNCTION(BlueprintCallable)
	FHitResultContext GetContext(const FInstancedStruct& ContextData);

public:
	UPROPERTY(EditAnywhere)
	TArray<FString> ComponentNames;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AActor>> ActorClasses;

private:
	TArray<TWeakObjectPtr<UPrimitiveComponent>> WaitComponents;
};