#include "Event/Handlers/GameplayEventHandler_ComponentBool.h"
#include "FunctionLibraries/GameplayUtilsLibrary.h"
#include "GameplayUtilsModule.h"

FGameplayEventContext UGameplayEventHandler_ComponentBool::MakeComponentBoolContext(AActor* Actor, const FString& ComponentName, bool ComponentValue)
{
    FGameplayEventContext Context;
    Context.Instigator = Actor;

    Context.Data.InitializeAs<FNameBoolData>();
    FNameBoolData& ComponentBool = Context.Data.GetMutable<FNameBoolData>();
    ComponentBool.Name = ComponentName;
    ComponentBool.Value = ComponentValue;

    return MoveTemp(Context);
}

void UGameplayEventHandler_ComponentBool::NativeExecute(const FGameplayEventContext& Context)
{
    AActor* InstigatorActor = Cast<AActor>(Context.Instigator);
    if (!InstigatorActor || Context.Data.GetScriptStruct() != FNameBoolData::StaticStruct())
    {
        return;
    }

    FNameBoolData& NameBoolData = Context.Data.GetMutable<FNameBoolData>();
    UActorComponent* Component = UGameplayUtilsLibrary::GetComponentByName(InstigatorActor, NameBoolData.Name);
    if (Component)
    {
        HandleComponent(Component, NameBoolData.Value);
    }
    else
    {
        GAMEPLAYUTILS_LOG(Error, TEXT("%s error, cannot get component by name %s in %s"), *FString(__FUNCTION__), *NameBoolData.Name, *GetNameSafe(InstigatorActor));
    }
}

void UGameplayEventHandler_ComponentBool::HandleComponent_Implementation(UActorComponent* Component, bool Value)
{
}
