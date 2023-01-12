#include "board.hpp"

board hypothetical_board;

qpl::size board::size() const {
	return this->board.size();
}
bool board::is_whites_turn() const {
	return this->whites_turn;
}
bool board::is_blacks_turn() const {
	return !this->whites_turn;
}

void board::starting_position() {
	this->board.fill(empty_square);

	this->get(square::a8) = black_rook;
	this->get(square::b8) = black_knight;
	this->get(square::c8) = black_bishop;
	this->get(square::d8) = black_queen;
	this->get(square::e8) = black_king;
	this->get(square::f8) = black_bishop;
	this->get(square::g8) = black_knight;
	this->get(square::h8) = black_rook;

	for (qpl::size i = 0u; i < 8u; ++i) {
		this->get(i, 1) = black_pawn;
		this->get(i, 6) = white_pawn;
	}

	this->get(square::a1) = white_rook;
	this->get(square::b1) = white_knight;
	this->get(square::c1) = white_bishop;
	this->get(square::d1) = white_queen;
	this->get(square::e1) = white_king;
	this->get(square::f1) = white_bishop;
	this->get(square::g1) = white_knight;
	this->get(square::h1) = white_rook;

	this->white_king_moved = false;
	this->white_a_rook_moved = false;
	this->white_h_rook_moved = false;
	this->black_king_moved = false;
	this->black_a_rook_moved = false;
	this->black_h_rook_moved = false;
	this->pawn_double_advance = qpl::vec(qpl::size_max, qpl::size_max);
	this->whites_turn = true;
}

void board::remove_piece(qpl::vec2s pos) {
	if (is_white_king(this->get(pos))) {
		this->white_king_moved = true;
	}
	else if (is_black_king(this->get(pos))) {
		this->black_king_moved = true;
	}
	else if (is_black_rook(this->get(pos))) {
		if (pos == to_vec(square::a8)) {
			this->black_a_rook_moved = true;
		}
		if (pos == to_vec(square::h8)) {
			this->black_h_rook_moved = true;
		}
	}
	else if (is_white_rook(this->get(pos))) {
		if (pos == to_vec(square::a1)) {
			this->white_a_rook_moved = true;
		}
		if (pos == to_vec(square::h1)) {
			this->white_h_rook_moved = true;
		}
	}

	this->get(pos) = 0;
}
void board::move_piece(qpl::vec2s from, qpl::vec2s to) {
	this->pawn_double_advance = qpl::vec(qpl::size_max, qpl::size_max);
	if (is_pawn(this->get(from))) {
		if (qpl::abs(qpl::signed_cast(from.y) - qpl::signed_cast(to.y)) == 2) {
			this->pawn_double_advance = to;
		}
	}

	this->remove_piece(to);
	this->get(to) = this->get(from);
	this->remove_piece(from);
}

bool board::out_of_bounds(qpl::i32 x, qpl::i32 y) const {
	return x < 0 || y < 0 || x >= 8 || y >= 8;
}
bool board::out_of_bounds(qpl::vec2i pos) const {
	return this->out_of_bounds(pos.x, pos.y);
}

bool board::valid_move_opponent(qpl::vec2i pos, piece piece) const {
	if (this->out_of_bounds(pos)) {
		return false;
	}
	auto target = this->get(pos);
	return target && !is_same_color(target, piece);
}
bool board::valid_move_not_friend(qpl::vec2i pos, piece piece) const {
	if (this->out_of_bounds(pos)) {
		return false;
	}
	auto target = this->get(pos);
	return !target || !is_same_color(target, piece);
}
bool board::valid_move_friend(qpl::vec2i pos, piece piece) const {
	if (this->out_of_bounds(pos)) {
		return false;
	}
	auto target = this->get(pos);
	return target && is_same_color(target, piece);
}
bool board::valid_move_enemy(qpl::vec2i pos, piece piece) const {
	if (this->out_of_bounds(pos)) {
		return false;
	}
	auto target = this->get(pos);
	return target && !is_same_color(target, piece);
}
bool board::valid_move_piece(qpl::vec2i pos, piece piece) const {
	if (this->out_of_bounds(pos)) {
		return false;
	}
	auto target = this->get(pos);
	return target;
}
bool board::valid_move_empty(qpl::vec2i pos, piece piece) const {
	if (this->out_of_bounds(pos)) {
		return false;
	}
	auto target = this->get(pos);
	return !target;
}

