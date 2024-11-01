#pragma once

#include "iostream"

ULocalPlayer* g_local     = NULL;
UWorld*       g_world     = NULL;
UGameEngine*  g_engine    = NULL;
ABall_TA*     g_ball      = NULL;
AWorldInfo*   g_worldinfo = NULL;

#include "memory.hpp"
#include "console.hpp"
#include "input.hpp"
#include "render.hpp"

#define print_success(s)	 std::cout << "[+] " << s << std::endl;
#define print_information(s) std::cout << "[*] " << s << std::endl;
#define print_error(s)		 std::cout << "[!] " << s << std::endl;

#define print_vector(v)  printf("Location { X=%.3f Y=%.3f Z=%.3f }\n", v.X, v.Y, v.Z);
#define print_rotator(v) printf("Location { X=%d Y=%d Z=%d }\n", v.Pitch, v.Yaw, v.Roll);

namespace utils {

	bool initialize_sdk( void ) {

		auto gnames = memory::pattern_scan( "48 89 05 ? ? ? ? 89 5C 24 60", 0 );
		if ( !gnames )
			return false;

		//gnames += *(int32_t*)gnames + 4 + 47;
		gnames += *(int32_t*)( gnames + 3 ) + 7;
		if ( !gnames )
			return false;

		auto gobjects = memory::pattern_scan( "48 8B 05 ? ? ? ? 48 63 CB 48 89 3C C8", 0 );
		if ( !gobjects )
			return false;

		//gobjects += *(int32_t*)gobjects + 4 + 101;
		gobjects += *(int32_t*)( gobjects + 3 ) + 7;
		if ( !gobjects )
			return false;

		GObjects = reinterpret_cast<TArray<class UObject*>*>( gobjects );
		GNames = reinterpret_cast<TArray<struct FNameEntry*>*>( gnames );

		return true;
	}

	template<typename T>
	T get_vfunc( void* ptr, uint16_t index ) {
		return ( *reinterpret_cast<T**>( (void*)ptr ) )[index];
	}

	UGameEngine* engine( void ) {
		return UObject::FindObject<UGameEngine>( "GameEngine_TA Transient.GameEngine_TA" );
	}

	ULocalPlayer* local( void ) {
		return g_engine->GamePlayers[0];
	}

	ABall_TA* ball( void ) {

		for ( APawn* p = g_local->Actor->WorldInfo->PawnList; p; p = p->NextPawn ) {

			if ( !p || p->bDeleteMe ) break;

			if ( p->IsA( ABall_TA::StaticClass() ) ) {
				return reinterpret_cast<ABall_TA*>( p );
			}
		}

		return NULL;
	}

	UMapList_TA* maplist( void ) {
		return UObject::FindObject<UMapList_TA>( "MapList_TA MapList.MapList" );
	}

	UWorld* world( void ) {

		auto pMapList = maplist();
		auto pWorld = (UWorld*)NULL;

		for ( auto& pMap : pMapList->AllMaps ) {
			auto fullname = pMap->GetName().append( ".TheWorld" ).insert( 0, "World " );
			if ( pWorld = UObject::FindObject<UWorld>( fullname ) ) {
				return pWorld;
			}
		}
	}

	inline bool init_frame( void ) {

		if ( !g_local || !g_local->Actor || !g_local->Actor->Pawn || g_local->Actor->Pawn->bDeleteMe ) {
			if ( g_ball ) {
				g_ball = NULL;
				print_information( "player dead" );
			}
			return false;
		}

		if ( !g_ball ) {
			if ( g_ball = utils::ball() ) {
				print_information( "player alive" );
				return true;
			}
			else {
			}
		}

		return true;
	}

	APawn* closest_target( AActor* local, APawn* pawnlist ) {

		float  distance_min = 99999;
		APawn* closest = NULL;

		for ( APawn* p = pawnlist; p; p = p->NextPawn ) {

			if ( !p || p == local || p == g_ball || p->bDeleteMe )
				continue;

			if ( !local ) break;

			auto distance = local->Location.distance( p->Location );

			if ( distance < distance_min ) {
				closest = p;
				distance_min = distance;
			}
		}

		return closest;
	}

	APawn* closest_target_aligned( APawn* local, APawn* pawnlist ) {

		float  angle_min = 99999;
		APawn* closest = NULL;

		//uint8_t team = local->GetTeamNum();

		for ( APawn* p = pawnlist; p; p = p->NextPawn ) {

			if ( !p || p == g_ball || p->bDeleteMe )
				break;

			if (!local) break;

			if (p == local) continue;

			//uint8_t en = p->GetTeamNum();

			//if ( en == team ) {
			//	continue;
			//}

			auto angle = abs( local->Velocity.angle( p->Location - local->Location ) );

			if ( angle < angle_min ) {
				closest = p;
				angle_min = angle;
			}
		}

		return closest;
	}
}