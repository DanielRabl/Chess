#pragma once
#include <qpl/qpl.hpp>
#include "square.hpp"

using piece = qpl::u8;
constexpr piece empty_square = 0u;
constexpr piece white_king = 1u;
constexpr piece white_queen = 2u;
constexpr piece white_bishop = 3u;
constexpr piece white_knight = 4u;
constexpr piece white_rook = 5u;
constexpr piece white_pawn = 6u;
constexpr piece black_king = 1u + 6u;
constexpr piece black_queen = 2u + 6u;
constexpr piece black_bishop = 3u + 6u;
constexpr piece black_knight = 4u + 6u;
constexpr piece black_rook = 5u + 6u;
constexpr piece black_pawn = 6u + 6u;

constexpr std::array<std::string, 13> piece_names = {
	"empty square",
	"white king",
	"white queen",
	"white bishop",
	"white knight",
	"white rook",
	"white pawn",
	"black king",
	"black queen ",
	"black bishop",
	"black knight",
	"black rook",
	"black pawn",
};


constexpr bool is_empty(piece piece) {
	return piece == empty_square;
}
constexpr bool is_black(piece piece) {
	return piece >= black_king && piece <= black_pawn;
}
constexpr bool is_white(piece piece) {
	return piece >= white_king && piece <= white_pawn;
}
constexpr bool is_color(piece piece, bool white_color) {
	return (white_color && is_white(piece)) || (!white_color && is_black(piece));
}
constexpr piece as_black_color(piece piece) {
	if (is_white(piece)) {
		return piece + 6u;
	}
	return piece;
}
constexpr piece as_white_color(piece piece) {
	if (is_black(piece)) {
		return piece - 6u;
	}
	return piece;
}
constexpr bool is_same_color(piece piece1, piece piece2) {
	return is_black(piece1) == is_black(piece2);
}
constexpr bool is_white_king(piece piece) {
	return piece == white_king;
}
constexpr bool is_black_king(piece piece) {
	return piece == black_king;
}
constexpr bool is_king(piece piece) {
	return is_white_king(piece) || is_black_king(piece);
}
constexpr bool is_white_queen(piece piece) {
	return piece == white_queen;
}
constexpr bool is_black_queen(piece piece) {
	return piece == black_queen;
}
constexpr bool is_queen(piece piece) {
	return is_white_queen(piece) || is_black_queen(piece);
}
constexpr bool is_white_rook(piece piece) {
	return piece == white_rook;
}
constexpr bool is_black_rook(piece piece) {
	return piece == black_rook;
}
constexpr bool is_rook(piece piece) {
	return is_white_rook(piece) || is_black_rook(piece);
}
constexpr bool is_white_bishop(piece piece) {
	return piece == white_bishop;
}
constexpr bool is_black_bishop(piece piece) {
	return piece == black_bishop;
}
constexpr bool is_bishop(piece piece) {
	return is_white_bishop(piece) || is_black_bishop(piece);
}
constexpr bool is_white_knight(piece piece) {
	return piece == white_knight;
}
constexpr bool is_black_knight(piece piece) {
	return piece == black_knight;
}
constexpr bool is_knight(piece piece) {
	return is_white_knight(piece) || is_black_knight(piece);
}
constexpr bool is_white_pawn(piece piece) {
	return piece == white_pawn;
}
constexpr bool is_black_pawn(piece piece) {
	return piece == black_pawn;
}
constexpr bool is_pawn(piece piece) {
	return is_white_pawn(piece) || is_black_pawn(piece);
}

constexpr std::string piece_name(piece piece) {
	return piece_names[piece];
}

constexpr auto piece_dimension = qpl::vec(800 / 6.0f, 133.5f);

constexpr qpl::hitbox get_piece_hitbox(piece piece) {
	qpl::u8 n = piece - 1u;
	auto x = n % 6u;
	auto y = n / 6u;
	return qpl::hitbox(qpl::vec(x, y) * piece_dimension, piece_dimension);
}
