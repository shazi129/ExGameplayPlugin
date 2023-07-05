#pragma once

#include "ExTypes.h"
#include "GameplayTagsManager.h"
#include "GameFramework/Character.h"
#include "ExGameplayLibrary.generated.h"

UCLASS()
class EXGAMEPLAYLIBRARY_API UExGameplayLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary", meta = (DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static bool IsClient(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary")
		static int64 GetTimestamp(); //统一用的UTC时间

	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary")
		static int GetTimeZone(); //当地时区

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

	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary", meta = (DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static bool IsGameplayTagValid(FGameplayTag GameplayTag);

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

	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary")
	static bool PawnInputEanbled(APawn* Pawn);

	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary")
	static FString GetComputerName();

	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary", meta = (DefaultToSelf = "WorldContextObject"))
	static bool IsRunning(UObject* WorldContextObject);
	
	/**
	 * @brief 获取Object的完整包名，例如：/Game/FPSGame/GameBase/BP_Character,等同于Object->GetPackage()->GetName()
	 * 但会过滤Editor环境下的PIE前缀，例如在PIE模式下，一个Level的包名为：/Game/FPSGame/Maps/UEDPIE_0_Login
	 * 通过GetPackageFullName获取的是：/Game/FPSGame/Maps/Login
	 * 
	 * @param Object 目标Object
	 * @return 完整包名
	*/
	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary")
	static FString GetPackageFullName(UObject* Object);


	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary")
	static FString ObjectPathToPackageName(const FString& ObjectPath);

	//在某个socket上spawn一个Actor
	UFUNCTION(BlueprintCallable, Category = "ExGameplayLibrary", meta = (DefaultToSelf = "WorldContextObject"))
	static AActor* SpawnActorInSocket(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, UPrimitiveComponent* Parent, FName Socket);

	//单独对某个Actor做LineTrace
	UFUNCTION(BlueprintCallable, Category = "ExGameplayLibrary")
	static bool ActorLineTraceSingle(AActor* Actor, FHitResult& OutHit, const FVector& Start, const FVector& End, ECollisionChannel TraceChannel);

	//移动Character
	UFUNCTION(BlueprintCallable, Category = "ExGameplayLibrary")
	static void CharacterSmoothMoveTo(ACharacter* Character, FVector TargetLocation, float DeltaSecond);

	//一个Object是否合法
	UFUNCTION(BlueprintPure, Category = "ExGameplayLibrary")
	static bool IsValidObject(UObject* Object);

	/**
	 * @brief 在参数列表中获取指定的int值，例如对于参数Param：-MaxFps=100 -AutoTest
	 * 可以使用ParseParamIntValue(Param, "-MaxFps=", OutValue)将MaxFps的值100写入参数OutValue中
	 *
	 * @param InString 参数字符串
	 * @param InParam  指定的参数项
	 * @param OutValue  获取到的参数项的int值
	 * @return 是否获取成功，当参数字符串中没有需要的参数项时，返回false
	*/
	UFUNCTION(BlueprintCallable, Category = "ExGameplayLibrary")
		static bool ParseParamIntValue(const FString& InString, const FString& InParam, int& OutValue);
};
