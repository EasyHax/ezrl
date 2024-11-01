#pragma once

#include "../imgui/imgui.h"

#include "../math/math.hpp"

namespace render {

	void ball( ImDrawList* draw_list, ImColor color = ImColor( 255, 0, 255 ) ) {

		auto p1 = world_to_screen( g_ball->Location );
		auto d1 = g_ball->Location.distance( g_local->Actor->Pawn->Location );
		auto s1 = 5000 / d1 + 10;

		draw_list->AddCircle( ImVec2( p1.X, p1.Y ), 5000 / d1 + 10, color );
	}

	void pawn( ImDrawList* draw_list, APawn* pawn, ImColor color = ImColor( 255, 0, 255 ) ) {

		auto p1 = world_to_screen( pawn->Location );
		auto d1 = g_ball->Location.distance( g_local->Actor->Pawn->Location );
		auto s1 = ( 5000 / d1 + 10 ) / 2;

		draw_list->AddRect( ImVec2( p1.X - s1, p1.Y - s1 ), ImVec2( p1.X + s1, p1.Y + s1 ), color );
	}
}