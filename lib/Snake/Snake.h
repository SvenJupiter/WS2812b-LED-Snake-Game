#include "stdint.h"
#include "FastLED.h"
#include "Ringbuffer.h"
#include "LedMatrix.h"
#include <utility>
#include <set>


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

        Direction& normalize() {
            this->x = ((this->x > 0) ? 1 : ((this->x > 0) ? -1 : 0));
            this->y = ((this->y > 0) ? 1 : ((this->y > 0) ? -1 : 0));
            return *this;
        }


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
            Normal_Color = CRGB::Blue,
            SpeedBoost_Color = CRGB::Red,
            SlowDown_Color = CRGB::Green,
            Ghost_Color = CRGB::White,
            Rainbow_Color = CRGB::Violet,
        };

    public:
        Position position;
        Type type;
        CRGB color;

    Fruit(const Position& pos, const Type& fruit_type = Normal_Type, const CRGB& Color = CRGB::Blue): position(pos), type(fruit_type), color(Color) {}

    Fruit(const Fruit& other): position(other.position), type(other.type) {}

    void operator=(const Fruit& other) {
        if (this != &other) {
            this->position = other.position;
            this->type = other.type;
        }
    }

    // Needed to put Fruits in a set (order doesn't matter in this case)
    bool operator<(const Fruit& other) const { return false; }
};
typedef std::set<Fruit> FruitList;


class Snake {

    public:

        // int32_t length;

        Ringbuffer<Position> body;
        CRGB head_color;
        CRGB body_base_color;
        uint8_t length_color_modifier;
        uint8_t time_color_modifier;

    public:

        Snake(const Position initial_pos, const uint32_t initial_length = 5): body(initial_length, initial_pos), head_color(CRGB::Red), body_base_color(CRGB::Green) {}


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

Direction get_direction_from_game_ai(const GameBoard& game_board, const FruitList& fruits, const Snake& snake);


Fruit create_random_fruit(const GameBoard& game_board, const FruitList& fruits, const Snake& snake);

void draw(LedMatrix& led_matrix, const GameBoard& game_board, const FruitList& fruits, const Snake& snake, const bool write_to_leds = true);

void snake_game_task(void*);