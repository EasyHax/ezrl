#include "windows.h"
#include "iostream"
#include "string"
#include "vector"

#include "SDK/SdkHeaders.hpp"

#define _M_X64
#include "MinHook.h"
#include "utils/utils.hpp"

#include "hook/present.hpp"
#include "hook/process_event.hpp"

#include "features/autodemo.hpp"
#include "features/prediction.hpp"
#include "features/chat.hpp"

/* #####################################################################################################  */

char buffer[50];
FVector g_location;
std::vector<AVehiclePickup_Boost_TA*> boosts;

HRESULT __fastcall hk_present( IDXGISwapChain* g_pSwapChain, UINT syncInterval, UINT flags ) {

	if ( !g_imgui_initialized ) {
		if ( !imgui_initialize(g_pSwapChain) ) {
			return o_present( g_pSwapChain, syncInterval, flags );
		}

		print_success( "imgui initialized\n" );
	}

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::CaptureMouseFromApp( false );
	ImGui::CaptureKeyboardFromApp( false );

	ImGui::NewFrame();

	if ( g_drawing && autodemo::target && g_local && g_local->Actor && g_local->Actor->Pawn ) {

		auto p0 = world_to_screen( autodemo::target->Location );
		auto p1 = world_to_screen( g_local->Actor->Pawn->Location );
		
		ImGui::GetForegroundDrawList()->AddLine( { p0.X, p0.Y }, { p1.X, p1.Y }, ImColor( 255, 0, 255 ), 0.5f );
	}

	//if ( g_show_menu ) {
	//	ImGui::ShowDemoWindow();
	//}

	//if ( g_drawing && g_ball && g_local && g_local->Actor && g_local->Actor->Pawn ) {

	//	ImDrawList* draw_list = ImGui::GetForegroundDrawList();


	//	auto it = boosts.begin();

	//	for ( uint8_t i = 0; i < boosts.size(); ++i ) {
	//		auto& boost = boosts.at( i );

	//		auto loc = world_to_screen( boost->Location );
	//		
	//		
	//		if ( boost && &boost->Timers && boost->Timers.Num() == 1 && boost->Timers[0].Count > 0 ) {
	//			auto str = "time: " + std::to_string( boost->RespawnDelay - boost->Timers[0].Count );
	//			draw_list->AddText( ImVec2( loc.X, loc.Y ), ImColor( 255, 0, 0 ), str.c_str() );
	//			printf( "timer: %lf\n", boost->Timers[0].Count );
	//		}
	//		else {
	//			boosts.erase( boosts.begin() + i );
	//		}
	//	}

	//	//draw_list->AddPolyline( prediction::points(), prediction_points, ImColor( 0, 0, 0 ), false, 4.f );
	//	//draw_list->AddPolyline( prediction::points(), prediction_points, ImColor( 255, 0, 255 ), false, 2.f );
	//	

	//	//render::ball( draw_list );

	//	if ( autodemo::target ) {
	//		render::pawn( draw_list, autodemo::target );
	//	}
	//}

	ImGui::EndFrame();
	ImGui::Render();

	g_context->OMSetRenderTargets( 1, &g_render, NULL );
	ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );

	return o_present( g_pSwapChain, syncInterval, flags );
}

/* #####################################################################################################  */

inline void PostRender( UCanvas* canvas ) {
	//canvas->Draw2DLine( 20, 20, 500, 500, { 255, 0, 255, 255 } );

	auto pos = canvas->Project( g_ball->Location );
	auto points = prediction::points();

	for ( uint8_t i = 0; i < prediction_points - 1; ++i ) {

		auto p1 = points[i];
		auto p2 = points[i + 1];
		//auto p1 = canvas->Project( predict_infos[i + 0].Location );
		//auto p2 = canvas->Project( predict_infos[i + 1].Location );

		canvas->Draw2DLine( p1.x, p1.y, p2.x, p2.y, { 255, 0, 255, 255 } );
	}

	//canvas->SetPos( pos.X - 15, pos.Y - 15, 0 );
	//canvas->SetDrawColor( 255, 0, 0, 255 );
	//canvas->DrawBox( 30, 30 );
}