std::vector<move> board::get_pawn_moves(qpl::vector2i pos, bool only_attacks) const {
	auto piece = this->get(pos);
	std::vector<move> result;

	qpl::isize pawn_direction = 1;
	qpl::size pawn_double_advancement_rank = 1u;
	if (is_white_pawn(piece)) {
		pawn_direction = -1;
		pawn_double_advancement_rank = 6u;
	}
	auto check = pos + qpl::vec(0, pawn_direction);
	if (!only_attacks && this->valid_move_empty(check, piece)) {
		result.push_back({ check, false });
		check = pos + qpl::vec(0, pawn_direction * 2);
		if (pos.y == pawn_double_advancement_rank && this->valid_move_empty(check, piece)) {
			result.push_back({ check, false });
		}
	}
	check = pos + qpl::vec(1, pawn_direction);
	if (this->valid_move_opponent(check, piece)) {
		result.push_back({ check, true });
	}
	check = pos + qpl::vec(-1, pawn_direction);
	if (this->valid_move_opponent(check, piece)) {
		result.push_back({ check, true });
	}
	if (this->pawn_double_advance != qpl::vec(qpl::size_max, qpl::size_max)) {
		check = pos + qpl::vec(1, 0);
		if (this->valid_move_opponent(check, piece) && check == this->pawn_double_advance) {
			result.push_back({ pos + qpl::vec(1, pawn_direction), true, false, true });
		}
		check = pos + qpl::vec(-1, 0);
		if (this->valid_move_opponent(check, piece) && check == this->pawn_double_advance) {
			result.push_back({ pos + qpl::vec(-1, pawn_direction), true, false, true });
		}
	}
	return result;
}
std::vector<move> board::get_knight_moves(qpl::vector2i pos) const {
	auto piece = this->get(pos);
	std::vector<move> result;
	constexpr auto ring = std::array{ qpl::vec(-1, -2), qpl::vec(1, -2), qpl::vec(2, -1), qpl::vec(2, 1), qpl::vec(1, 2), qpl::vec(-1, 2), qpl::vec(-2, 1), qpl::vec(-2, -1) };
	for (auto& i : ring) {
		auto check = pos + i;
		if (this->valid_move_not_friend(check, piece)) {
			result.push_back({ check, this->valid_move_enemy(check, piece) });
		}
	}
	return result;
}
std::vector<move> board::get_bishop_moves(qpl::vector2i pos) const {
	auto piece = this->get(pos);
	std::vector<move> result;
	for (auto& diagonal : qpl::vec_diagonals4) {
		for (qpl::isize i = 1; i <= 8; ++i) {
			auto check = pos + (diagonal * i);
			if (this->out_of_bounds(check) || this->valid_move_friend(check, piece)) {
				break;
			}
			if (this->valid_move_not_friend(check, piece)) {
				result.push_back({ check, this->valid_move_enemy(check, piece) });
			}
			if (this->valid_move_piece(check, piece)) {
				break;
			}
		}
	}
	return result;
}
std::vector<move> board::get_rook_moves(qpl::vector2i pos) const {
	auto piece = this->get(pos);
	std::vector<move> result;
	for (auto& cross : qpl::vec_cross4) {
		for (qpl::isize i = 1; i <= 8; ++i) {
			auto check = pos + (cross * i);
			if (this->out_of_bounds(check) || this->valid_move_friend(check, piece)) {
				break;
			}
			if (this->valid_move_not_friend(check, piece)) {
				result.push_back({ check, this->valid_move_enemy(check, piece) });
			}
			if (this->valid_move_piece(check, piece)) {
				break;
			}
		}
	}
	return result;
}
std::vector<move> board::get_queen_moves(qpl::vector2i pos) const {
	auto piece = this->get(pos);
	std::vector<move> result;
	for (auto& cross : qpl::vec_cross9) {
		for (qpl::isize i = 1; i <= 8; ++i) {
			auto check = pos + (cross * i);
			if (this->out_of_bounds(check) || this->valid_move_friend(check, piece)) {
				break;
			}
			if (this->valid_move_not_friend(check, piece)) {
				result.push_back({ check, this->valid_move_enemy(check, piece) });
			}
			if (this->valid_move_piece(check, piece)) {
				break;
			}
		}
	}
	return result;
}
std::vector<move> board::get_king_moves(qpl::vector2i pos, bool only_attacks) const {
	auto piece = this->get(pos);
	std::vector<move> result;
	for (auto& cross : qpl::vec_cross9) {
		auto check = pos + cross;
		if (!this->out_of_bounds(check) && !this->valid_move_friend(check, piece)) {
			if (this->valid_move_not_friend(check, piece)) {
				result.push_back({ check, this->valid_move_enemy(check, piece) });
			}
		}
	}
	if (this->chess_mode == chess_mode::regular && !only_attacks) {
		if (is_black(piece) && !this->black_king_moved && !this->is_black_in_check()) {
			if (!this->black_a_rook_moved) {
				if (this->empty(square::b8, square::c8, square::d8)) {
					if (!this->is_square_attacked(to_vec(square::d8), true)) {
						result.push_back({ to_vec(square::c8), false, true });
					}
				}
			}
			if (!this->black_h_rook_moved) {
				if (this->empty(square::f8, square::g8)) {
					if (!this->is_square_attacked(to_vec(square::f8), true)) {
						result.push_back({ to_vec(square::g8), false, true });
					}
				}
			}
		}
		if (is_white(piece) && !this->white_king_moved && !this->is_white_in_check()) {
			if (!this->white_a_rook_moved) {
				if (this->empty(square::b1, square::c1, square::d1)) {
					if (!this->is_square_attacked(to_vec(square::d1), false)) {
						result.push_back({ to_vec(square::c1), false, true });
					}
				}
			}
			if (!this->white_h_rook_moved) {
				if (this->empty(square::f1, square::g1)) {
					if (!this->is_square_attacked(to_vec(square::f1), false)) {
						result.push_back({ to_vec(square::g1), false, true });
					}
				}
			}
		}
	}
	return result;
}

