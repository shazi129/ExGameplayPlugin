#pragma once 

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DataVersionDescriptor.h"
#include "DataCenterSubsystem.generated.h"


UCLASS(BlueprintType)
class EXGAMEPLAYPLUGIN_API UDataCenterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static UDataCenterSubsystem* GetSubsystem(const UObject* WorldContextObject);

	//默认打进包里的数据路径
	UFUNCTION(BlueprintPure)
	static FString GetDefaultDataPath();

	//更新数据时，数据缓存的路径
	UFUNCTION(BlueprintPure)
	static FString GetCacheDataPath(int ZoneID, const FString& Environment);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	FString GetDataPath();

	UFUNCTION(BlueprintCallable)
	void RebuildDescriptor(int ZoneID, const FString& Environment);

	UFUNCTION(BlueprintCallable)
	void AddOnLineDesriptorWithJson(const FString& JsonContent);

	UFUNCTION(BlueprintCallable)
	void AddOnLineDescriptor(const FDataVersionDescriptor& Descriptor);

private:
	FDataVersionDescriptor CurrentDescriptor; //当前使用的数据版本

	TArray<FDataVersionDescriptor> OnlineDescriptors; //线上的配置
};