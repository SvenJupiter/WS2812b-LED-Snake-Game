#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <stdint.h>
#include <utility>
#include "FastLED.h"







class LedMatrix {

    public:

    enum WiringPattern: uint32_t {
        HorizontalLines,
        HorizontalZigZag,
        VerticalLines,
        VerticalZigZag,
    };

    enum WiringStart: uint32_t {
        TopLeft,
        TopRight,
        BottomLeft,
        BottemRight,
    };

    protected:

        typedef std::vector<CRGB> parent;
        typedef CRGB value_type;
        typedef CRGB* pointer;
        typedef const CRGB* const_pointer;
        typedef CRGB& reference;
        typedef const CRGB& const_reference;

        // Base Matrix
        pointer leds;
        uint32_t base_width;
        uint32_t base_height;
        WiringStart wiring_start_point; 
        WiringPattern wiring_pattern;

        // Submatirx
        uint32_t submatix_row_offset;
        uint32_t submatix_column_offset;
        uint32_t submatix_width;
        uint32_t submatix_height;
        bool submatix_reverse_rows;
        bool submatix_reverse_colums;

        std::pair<uint32_t, uint32_t> submatrix_index_to_base_matrix_index(const uint32_t subrow_index, const uint32_t subcolumn_index) const;
        uint32_t matrix_index_to_led_string_index(const uint32_t row_index, const uint32_t column_index) const;


    public:

        LedMatrix(pointer leds, uint32_t width, uint32_t height, 
            WiringStart wiring_start_point, WiringPattern wiring_pattern
        );

        LedMatrix(const LedMatrix& other);

        void operator=(const LedMatrix& other);


        // Implicit conversation
        operator pointer() { return this->leds; }
        operator const_pointer() const { return this->leds; }

        // Index operator
        reference operator[](const uint32_t index);
        const_reference operator[](const uint32_t index) const;

        // Index operator
        reference operator()(const uint32_t row_index, const uint32_t column_index);
        const_reference operator()(const uint32_t row_index, const uint32_t column_index) const;

        // Create Submatrix
        LedMatrix submat(const uint32_t Row_offset, const uint32_t Column_offset, 
            const uint32_t Submatix_width, const uint32_t Submatix_height, 
            const bool Reverse_row = false, const bool Reverse_column = false) const;

        // Reset to Base-Matrix
        LedMatrix basemat() const;

        // Dimensions & Size
        uint32_t width() const { return this->submatix_width; }
        uint32_t height() const { return this->submatix_height; }
        uint32_t size() const { return this->width() * this->height(); }

};







#endif