std::vector<move> board::get_possible_moves(qpl::vec2i pos, bool only_attacks) const {
	auto piece = this->get(pos);
	if (!piece) {
		return {};
	}

	if (is_pawn(piece)) {
		return this->get_pawn_moves(pos, only_attacks);
	}
	else if (is_knight(piece)) {
		return this->get_knight_moves(pos);
	}
	else if (is_bishop(piece)) {
		return this->get_bishop_moves(pos);
	}
	else if (is_rook(piece)) {
		return this->get_rook_moves(pos);
	}
	else if (is_queen(piece)) {
		return this->get_queen_moves(pos);
	}
	else if (is_king(piece)) {
		return this->get_king_moves(pos, only_attacks);
	}

	return {};
}

std::vector<move> board::get_possible_attacks(qpl::vec2i pos) const {
	return this->get_possible_moves(pos, true);
}
std::vector<move> board::get_possible_legal_moves(qpl::vec2i pos) const {
	auto piece = this->get(pos);

	if (this->is_whites_turn() && is_black(piece)) {
		return {};
	}
	if (this->is_blacks_turn() && is_white(piece)) {
		return {};
	}

	auto white_check = this->is_whites_turn() && this->is_white_in_check();
	auto black_check = this->is_blacks_turn() && this->is_black_in_check();

	auto moves = this->get_possible_moves(pos);

	std::vector<move> legal_moves;
	for (auto& move : moves) {
		hypothetical_board = *this;

		hypothetical_board.move_piece(pos, move.position);

		bool legal = true;

		if (this->is_blacks_turn() && hypothetical_board.is_black_in_check()) {
			legal = false;
		}
		else if (this->is_whites_turn() && hypothetical_board.is_white_in_check()) {
			legal = false;
		}

		if (legal) {
			legal_moves.push_back(move);
		}

	}
	return legal_moves;
}

qpl::vec2s board::get_piece_position(piece piece) const {
	for (qpl::size i = 0u; i < this->board.size(); ++i) {
		if (this->board[i] == piece) {
			return qpl::vec(i % 8u, i / 8u);
		}
	}
	return qpl::vec(qpl::size_max, qpl::size_max);
}

bool board::is_in_check(bool white) const {
	for (qpl::size i = 0u; i < this->board.size(); ++i) {
		auto x = i % 8u;
		auto y = i / 8u;
		auto pos = qpl::vec(x, y);
		auto& piece = this->board[i];

		if (piece && !is_king(piece)) {
			if (is_color(piece, !white)) {
				auto moves = this->get_possible_attacks(pos);
				for (auto& move : moves) {
					auto target = this->get(move.position);
					if (is_color(target, white) && is_king(this->get(move.position))) {
						return true;
					}
				}
			}
		}
	}
	return false;
}
bool board::is_white_in_check() const {
	return this->is_in_check(true);
}
bool board::is_black_in_check() const {
	return this->is_in_check(false);
}
bool board::current_player_in_check() const {
	return this->is_in_check(this->whites_turn);
}

bool board::is_square_attacked(qpl::vec2s pos, bool white_color) const {
	for (qpl::size i = 0u; i < this->board.size(); ++i) {
		auto x = i % 8u;
		auto y = i / 8u;
		auto& check_piece = this->board[i];

		if (check_piece && !is_king(check_piece)) {
			if (is_color(check_piece, white_color)) {
				auto moves = this->get_possible_moves({x, y});
				for (auto& move : moves) {
					if (move.position == pos) {
						return true;
					}
				}
			}
		}
	}
	return false;
}
piece& board::get(qpl::size x, qpl::size y) {
	auto index = y * 8u + x;
	if (index > this->board.size()) {
		qpl::println("board::get(", x, ", ", y, ") : out of bounds.");
		return this->board.front();
	}
	return this->board[index];
}
const piece& board::get(qpl::size x, qpl::size y) const {
	auto index = y * 8u + x;
	if (index > this->board.size()) {
		qpl::println("board::get(", x, ", ", y, ") : out of bounds.");
		return this->board.front();
	}
	return this->board[index];
}

piece& board::get(qpl::vec2s pos) {
	return this->get(pos.x, pos.y);
}
const piece& board::get(qpl::vec2s pos) const {
	return this->get(pos.x, pos.y);
}

piece& board::get(square square) {
	return this->get(to_vec(square));
}
const piece& board::get(square square) const {
	return this->get(to_vec(square));
}