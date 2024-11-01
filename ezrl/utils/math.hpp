#pragma once

#include "stdint.h"
#include "SDK/Engine_parameters.hpp"

FVector world_to_screen( FVector world_location, FVector camera_position, FVector camera_rotation, float fov, uint16_t screen_w = 1280, uint16_t screen_h = 720 ) {
	// Convert camera rotation to radians
	float pitch = camera_rotation.X * M_PI / 180.0f;
	float yaw = camera_rotation.Y * M_PI / 180.0f;
	float roll = camera_rotation.Z * M_PI / 180.0f;

	// Calculate camera forward, right, and up vectors
	FVector forward;
	forward.X = cosf( pitch ) * cosf( yaw );
	forward.Y = sinf( pitch );
	forward.Z = cosf( pitch ) * sinf( yaw );

	FVector right;
	right.X = -sinf( yaw );
	right.Y = 0;
	right.Z = cosf( yaw );

	FVector up = crossProduct( right, forward );

	// Calculate relative position of world_location to camera
	FVector rel_pos;
	rel_pos.X = world_location.X - camera_position.X;
	rel_pos.Y = world_location.Y - camera_position.Y;
	rel_pos.Z = world_location.Z - camera_position.Z;

	// Project relative position onto camera's forward vector
	float dist = dotProduct( rel_pos, forward );

	// Calculate screen position of world_location
	FVector screen_pos;
	screen_pos.X = screen_w / 2.0f + ( dotProduct( rel_pos, right ) / dist ) * screen_w / tanf( fov / 2.0f );
	screen_pos.Y = screen_h / 2.0f - ( dotProduct( rel_pos, up ) / dist ) * screen_w / tanf( fov / 2.0f );
	screen_pos.Z = dist;

	return screen_pos;
}