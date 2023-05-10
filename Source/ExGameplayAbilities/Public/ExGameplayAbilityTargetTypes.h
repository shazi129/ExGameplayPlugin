#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "InstancedStruct.h"
#include "ExGameplayAbilityTargetTypes.generated.h"

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FGameplayAbilityTargetData_Int64Array : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<int64> Int64Array;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_Int64Array::StaticStruct();
	}

	virtual FString ToString() const override {
		return TEXT("FGameplayAbilityTargetData_Int64Array");
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		SafeNetSerializeTArray_Default<64>(Ar, Int64Array);
		bOutSuccess = true;
		return true;
	}
};


template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Int64Array> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Int64Array>
{
	enum {
		WithNetSerializer = true
	};
};


USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FGameplayAbilityTargetData_FVectorArray : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FVector> FVectorArray;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_Int64Array::StaticStruct();
	}

	virtual FString ToString() const override {
		return TEXT("FGameplayAbilityTargetData_FVectorArray");
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		SafeNetSerializeTArray_Default<32>(Ar, FVectorArray);
		bOutSuccess = true;
		return true;
	}
};


template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_FVectorArray> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_FVectorArray>
{
	enum {
		WithNetSerializer = true
	};
};

USTRUCT(BlueprintType)
struct EXGAMEPLAYABILITIES_API FGameplayAbilityTargetData_FInstancedStruct : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FInstancedStruct InstancedStruct;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_FInstancedStruct::StaticStruct();
	}

	virtual FString ToString() const override {
		return TEXT("FGameplayAbilityTargetData_FInstancedStruct");
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		InstancedStruct.Serialize(Ar);
		bOutSuccess = true;
		return true;
	}
};


template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_FInstancedStruct> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_FInstancedStruct>
{
	enum {
		WithNetSerializer = true
	};
};