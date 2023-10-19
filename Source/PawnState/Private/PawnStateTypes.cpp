#include "PawnStateTypes.h"
#include "NativeGameplayTags.h"

//跑动的tag
UE_DEFINE_GAMEPLAY_TAG(TAG_PawnState_Run, "PawnState.Run");

//获取服务端的pawnState
UE_DEFINE_GAMEPLAY_TAG(TAG_GetServerStates, "PawnState.Cheat.GetServerStates");
UE_DEFINE_GAMEPLAY_TAG(TAG_RetServerStates, "PawnState.Cheat.RetServerStates");

//获取服务端的Tag
UE_DEFINE_GAMEPLAY_TAG(TAG_GetServerTags, "PawnState.Cheat.GetServerTags");
UE_DEFINE_GAMEPLAY_TAG(TAG_RetServerTags, "PawnState.Cheat.RetServerTags");