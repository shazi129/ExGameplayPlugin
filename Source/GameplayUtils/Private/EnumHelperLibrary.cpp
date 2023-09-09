#include "EnumHelperLibrary.h"

FString UEnumHelperLibrary::NetRoleToString(ENetRole NetRole)
{
	switch (NetRole)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "Max";
	}
	return "Invalid";
}

FString UEnumHelperLibrary::NetModeToString(ENetMode NetMode)
{
	switch (NetMode)
	{
	case NM_Standalone:
		return "Standalone";
	case NM_DedicatedServer:
		return "DedicatedServer";
	case NM_ListenServer:
		return "ListenServer";
	case NM_Client:
		return "Client";
	case NM_MAX:
		return "Max";
	default:
		return "Invalid";
	}
	return "Invalid";
}
