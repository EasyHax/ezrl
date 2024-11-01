#pragma once

#include "windows.h"

template<typename T>
class hook_engine {

public:

	T o_fn;

	struct hook_t {
		uint64_t hk_addr;
		uint8_t tr_addr[];
	};

	hook_engine( T target, T detour, T& original, uint8_t len = 16 ) {

		if ( !alloc( target, len ) ) {
			return;
		}

		byte jmp[] = {
			0x50,                               // push   rax
			0x48, 0xB8, 0, 0, 0, 0, 0, 0, 0, 0, // movabs rax, des
			0x48, 0x87, 0x04, 0x24,	            // xchg   QWORD PTR [rsp],rax
			0xC3                                // ret
		};

		*(uint16_t*)&far_jmp = 0x25FF;
		*(uint64_t*)( &jmp[3] ) = (uint64_t)target + len;
		memcpy( hook_data->tr_addr, target, len );
		memcpy( hook_data->tr_addr + len, jmp, sizeof( jmp ) );

		hook_data->hk_addr = (uint64_t)detour;
		*(uint32_t*)( &far_jmp[2] ) = (uint32_t)( (uint64_t)&hook_data->hk_addr - (uint64_t)target - sizeof( far_jmp ) );
		og_addr = (void*)target;

		o_fn = reinterpret_cast<T>( &hook_data->tr_addr );
		original = o_fn;
	}

	void hook() {
		VirtualProtect( og_addr, sizeof( far_jmp ), PAGE_EXECUTE_READWRITE, &_ );
		memcpy( og_addr, far_jmp, sizeof( far_jmp ) );
		VirtualProtect( og_addr, sizeof( far_jmp ), _, &_ );
	}

	void unhook() {
		VirtualProtect( og_addr, sizeof( far_jmp ), PAGE_EXECUTE_READWRITE, &_ );
		memcpy( og_addr, hook_data->tr_addr, sizeof( far_jmp ) );
		VirtualProtect( og_addr, sizeof( far_jmp ), _, &_ );
	}

	//private:

	bool alloc( void* src, uint8_t len ) {

		MEMORY_BASIC_INFORMATION mbi;

		for ( uint64_t addr = (uint64_t)src; addr > (uint64_t)src - 0x80000000; addr = (uint64_t)mbi.BaseAddress - 1 ) {
			if ( !VirtualQuery( (LPCVOID)addr, &mbi, sizeof( mbi ) ) ) {
				break;
			}

			if ( mbi.State == MEM_FREE ) {
				if ( hook_data = (hook_t*)VirtualAlloc( mbi.BaseAddress, len, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE ) ) {
					return true;
				}
			}
		}
		return false;
	}

	void* og_addr;

	hook_t* hook_data;

	uint8_t far_jmp[6];

	DWORD _;
};