void __fastcall hk_processevent( UObject* object, UFunction* function, void* params, void* result ) {

	//if ( object->GetFullName().find( "Boost" ) != std::string::npos || function->GetFullName().find( "Boost" ) != std::string::npos ) {
	//	print_information( object->GetFullName() );
	//	print_information( function->GetFullName() );
	//}

	if ( !utils::init_frame() ) {
		return o_processevent( object, function, params, result );
	}

	//if ( object->GetFullName().find("VehiclePickup_Boost_TA") != std::string::npos ) {

	//	AVehiclePickup_Boost_TA* boost = reinterpret_cast<AVehiclePickup_Boost_TA*>( object );
	//	

	//	if ( &boost->Timers && boost->Timers.Num() == 1 ) {
	//		printf( "added boost\n" );
	//		auto present = false;

	//		for ( auto& b : boosts ) {
	//			if ( b->Location == boost->Location ) {
	//				present = true;
	//				break;
	//			}
	//		}

	//		if ( !present ) boosts.push_back( boost );
	//	}
	//}

	//if ( function->GetName() == "PostRender" && params ) {
	//	
	//	PostRender( reinterpret_cast<UGameViewportClient_eventPostRender_Params*>( params )->Canvas );

	//	return o_processevent( object, function, params, result );
	//}

	if ( function->GetName() != "Tick" ) {
		return o_processevent( object, function, params, result );
	}

	//prediction::calculate();
	autodemo::execute();

	return o_processevent( object, function, params, result );
}

/* #####################################################################################################  */

void exec() {

	if ( !utils::initialize_sdk() ) {
		return;
	}

	//if ( !hook_present() ) {
	//	print_error( "present hook failed" );
	//	return;
	//}

	console::println( ( std::stringstream() << "gNames: "   << GNames   ).str() );
	console::println( ( std::stringstream() << "gObjects: " << GObjects ).str() );


	g_engine = utils::engine();
	//g_engine = reinterpret_cast<UGameEngine*>( GObjects->At( 141387 ) );
	//print_information( g_engine->GetFullName() );

	if ( !g_engine ) {
		print_error( "engine null" );
		//unhook_present();
		return;
	}

	g_local = utils::local();

	UObject::FindClass( "" );

	console::println( "running ...", msg_color::light_magenta, msg_type::information );

	while ( !input::is_key_down( VK_DELETE ) ) {

		Sleep(1);

		if (input::is_key_down(VK_F12)) {
			chat::send("Rat!");
		}

		if ( !utils::init_frame() ) {
			continue;
		}

		autodemo::execute();
	}

	//unhook_present();
}

DWORD WINAPI initialize( LPVOID instance ) {

	console::attach();

	console::println( "========= injected =========", msg_color::yellow, msg_type::success );

	MH_Initialize();

	exec();

	MH_Uninitialize();

	console::println( "======== uninjected ========", msg_color::yellow, msg_type::success );

	Sleep( 500 );

	FreeLibraryAndExitThread( static_cast<HMODULE>( instance ), 0 );

	return TRUE;
}

DWORD WINAPI release( VOID ) {
	console::detach();
	return TRUE;
}

std::int32_t WINAPI DllMain( const HMODULE instance, const unsigned long reason, const void* reserved ) {

	DisableThreadLibraryCalls( instance );

	if ( reason == DLL_PROCESS_ATTACH ) {
		if ( auto handle = CreateThread( nullptr, NULL, initialize, instance, NULL, nullptr ) )
			CloseHandle( handle );
	}

	if ( reason == DLL_PROCESS_DETACH ) {
		release();
	}

	return true;
}