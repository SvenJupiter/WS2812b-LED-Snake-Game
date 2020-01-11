#include "stdint.h"
#include "FastLED.h"
#include "Ringbuffer.h"
#include <utility>


class Direction {

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


    public:

    static const Direction Up;
    static const Direction Down;
    static const Direction Left;
    static const Direction Right;
    static const Direction UpRight;
    static const Direction UpLeft;
    static const Direction DownLeft;
    static const Direction DownRight;

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



    public:

};




class Fruit {


    public:

        enum Type : int32_t {
            Normal_Type, // Lets Snake grow
            SpeedBoost_Type, // Make Snake faster for a few seconds
            SlowDown_Type, // Make Snake slower for a few seconds
            Ghost_Type, // Snake can pass over its body without biting it off
            Rainbow_Type, // 
        };

        enum Color : int32_t {
            Normal_Color = CRGB::Green,
            SpeedBoost_Color = CRGB::Red,
            SlowDown_Color = CRGB::Blue,
            Ghost_Color = CRGB::White,
            Rainbow_Color = CRGB::Violet,
        };

    public:
        Position position;
        Type type;

    Fruit(const Position& pos, const Type& fruit_type = Normal_Type): position(pos), type(fruit_type) {}

    Fruit(const Fruit& other): position(other.position), type(other.type) {}

    void operator=(const Fruit& other) {
        if (this != &other) {
            this->position = other.position;
            this->type = other.type;
        }
    }
};


class Snake {

    public:

        // int32_t length;

        Ringbuffer<Position> body;
        CHSV head_color;
        CHSV body_base_color;
        uint8_t length_color_modifier;
        uint8_t time_color_modifier;

    public:

        Snake(const Position initial_pos, const uint32_t initial_length = 5): body(initial_length, initial_pos) {}


        Position& head() { return this->body.front(); }
        const Position& head() const { return this->body.front(); }
        const Position& tail() const { return this->body.back(); }
        Position& tail() { return this->body.back(); }
        int32_t length() const { return this->body.size(); }

        void grow() {
            this->body.insert(this->body.end(), this->body.back());
        }

        void eat(const Fruit& fruit) {
            this->grow();
        }

        int32_t bite_off_tail(const Ringbuffer<Position>::const_iterator& bite_mark) {
            this->body.erase(bite_mark, this->body.end());
            return this->length();
        }

        void move(const Position& new_head_position) {
            this->body.push_front(new_head_position);
        }

        std::pair<bool, Ringbuffer<Position>::const_iterator> is_biting_itself() const {

            // for every body part
            for (auto body_part = this->body.begin()+1; body_part != this->body.end(); ++body_part) {

                // check if position of body_part is the same as the position of the head
                if (this->head() == *body_part) {
                    return std::pair<bool, Ringbuffer<Position>::const_iterator>(true, body_part);
                }
            }

            return std::pair<bool, Ringbuffer<Position>::const_iterator>(false, this->body.end());
        }

    


};

class GameBoard {

    public:

        int32_t width;
        int32_t height;


        bool loop_x;
        bool loop_y;

        bool invert_x_movement;
        bool invert_y_movement;

        CHSV board_color;

    public:

        GameBoard(const int32_t Width, const int32_t Height,
            const bool Loop_x = true, const bool Loop_y = true,
            const bool Invert_x = false, const bool Invert_y = false,
            CHSV Board_color = CHSV(0,0,0)): \
        width(Width), height(Height), \
        loop_x(Loop_x), loop_y(Loop_y), \
        invert_x_movement(Invert_x), \
        invert_y_movement(Invert_y), \
        board_color(Board_color) {}

};


Fruit create_random_fruit(const GameBoard& game_board, const std::vector<Fruit>& fruits, const Snake& snake);

void snake_game_task(void*);