#pragma once


typedef void( __fastcall* t_ProcessEvent ) ( UObject*, UFunction*, void*, void* );
//typedef __int64( __fastcall* t_ProcessEvent ) ( void*, __int64, __int64*, __int64 );
t_ProcessEvent o_processevent;
t_ProcessEvent o_processevent_addr;

extern void __fastcall hk_processevent( UObject*, UFunction* pFunction, void* pParams, void* pResult );
//extern __int64 __fastcall  __fastcall hk_processevent( void* a1, __int64 a2, __int64* a3, __int64 a4 );

bool hook_processevent( void ) {

	o_processevent_addr = utils::get_vfunc<t_ProcessEvent>( g_engine, 67 );

	if ( MH_CreateHook( o_processevent_addr, (void*)hk_processevent, (void**)&o_processevent ) != MH_OK ) {
		return false;
	}

	return MH_EnableHook( o_processevent_addr ) == MH_OK;
}

bool unhook_processevent( void ) {

	if ( MH_DisableHook( o_processevent_addr ) != MH_OK ) {
		return false;
	}

	return MH_RemoveHook( o_processevent_addr ) == MH_OK;
}