#pragma once

#include "windows.h"
#include "stdio.h"

namespace msg_type {
	const char* success = "[+] ";
	const char* error = "[!] ";
	const char* information = "[~] ";
	const char* raw = "";
};

enum class msg_color {

	bright = FOREGROUND_INTENSITY,
	blue = FOREGROUND_BLUE,
	red = FOREGROUND_RED,
	green = FOREGROUND_GREEN,

	cyan = blue | green,
	yellow = red | green,
	magenta = blue | red,
	white = blue | red | green,

	light_blue = blue | bright,
	light_red = red | bright,
	light_green = green | bright,

	light_cyan = blue | green | bright,
	light_yellow = red | green | bright,
	light_magenta = blue | red | bright,
	light_white = blue | red | green | bright
};

namespace console {

	inline bool print( const std::string msg, msg_color color = msg_color::light_green, const char* type = msg_type::information ) {

		auto hout = GetStdHandle( STD_OUTPUT_HANDLE );
		auto info = CONSOLE_SCREEN_BUFFER_INFO{};

		if ( GetConsoleScreenBufferInfo( hout, &info ) ) {
			if ( SetConsoleTextAttribute( hout, (short)color ) ) {

				std::cout << type << msg;

				if ( SetConsoleTextAttribute( hout, info.wAttributes ) ) {
					return true;
				}
			}
		}

		return false;
	}

	inline bool println( const std::string msg, msg_color color = msg_color::light_green, const char* type = msg_type::information ) {
		return print( msg + "\n", color, type );
	}

	bool attach(void) {

		AllocConsole();

		freopen_s(reinterpret_cast<_iobuf**>(__acrt_iob_func(0)), "conin$",  "r", static_cast<_iobuf*>(__acrt_iob_func(0)));
		freopen_s(reinterpret_cast<_iobuf**>(__acrt_iob_func(1)), "conout$", "w", static_cast<_iobuf*>(__acrt_iob_func(1)));
		freopen_s(reinterpret_cast<_iobuf**>(__acrt_iob_func(2)), "conout$", "w", static_cast<_iobuf*>(__acrt_iob_func(2)));

		SetWindowPos( GetConsoleWindow(), 0, 0, 0, 300, 400, SWP_NOZORDER | SWP_NOMOVE );

		return true;
	}

	bool detach() {

		fclose(static_cast<_iobuf*>(__acrt_iob_func(0)));
		fclose(static_cast<_iobuf*>(__acrt_iob_func(1)));
		fclose(static_cast<_iobuf*>(__acrt_iob_func(2)));

		FreeConsole();

		return true;
	}
}