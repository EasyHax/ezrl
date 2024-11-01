#pragma once

FVector world_to_screen(	FVector world_location, 
							FVector camera_position  = g_local->Actor->PlayerCamera->Location, 
							FRotator camera_rotation = g_local->Actor->PlayerCamera->Rotation, 
							float fov                = g_local->Actor->PlayerCamera->DefaultFOV, 
							uint16_t screen_w        = 1920, 
							uint16_t screen_h        = 1080 ) {
	
	float pitch = camera_rotation.Pitch * float( CONST_Pi ) / 32767;
	float yaw   = camera_rotation.Yaw * float( CONST_Pi )   / 32767;
	float roll  = camera_rotation.Roll * float( CONST_Pi )  / 32767;

	FVector x_axis{
		cos( pitch ) * cos( yaw ), 
		cos( pitch ) * sin( yaw ), 
		sin( pitch ) 
	};

	FVector y_axis{ 
		sin( roll ) * sin( pitch ) * cos( yaw ) - cos( roll ) * sin( yaw ),
		sin( roll ) * sin( pitch ) * sin( yaw ) + cos( roll ) * cos( yaw ), 
		-sin( roll ) * cos( pitch ) 
	};

	FVector z_axis{ 
		-( cos( roll ) * sin( pitch ) * cos( yaw ) + sin( roll ) * sin( yaw ) ), 
		cos( yaw ) * sin( roll ) - cos( roll ) * sin( pitch ) * sin( yaw ), 
		cos( roll ) * cos( pitch ) 
	};

	FVector delta = world_location - camera_position;
	FVector trans = FVector( delta.dot( y_axis ), delta.dot( z_axis ), delta.dot( x_axis ) );

	if ( trans.Z < 1.f )
		trans.Z = 1.f;

	float x_center = screen_w / 2.0f;
	float y_center = screen_h / 2.0f;

	return FVector{
		x_center + trans.X * ( x_center / tan( fov * (float)CONST_Pi / 360.f ) ) / trans.Z,
		y_center - trans.Y * ( x_center / tan( fov * (float)CONST_Pi / 360.f ) ) / trans.Z
	};
}

float proportional_navigation( const AActor* local, const AActor* target, float predict_factor = 1.0f ) {

	if ( !local ) return 0;

	auto rel_location = target->Location - local->Location;
	auto rel_velocity = target->Velocity - local->Velocity;

	auto time = rel_location.length() / rel_velocity.length();
	auto col_location = target->Location + target->Velocity * time * predict_factor;
	auto vec_location = col_location - local->Location;

	return local->Velocity.angle( vec_location ) * CONST_RadToDeg;
}