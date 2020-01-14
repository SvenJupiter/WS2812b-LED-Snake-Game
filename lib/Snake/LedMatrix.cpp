#include "LedMatrix.h"


std::pair<uint32_t, uint32_t> LedMatrix::submatrix_index_to_base_matrix_index(const uint32_t subrow_index, const uint32_t subcolumn_index) const {

    const uint32_t baserow_index = submatix_row_offset + (submatix_reverse_rows ? (submatix_height - (subrow_index + 1)) : subrow_index);
    const uint32_t basecolumn_index = submatix_column_offset + (submatix_reverse_colums ? (submatix_width - (subcolumn_index + 1)) : subcolumn_index);
    return std::pair<uint32_t, uint32_t>(baserow_index, basecolumn_index);
}

uint32_t LedMatrix::matrix_index_to_led_string_index(
    const uint32_t row_index, const uint32_t column_index) const
{

    switch (wiring_pattern) {
        case HorizontalLines:
            switch (wiring_start_point) {
                case TopLeft: 
                    return (row_index * base_width) + column_index;
                case TopRight: 
                    return (row_index * base_width) + (base_width - (column_index + 1));
                case BottomLeft: 
                    return ((base_height - (row_index + 1)) * base_width) + column_index;
                case BottemRight: 
                    return ((base_height - (row_index + 1)) * base_width) + (base_width - (column_index + 1));
            }
            break;
        case HorizontalZigZag:
            switch (wiring_start_point) {
                case TopLeft: 
                    return (row_index * base_width) + ((row_index % 2 == 0) ? column_index : (base_width - (column_index + 1)));
                case TopRight: 
                    return (row_index * base_width) + ((row_index % 2 == 0) ? (base_width - (column_index + 1)) : column_index);
                case BottomLeft: 
                    return ((base_height - (row_index + 1)) * base_width) + (((base_height - (row_index + 1)) % 2 == 0) ? column_index : (base_width - (column_index + 1)));
                case BottemRight: 
                    return ((base_height - (row_index + 1)) * base_width) + (((base_height - (row_index + 1)) % 2 == 0) ? (base_width - (column_index + 1)) : column_index);
            }
            break;
        case VerticalLines:
            switch (wiring_start_point) {
                case TopLeft: 
                    return (column_index * base_height) + row_index;
                case BottomLeft: 
                    return (column_index * base_height) + (base_height - (row_index + 1));
                case TopRight: 
                    return ((base_width - (column_index + 1)) * base_height) + row_index;
                case BottemRight: 
                    return ((base_width - (column_index + 1)) * base_height) + (base_height - (row_index + 1));
            }
            break;
        case VerticalZigZag:
            switch (wiring_start_point) {
                case TopLeft: 
                    return (column_index * base_height) + ((column_index % 2 == 0) ? row_index : (base_height - (row_index + 1)));
                case BottomLeft: 
                    return (column_index * base_height) + ((column_index % 2 == 0) ? (base_height - (row_index + 1)) : row_index);
                case TopRight: 
                    return ((base_width - (column_index + 1)) * base_height) + (((base_width - (column_index + 1)) % 2 == 0) ? row_index : (base_height - (row_index + 1)));
                case BottemRight: 
                    return ((base_width - (column_index + 1)) * base_height) + (((base_width - (column_index + 1)) % 2 == 0) ? (base_height - (row_index + 1)) : row_index);
            }
            break;
    }

}


LedMatrix::LedMatrix(pointer Leds, uint32_t Width, uint32_t Height, 
    WiringStart Wiring_start_point, WiringPattern Wiring_pattern)
:
    leds(Leds),
    base_width(Width),
    base_height(Height),
    wiring_start_point(Wiring_start_point),
    wiring_pattern(Wiring_pattern),
    submatix_row_offset(0),
    submatix_column_offset(0),
    submatix_width(Width),
    submatix_height(Height),
    submatix_reverse_rows(false),
    submatix_reverse_colums(false)
{}

