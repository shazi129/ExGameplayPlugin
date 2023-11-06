#pragma once

#include "CoreMinimal.h"
#include "GameplayTypes.generated.h"

//在某个端执行行为
UENUM(BlueprintType)
enum class EExecNetMode : uint8
{
	//在客户端触发, 包括Client， ListenServer, Standalone
	E_Client			UMETA(DisplayName = "Client"),

	//在服务端触发， 包括DedicatedServer， ListenServer, Standalone
	E_Server			UMETA(DisplayName = "Server"),

	//严格意义上的客户端，只有Client
	E_ExactClient		UMETA(DisplayName = "Exact Client"),

	//严格意义上的服务端，只有DedicatedServer， ListenServer
	E_ExactServer		UMETA(DisplayName = "Exact Server"),

	//不限制
	E_Aways				UMETA(DisplayName = "Aways"),
};

USTRUCT(BlueprintType)
struct GAMEPLAYUTILS_API FFilterActorCondition
{
	GENERATED_BODY()

	//Actor中必须有这些Component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<UActorComponent>> RequireComponentClasses;

	//Actor中不能有这些Component
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<UActorComponent>> ExcludeComponentClasses;

	//Actor必须属于在指定的Class
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<AActor>> ActorClasses;

	//忽略的Actor
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AActor*> IgnoreActors;

	bool FilterActorClasses(AActor* Actor) const;
	bool FilterExcludeComponentClasses(AActor* Actor) const;
	bool FilterRequireComponentClasses(AActor* Actor) const;
	bool FilterIgnoreActors(AActor* Actor) const;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStringParamDelegate, const FString&, StringParam);