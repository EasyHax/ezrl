#pragma once

#include "../math/math.hpp"

namespace autodemo {

	APawn* target = NULL;
	const float th = 0.5;

	inline void execute() {
		if ( input::is_key_down( 'V' ) ) {

			if ( target && target->Velocity.is_null() ) {
				direction::reset(); target = NULL;
				return;
			}

			if ( target || ( target = utils::closest_target_aligned( g_local->Actor->Pawn, g_local->Actor->WorldInfo->PawnList ) ) ) {

				auto steer = proportional_navigation( g_local->Actor->Pawn, target, 0.9f );

				if ( steer > th ) { direction::to( direction::dir::right ); }
				else if ( steer < -th ) { direction::to( direction::dir::left ); }
				else direction::reset();
			}
		}
		else { direction::reset(); target = NULL; }
	}
}