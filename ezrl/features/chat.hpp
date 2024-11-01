#pragma once

#include "../utils/input.hpp"

namespace chat {

	const char chat_key = 'T';

	void send( std::string message ) {

		input::save_keys();
		input::press_key( chat_key );

		Sleep( 5 );

		auto caps_lock = GetKeyState( VK_CAPITAL ) & 0x0001;

		for ( auto& chr : message ) {

			auto key = VkKeyScanA( chr );
			auto upper = std::isupper( chr );

			if ( upper != caps_lock ) {
				input::press_key( VK_CAPITAL );
				caps_lock = upper;
			}

			input::press_key( key );
		}

		input::press_key( VK_RETURN );

		Sleep( 100 );

		input::restore_keys();
	}
}