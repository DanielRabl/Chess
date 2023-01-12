#pragma once

#include <qpl/qpl.hpp>
#include "piece.hpp"
#include "square.hpp"
#include "move.hpp"

enum class chess_mode {
	regular,
	antichess
};

struct board;

extern board hypothetical_board;

struct board {
	std::array<piece, 64> board;
	bool white_king_moved = false;
	bool white_a_rook_moved = false;
	bool white_h_rook_moved = false;
	bool black_king_moved = false;
	bool black_a_rook_moved = false;
	bool black_h_rook_moved = false;
	qpl::vec2s pawn_double_advance;
	bool whites_turn = true;
	chess_mode chess_mode = chess_mode::regular;

	qpl::size size() const;
	bool is_whites_turn() const;
	bool is_blacks_turn() const;


	void starting_position();

	void remove_piece(qpl::vec2s pos);
	void move_piece(qpl::vec2s from, qpl::vec2s to);

	bool out_of_bounds(qpl::i32 x, qpl::i32 y) const;
	bool out_of_bounds(qpl::vec2i pos) const;

	bool valid_move_opponent(qpl::vec2i pos, piece piece) const;
	bool valid_move_not_friend(qpl::vec2i pos, piece piece) const;
	bool valid_move_friend(qpl::vec2i pos, piece piece) const;
	bool valid_move_enemy(qpl::vec2i pos, piece piece) const;
	bool valid_move_piece(qpl::vec2i pos, piece piece) const;
	bool valid_move_empty(qpl::vec2i pos, piece piece) const;


	std::vector<move> get_pawn_moves(qpl::vector2i pos, bool only_attacks = false) const;
	std::vector<move> get_knight_moves(qpl::vector2i pos) const;
	std::vector<move> get_bishop_moves(qpl::vector2i pos) const;
	std::vector<move> get_rook_moves(qpl::vector2i pos) const;
	std::vector<move> get_queen_moves(qpl::vector2i pos) const;
	std::vector<move> get_king_moves(qpl::vector2i pos, bool only_attacks = false) const;

	std::vector<move> get_possible_moves(qpl::vec2i pos, bool only_attacks = false) const;
	std::vector<move> get_possible_attacks(qpl::vec2i pos) const;
	std::vector<move> get_possible_legal_moves(qpl::vec2i pos) const;

	qpl::vec2s get_piece_position(piece piece) const;

	bool is_in_check(bool white) const;
	bool is_white_in_check() const;
	bool is_black_in_check() const;
	bool current_player_in_check() const;

	bool is_square_attacked(qpl::vec2s pos, bool white_color) const;

	piece& get(qpl::size x, qpl::size y);
	const piece& get(qpl::size x, qpl::size y) const;

	piece& get(qpl::vec2s pos);
	const piece& get(qpl::vec2s pos) const;

	piece& get(square square);
	const piece& get(square square) const;

	template<typename... sq>
	bool empty(sq... square) const {
		return ((is_empty(this->get(square))) && ...);
	}
};
