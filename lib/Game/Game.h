#pragma once

#include "stdint.h"
#include <string>
#include <utility>
#include <set>
#include <stdarg.h> // Variadic functions
#include "FastLED.h" // Color

namespace Game {

template <typename... Args>
std::string format_str(const std::string fmt, Args... args) {

    char* buf = nullptr; 
    const int res = asprintf(&buf, fmt.c_str(), args...);

    if (res >= 0) {
        const std::string temp(buf); 
        free(buf); 
        return temp; 
    } 
    else { 
        return ""; 
    }
}

class Direction {

    public:

        static const Direction Up;
        static const Direction Down;
        static const Direction Left;
        static const Direction Right;
        static const Direction UpRight;
        static const Direction UpLeft;
        static const Direction DownLeft;
        static const Direction DownRight;
        static const Direction None;

    public:

        int32_t x;
        int32_t y;

        // default constuctor
        Direction(const int32_t x_dir, const int32_t y_dir): x(x_dir), y(y_dir) {}

        // copy constructor
        Direction(const Direction& other): x(other.x), y(other.y) {}

        void operator=(const Direction& other) {
            if (this != &other) {
                this->x = other.x;
                this->y = other.y;
            }
        }

        bool operator==(const Direction& other) const {
            return ((this->x == other.x) && (this->y == other.y));
        }

        bool operator!=(const Direction& other) const {
            return !(*this == other);
        }

        Direction operator+(const Direction& other) {
            Direction dir(this->x + other.x, this->y + other.y);
            return dir;
        }

        void set_x(const int32_t x_dir) { this->x = x_dir; }
        void set_y(const int32_t y_dir) { this->y = y_dir; }
        void set_xy(const int32_t x_dir, const int32_t y_dir) { this->set_x(x_dir); this->set_y(y_dir); }

        Direction& normalize() {
            this->x = ((this->x > 0) ? 1 : ((this->x < 0) ? -1 : 0));
            this->y = ((this->y > 0) ? 1 : ((this->y < 0) ? -1 : 0));
            return *this;
        }

        std::string to_string() const {
            if (*this == Up) { return "Up"; }
            else if (*this == Down) { return "Down"; }
            else if (*this == Left) { return "Left"; }
            else if (*this == Right) { return "Right"; }
            else if (*this == UpRight) { return "UpRight"; }
            else if (*this == UpLeft) { return "UpLeft"; }
            else if (*this == DownLeft) { return "DownLeft"; }
            else if (*this == DownRight) { return "DownRight"; }
            else if (*this == None) { return "None"; }
            else { return format_str("(%d/%d)", this->x, this->y); }
        }


};

class Position {

    public:

        int32_t x;
        int32_t y;

        // default constuctor
        Position(const int32_t x_pos, const int32_t y_pos): x(x_pos), y(y_pos) {}

        // copy constructor
        Position(const Position& other): x(other.x), y(other.y) {}

        void operator=(const Position& other) {
            if (this != &other) {
                this->x = other.x;
                this->y = other.y;
            }
        }

        bool operator==(const Position& other) const {
            return ((this->x == other.x) && (this->y == other.y));
        }

        bool operator!=(const Position& other) const {
            return !(*this == other);
        }

        void set_x(const int32_t x_pos) { this->x = x_pos; }
        void set_y(const int32_t y_pos) { this->y = y_pos; }
        void set_xy(const int32_t x_pos, const int32_t y_pos) { this->set_x(x_pos); this->set_y(y_pos); }


        Position& operator+=(const Direction& dir) {
            this->x += dir.x;
            this->y += dir.y;
            return *this;
        }

        Position operator+(const Direction& dir) const {
            Position pos = *this;
            pos += dir;
            return pos;
        }

        Position& operator-=(const Direction& dir) {
            this->x -= dir.x;
            this->y -= dir.y;
            return *this;
        }

        Position operator-(const Direction& dir) const {
            Position pos = *this;
            pos -= dir;
            return pos;
        }

        // prefix ++ // change x 
        Position& operator++() {
            this->x += 1;
            return *this;
        }

        // prefix -- // change x 
        Position& operator--() {
            this->x -= 1;
            return *this;
        }

        // postfix ++ // change y 
        Position& operator++(int) {
            this->y += 1;
            return *this;
        }

        // postfix -- // change y 
        Position& operator--(int) {
            this->y -= 1;
            return *this;
        }


        static int32_t distance_squared(const Position& p1, const Position& p2) {
            const int32_t dx = p2.x - p1.x;
            const int32_t dy = p2.y - p1.y;
            return (dx*dx + dy*dy);
        }

        static Direction direction(const Position& start_point, const Position& end_point) {
            const int32_t dx = end_point.x - start_point.x;
            const int32_t dy = end_point.y - start_point.y;
            return Direction(dx, dy);
        }

        Direction operator-(const Position& start_point) const {
            return direction(start_point, *this);
        }


        std::string to_string() const { return format_str("(%d/%d)", this->x, this->y); }


    public:

};

class GameBoard {

    public:

        int32_t width;
        int32_t height;


        bool loop_x;
        bool loop_y;

        bool invert_x_movement;
        bool invert_y_movement;

        CRGB board_color;

    public:

        GameBoard(const int32_t Width, const int32_t Height,
            const bool Loop_x = true, const bool Loop_y = true,
            const bool Invert_x = false, const bool Invert_y = false,
            CRGB Board_color = CRGB::Black): \
        width(Width), height(Height), \
        loop_x(Loop_x), loop_y(Loop_y), \
        invert_x_movement(Invert_x), \
        invert_y_movement(Invert_y), \
        board_color(Board_color) {}

        uint32_t size() const { return this->width * this->height; }

};


Direction get_direction_from_ps4_control_pad();
Direction get_direction_from_ps4_analog_stick(const bool left_stick = true, const uint32_t magnitude_thershold = 100);
Direction get_direction_from_ps4();

}; // namecpace Game