LedMatrix::LedMatrix(const LedMatrix& other) 
:
    leds(other.leds),
    base_width(other.base_width),
    base_height(other.base_height),
    wiring_start_point(other.wiring_start_point),
    wiring_pattern(other.wiring_pattern),
    submatix_row_offset(other.submatix_row_offset),
    submatix_column_offset(other.submatix_column_offset),
    submatix_width(other.submatix_width),
    submatix_height(other.submatix_height),
    submatix_reverse_rows(other.submatix_reverse_rows),
    submatix_reverse_colums(other.submatix_reverse_colums)
{}

void LedMatrix::operator=(const LedMatrix& other) {
    if (this != &other) {
        this->leds = other.leds;
        this->base_width = other.base_width;
        this->base_height = other.base_height;
        this->wiring_start_point = other.wiring_start_point;
        this->wiring_pattern = other.wiring_pattern;
        this->submatix_row_offset = other.submatix_row_offset;
        this->submatix_column_offset = other.submatix_column_offset;
        this->submatix_width = other.submatix_width;
        this->submatix_height = other.submatix_height;
        this->submatix_reverse_rows = other.submatix_reverse_rows;
        this->submatix_reverse_colums = other.submatix_reverse_colums;
    }
}


// Index operator
LedMatrix::reference LedMatrix::operator[](const uint32_t index) {
    const uint32_t subrow_index = index / this->submatix_width;
    const uint32_t subcolumn_index = index % this->submatix_width;
    const auto base_indices = this->submatrix_index_to_base_matrix_index(subrow_index, subcolumn_index);
    return this->leds[this->matrix_index_to_led_string_index(base_indices.first, base_indices.second)];
}
LedMatrix::const_reference LedMatrix::operator[](const uint32_t index) const {
    const uint32_t subrow_index = index / this->submatix_width;
    const uint32_t subcolumn_index = index % this->submatix_width;
    const auto base_indices = this->submatrix_index_to_base_matrix_index(subrow_index, subcolumn_index);
    return this->leds[this->matrix_index_to_led_string_index(base_indices.first, base_indices.second)];
}

// Index operator
LedMatrix::reference LedMatrix::operator()(const uint32_t row_index, const uint32_t column_index) {
    const auto base_indices = this->submatrix_index_to_base_matrix_index(row_index, column_index);
    return this->leds[this->matrix_index_to_led_string_index(base_indices.first, base_indices.second)];
}
LedMatrix::const_reference LedMatrix::operator()(const uint32_t row_index, const uint32_t column_index) const {
    const auto base_indices = this->submatrix_index_to_base_matrix_index(row_index, column_index);
    return this->leds[this->matrix_index_to_led_string_index(base_indices.first, base_indices.second)];
}

// Create Submatrix
LedMatrix LedMatrix::submat(const uint32_t Row_offset, const uint32_t Column_offset, 
    const uint32_t Submatix_width, const uint32_t Submatix_height, 
    const bool Reverse_row, const bool Reverse_column) const
{
    LedMatrix other = *this;
    other.submatix_row_offset = Row_offset;
    other.submatix_column_offset = Column_offset;
    other.submatix_width = Submatix_width;
    other.submatix_height = Submatix_height;
    other.submatix_reverse_rows = Reverse_row;
    other.submatix_reverse_colums = Reverse_column;
    return other;
}

// Reset to Base-Matrix
LedMatrix LedMatrix::basemat() const {

    LedMatrix other = *this;
    other.submatix_row_offset = this->submatix_row_offset;
    other.submatix_column_offset = this->submatix_column_offset;
    other.submatix_width = this->submatix_width;
    other.submatix_height = this->submatix_height;
    other.submatix_reverse_rows = this->submatix_reverse_rows;
    other.submatix_reverse_colums = this->submatix_reverse_colums;
    return other;
}