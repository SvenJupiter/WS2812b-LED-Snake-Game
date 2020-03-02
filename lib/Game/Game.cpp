#include <Arduino.h>

#include "Game.h"
#include "freertos/task.h"
#include <PS4Controller.h>

namespace Game {


const Direction Direction::Up(0, -1);
const Direction Direction::Down(0, 1);
const Direction Direction::Left(-1, 0);
const Direction Direction::Right(1, 0);
const Direction Direction::UpRight(1, -1);
const Direction Direction::UpLeft(-1, -1);
const Direction Direction::DownLeft(-1, 1);
const Direction Direction::DownRight(1, 1);
const Direction Direction::None(0, 0);


Direction get_direction_from_ps4_control_pad() {

    if (PS4.isConnected()) {

        if (PS4.data.button.up) { // up
            Serial.println("Up Button");
            return Direction::Up;
        }
        else if (PS4.data.button.down) { // down
            Serial.println("Down Button");
            return Direction::Down;
        }
        else if (PS4.data.button.left) { // left
            Serial.println("Left Button");
            return Direction::Left;
        }
        else if (PS4.data.button.right) { // right
            Serial.println("Right Button");
            return Direction::Right;
        }
        else if (PS4.data.button.upright) { // upright
            Serial.println("Up Right Button");
            return Direction::UpRight;
        }
        else if (PS4.data.button.upleft) { // upleft
            Serial.println("Up Left Button");
            return Direction::UpLeft;
        }
        else if (PS4.data.button.downleft) { // downleft
            Serial.println("Down Left Button");
            return Direction::DownLeft;
        }
        else if (PS4.data.button.downright) { // downright
            Serial.println("Down Right Button");
            return Direction::DownRight;
        }
    }
    // else
    return Direction::None;
}

Direction get_direction_from_ps4_analog_stick(const bool left_stick, const uint32_t magnitude_thershold) {

    if (PS4.isConnected()) {

        // const bool movement = ( \
        //     PS4.event.analog_move.stick.lx || PS4.event.analog_move.stick.ly || \
        //     PS4.event.analog_move.stick.rx || PS4.event.analog_move.stick.ry \
        // );

        // Get analog values from analog stick
        const int32_t analog_x = (left_stick ? PS4.data.analog.stick.lx :  PS4.data.analog.stick.rx);
        const int32_t analog_y = (left_stick ? PS4.data.analog.stick.ly :  PS4.data.analog.stick.ry);

        // Compare magnitude
        const int32_t magnitude_squared = ((analog_x*analog_x) + (analog_y*analog_y));
        if (magnitude_thershold*magnitude_thershold > magnitude_squared) {
            return Direction::None;
        }

    
        #define comp(value) (((value)*(16777216L))/(40503782L))

        if (analog_y > 0 && abs(analog_x) <= comp(analog_y)) { // up
            Serial.println("Stick Up");
            return Direction::Up;
        }
        else if (analog_y < 0 && abs(analog_x) <= comp(-analog_y)) { // down
            Serial.println("Stick Down");
            return Direction::Down;
        }
        else if (analog_x < 0 && abs(analog_y) <= comp(-analog_x)) { // left
            Serial.println("Left Button");
            return Direction::Left;
        }
        else if (analog_x > 0 && abs(analog_y) <= comp(analog_x)) { // right
            Serial.println("Stick Right");
            return Direction::Right;
        }
        else if (analog_y > comp(analog_x) && analog_x > comp(analog_y)) { // upright
            Serial.println("Stick Up Right");
            return Direction::UpRight;
        }
        else if (analog_y > comp(-analog_x) && (-analog_x) > comp(analog_y)) { // upleft
            Serial.println("Stick Up Left");
            return Direction::UpLeft;
        }
        else if (analog_y < comp(analog_x) && analog_x < comp(analog_y)) { // downleft
            Serial.println("Stick Down Left");
            return Direction::DownLeft;
        }
        else if (analog_x > comp(-analog_y) && (-analog_y) > comp(analog_x)) { // downright
            Serial.println("Stick Down Right");
            return Direction::DownRight;
        }
    }
    // else
    return Direction::None;
}

Direction get_direction_from_ps4() {
    return get_direction_from_ps4_control_pad() + get_direction_from_ps4_analog_stick(true, 100);
}


}; // namespace Game