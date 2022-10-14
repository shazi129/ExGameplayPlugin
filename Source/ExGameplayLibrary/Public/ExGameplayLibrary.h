#pragma once

#include "ExTypes.h"
#include "GameplayTagsManager.h"
#include "ExGameplayLibrary.generated.h"

UCLASS()
class EXGAMEPLAYLIBRARY_API UExGameplayLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary", meta = (DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static bool IsClient(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary")
		static int64 GetTimestamp();

	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary", meta = (DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static FString GetLogPrefix(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "ExGameplayLibrary", meta = (DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static void Log(const UObject* WorldContextObject, FString Message, EBPLogLevel Level);

	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary")
		static EBPNetMode GetWorldNetMode(UWorld* World);

	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary")
		static bool IsEditorMode();

	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary")
		static FString FormatLeftTime(int64 Seconds, int Segments = 3, FString FromatString = "");

	UFUNCTION(BlueprintCallable, Category = "ExGameplayLibrary")
		static bool ExecCommand(const FString& Command);

	UFUNCTION(BlueprintCallable, Category = "ExGameplayLibrary", meta = (DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static void SetLumenEnable(const UObject* WorldContextObject, bool Enable);

	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary", meta = (DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static FGameplayTag RequestGameplayTag(FName TagName, bool ErrorIfNotFound = true);

	UFUNCTION(BlueprintCallable, Category = "ExGameplayLibrary", meta = (DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static void SetDynamicGlobalIlluminationLevel(const UObject* WorldContextObject, int Level);

	UFUNCTION(BlueprintCallable, Category = "ExGameplayLibrary", meta = (DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static void SetPostProcessingGI(const UObject* WorldContextObject, int Level);

	UFUNCTION(BlueprintCallable, Category = "ExGameplayLibrary", meta = (DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static void SetPostProcessingReflection(const UObject* WorldContextObject, int Level);

	UFUNCTION(BlueprintCallable, Category = "ExGameplayLibrary", meta = (DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static void SetPostProcessingSSAO(const UObject* WorldContextObject, int Level);

	UFUNCTION(BlueprintCallable, Category = "ExGameplayLibrary")
	static bool IsActorHidden(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "ExGameplayLibrary")
	static bool IsA(UObject* Object, UClass* Class);

	UFUNCTION(BlueprintCallable, Category = "ExGameplayLibrary")
	static UObject* StaticLoadObject(UClass* Class, UObject* InOuter, const FString& Name);
};
