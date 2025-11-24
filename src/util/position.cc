export module util.position;

export struct Position {
    int row;
    int col;

    constexpr bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }

    constexpr bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};