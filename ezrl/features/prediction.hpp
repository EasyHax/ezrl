#pragma once

#include "../math/math.hpp"

#define prediction_points 60
#define prediction_factor 4

FBallPredictionInfo predict_infos  [prediction_points];
ImVec2              predict_points [prediction_points];

namespace prediction {

	uint8_t predict_count = 0;

	inline void calculate() {

		if ( !g_ball || g_ball->bDeleteMe ) {
			return;
		}

		auto time = ( predict_count + 1.f ) * prediction_factor / prediction_points;

		predict_infos[predict_count] = FBallPredictionInfo{};
		g_ball->PredictPosition( time, predict_infos[predict_count] );

		predict_count = predict_count == prediction_points ? 0 : predict_count + 1;
	}

	ImVec2* points( void ) {

		for ( uint8_t i = 0; i < prediction_points; ++i ) {

			auto p1 = world_to_screen( predict_infos[i].Location );

			predict_points[i] = ImVec2( p1.X, p1.Y );
		}

		return predict_points;
	}
}