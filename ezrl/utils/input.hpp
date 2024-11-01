#pragma once
#include "windows.h"

namespace input {
    
    uint8_t  keys[] = { 'A', 'E', 'Z', 'Q', 'S', 'D', VK_LBUTTON, VK_RBUTTON };
    uint16_t keys_state;

    inline bool is_key_down(uint8_t key) {
        return GetAsyncKeyState(key) & 0x8000;
    }

    inline void send_key(uint8_t& key, bool release = false) {
        keybd_event(key, 0, release ? KEYEVENTF_KEYUP : NULL, 0);
    }

    inline void send_key(char key, bool release = false) {
        send_key((uint8_t&)key, release);
    }

    inline void press_key(char key) {
        send_key(key, false);
        send_key(key, true );
    }

    inline void send_mouse(unsigned long key, bool release = false) {
        auto code = release ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_LEFTDOWN;  
        code = key == VK_LBUTTON ? code : (code << 2);
        mouse_event(code, 0, 0, 0, 0);
    }

    void save_keys(void) {

        keys_state = 0;

        for (uint8_t i = 0; i < _countof(keys); ++i) {
            if (is_key_down(keys[i])) {
                keys_state |= (1 << i);
            }
        }
    }

    void restore_keys(void) {

        for (uint8_t i = 0; i < _countof(keys); ++i) {

            auto release = bool(~keys_state & (1 << i));

            if (keys[i] <= VK_RBUTTON) {
                send_mouse(keys[i], release);
            }
            else 
            {
                send_key(keys[i], release);
            }
        }
    }
}

namespace direction {

    /*
   
        go to C:\Users\???\Documents\My Games\Rocket League\TAGame\Config\TAInput.ini
        go to the [ProjectX.ControlPreset_X] section
        copy past these two lines :
        -----------------------------------------
        PCBindings=( Action="SteerRight",Key="O" )
        PCBindings=( Action="SteerLeft",Key="I" )
        -----------------------------------------
    */

    enum class dir { none, left = 'I', right = 'O' };

    dir current_direction = dir::none;

    bool has_jumped = false;
    uint64_t ticks = 0, delay = 3;

    void to(dir direction) {

        if (direction == current_direction || direction == dir::none) { return; }

        input::send_key((uint8_t)current_direction, true);
        input::send_key((uint8_t)direction);

        current_direction = direction;
    }

    void jump( void ) {
        if ( !has_jumped ) {
            mouse_event( MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0 );
            mouse_event( MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0 );
            has_jumped = true;
            ticks = GetTickCount64();
        }
    }

    void reset_jump( void ) {

        if ( has_jumped ) {
            if ( GetTickCount64() - ticks > delay * 1000 ) {
                has_jumped = false;
            }
        }
    }

    void reset( void ) {
        if (current_direction != dir::none) {
            keybd_event((uint8_t)current_direction, 0, KEYEVENTF_KEYUP, 0);
        }
        current_direction = dir::none;
    }
}