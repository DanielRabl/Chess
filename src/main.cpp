#include <qpl/qpl.hpp>


using piece = qpl::u8;
constexpr piece white_king	 = 1u;
constexpr piece white_queen	 = 2u;
constexpr piece white_bishop = 3u;
constexpr piece white_knight = 4u;
constexpr piece white_rook	 = 5u;
constexpr piece white_pawn	 = 6u;
constexpr piece black_king	 = 1u + 6u;
constexpr piece black_queen	 = 2u + 6u;
constexpr piece black_bishop = 3u + 6u;
constexpr piece black_knight = 4u + 6u;
constexpr piece black_rook	 = 5u + 6u;
constexpr piece black_pawn	 = 6u + 6u;

constexpr bool is_black(piece piece) {
	return piece >= black_king && piece <= black_pawn;
}
constexpr bool is_white(piece piece) {
	return piece >= white_king && piece <= white_pawn;
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

constexpr qpl::hitbox get_piece_hitbox(piece piece) {
	qpl::u8 n = piece - 1u;
	constexpr auto dim = qpl::vec(800 / 6.0f, 133.5f);
	auto x = n % 6u;
	auto y = n / 6u;
	return qpl::hitbox(qpl::vec(x, y) * dim, dim);
}

struct board {
	std::array<piece, 64> board;

	void starting_position() {
		this->get(0, 0) = black_rook;
		this->get(1, 0) = black_knight;
		this->get(2, 0) = black_bishop;
		this->get(3, 0) = black_queen;
		this->get(4, 0) = black_king;
		this->get(5, 0) = black_bishop;
		this->get(6, 0) = black_knight;
		this->get(7, 0) = black_rook;

		for (qpl::size i = 0u; i < 8u; ++i) {
			this->get(i, 1) = black_pawn;
			this->get(i, 6) = white_pawn;
		}

		this->get(0, 7) = white_rook;
		this->get(1, 7) = white_knight;
		this->get(2, 7) = white_bishop;
		this->get(3, 7) = white_queen;
		this->get(4, 7) = white_king;
		this->get(5, 7) = white_bishop;
		this->get(6, 7) = white_knight;
		this->get(7, 7) = white_rook;
	}

	piece& get(qpl::size x, qpl::size y) {
		auto index = y * 8u + x;
		if (index > this->board.size()) {
			qpl::println("board::get(", x, ", ", y, ") : out of bounds.");
			return this->board.front();
		}
		return this->board[index];
	}
	const piece& get(qpl::size x, qpl::size y) const {
		auto index = y * 8u + x;
		if (index > this->board.size()) {
			qpl::println("board::get(", x, ", ", y, ") : out of bounds.");
			return this->board.front();
		}
		return this->board[index];
	}
};

struct piece_graphic {
	qsf::sprite sprite;
	qpl::vec2s board_position;

	void draw(qsf::draw_object& draw) const {
		draw.draw(this->sprite);
	}
	void set_position(qpl::f32 square_width) {
		this->sprite.set_center((this->board_position + 0.5f) * square_width);
	}
};


struct board_graphic {
	qsf::vertex_array va;
	board board;
	sf::Texture texture;
	std::vector<piece_graphic> pieces;
	qpl::rgb light_square_color = qpl::rgb(240, 217, 181);
	qpl::rgb dark_square_color = qpl::rgb(181, 136, 99);
	qpl::f32 square_width;
	qpl::size dragging_index = 0u;
	qpl::vec2f dragging_start_position = qpl::vec(0, 0);
	qpl::vec2s hover_square;
	bool dragging = false;
	bool clicked_on_possible_move = false;
	std::vector<qpl::vec2s> possible_moves;
	qpl::animation selected_animation;
	
	qpl::vec2f animation_pos_a;
	qpl::vec2f animation_pos_b;

	void init() {
		this->selected_animation.set_duration(0.2);
		this->texture.loadFromFile("resources/pieces.png");
		this->texture.setSmooth(true);
		this->board.starting_position();
	}

	bool out_of_bounds(qpl::i32 x, qpl::i32 y) const {
		return x < 0 || y < 0 || x >= 8 || y >= 8;
	}
	bool out_of_bounds(qpl::vec2i pos) const {
		return this->out_of_bounds(pos.x, pos.y);
	}

	bool valid_move_opponent(qpl::vec2i pos, piece piece) {
		if (this->out_of_bounds(pos.x, pos.y)) {
			return false;
		}
		auto target = this->board.get(pos.x, pos.y);
		return target && !is_same_color(target, piece);
	}
	bool valid_move_not_friend(qpl::vec2i pos, piece piece) {
		if (this->out_of_bounds(pos.x, pos.y)) {
			return false;
		}
		auto target = this->board.get(pos.x, pos.y);
		return !target || !is_same_color(target, piece);
	}
	bool valid_move_friend(qpl::vec2i pos, piece piece) {
		if (this->out_of_bounds(pos.x, pos.y)) {
			return false;
		}
		auto target = this->board.get(pos.x, pos.y);
		return target && is_same_color(target, piece);
	}
	bool valid_move_piece(qpl::vec2i pos, piece piece) {
		if (this->out_of_bounds(pos.x, pos.y)) {
			return false;
		}
		auto target = this->board.get(pos.x, pos.y);
		return target;
	}

	bool valid_move_empty(qpl::vec2i pos, piece piece) {
		if (this->out_of_bounds(pos.x, pos.y)) {
			return false;
		}
		auto target = this->board.get(pos.x, pos.y);
		return !target;
	}

	std::vector<qpl::vec2s> get_possible_moves(qpl::vec2i pos) {
		auto piece = this->board.get(pos.x, pos.y);
		if (!piece) {
			return {};
		}

		std::vector<qpl::vec2s> result;
		if (is_black_pawn(piece)) {
			auto check = pos + qpl::vec(0, 1);
			if (this->valid_move_empty(check, piece)) {
				result.push_back(check);
				check = pos + qpl::vec(0, 2);
				if (pos.y == 1u && this->valid_move_empty(check, piece)) {
					result.push_back(check);
				}
			}
			check = pos + qpl::vec(1, 1);
			if (this->valid_move_opponent(check, piece)) {
				result.push_back(check);
			}
			check = pos + qpl::vec(-1, 1);
			if (this->valid_move_opponent(check, piece)) {
				result.push_back(check);
			}
		}
		else if (is_white_pawn(piece)) {
			auto check = pos - qpl::vec(0, 1);
			if (this->valid_move_empty(check, piece)) {
				result.push_back(check);
				check = pos - qpl::vec(0, 2);
				if (pos.y == 6u && this->valid_move_empty(check, piece)) {
					result.push_back(check);
				}
			}
			check = pos + qpl::vec(1, -1);
			if (this->valid_move_opponent(check, piece)) {
				result.push_back(check);
			}
			check = pos + qpl::vec(-1, -1);
			if (this->valid_move_opponent(check, piece)) {
				result.push_back(check);
			}
		}
		else if (is_knight(piece)) {
			constexpr auto ring = std::array{ qpl::vec(-1, -2), qpl::vec(1, -2), qpl::vec(2, -1), qpl::vec(2, 1), qpl::vec(1, 2), qpl::vec(-1, 2), qpl::vec(-2, 1), qpl::vec(-2, -1) };
			for (auto& i : ring) {
				auto check = pos + i;
				if (this->valid_move_not_friend(check, piece)) {
					result.push_back(check);
				}
			}
		}
		else if (is_bishop(piece)) {
			constexpr auto diagonals = std::array{ qpl::vec(-1, -1), qpl::vec(1, -1), qpl::vec(1, 1), qpl::vec(-1, 1) };
			for (auto& diagonal : diagonals) {
				for (qpl::isize i = 1; i <= 8; ++i) {
					auto check = pos + (diagonal * i);
					if (this->out_of_bounds(check) || this->valid_move_friend(check, piece)) {
						break;
					}
					if (this->valid_move_not_friend(check, piece)) {
						result.push_back(check);
					}
					if (this->valid_move_piece(check, piece)) {
						break;
					}
				}
			}
		}
		else if (is_rook(piece)) {
			for (auto& cross : qpl::vec_cross4()) {
				for (qpl::isize i = 1; i <= 8; ++i) {
					auto check = pos + (cross * i);
					if (this->out_of_bounds(check) || this->valid_move_friend(check, piece)) {
						break;
					}
					if (this->valid_move_not_friend(check, piece)) {
						result.push_back(check);
					}
					if (this->valid_move_piece(check, piece)) {
						break;
					}
				}
			}
		}
		else if (is_queen(piece)) {
			for (auto& cross : qpl::vec_cross9()) {
				for (qpl::isize i = 1; i <= 8; ++i) {
					auto check = pos + (cross * i);
					if (this->out_of_bounds(check) || this->valid_move_friend(check, piece)) {
						break;
					}
					if (this->valid_move_not_friend(check, piece)) {
						result.push_back(check);
					}
					if (this->valid_move_piece(check, piece)) {
						break;
					}
				}
			}
		}
		else if (is_king(piece)) {
			for (auto& cross : qpl::vec_cross9()) {
				auto check = pos + cross;
				if (!this->out_of_bounds(check) && !this->valid_move_friend(check, piece)) {
					if (this->valid_move_not_friend(check, piece)) {
						result.push_back(check);
					}
				}
			}
		}
		
		return result;
	}

	void update_board() {
		this->pieces.clear();
		qpl::size ctr = 0u;
		for (auto& i : this->board.board) {
			if (i) {
				auto x = ctr % 8u;
				auto y = ctr / 8u;
				auto scale = this->square_width / (133.3f);

				this->pieces.push_back({});
				this->pieces.back().board_position = qpl::vec(x, y);
				this->pieces.back().sprite.set_texture(this->texture);
				this->pieces.back().sprite.set_texture_rect(get_piece_hitbox(i));
				this->pieces.back().sprite.set_scale(scale);
				this->pieces.back().set_position(this->square_width);

			}
			++ctr;
		}
	}
	void highlight_square(qpl::vec2s pos, bool highlight) {
		auto index = pos.y * 8u + pos.x;
		auto color = (pos.x + pos.y) % 2 ? this->dark_square_color : this->light_square_color;
		if (highlight) {
			color.darken(0.25);
		}

		auto square4 = qpl::vec_square4();
		for (qpl::size j = 0u; j < square4.size(); ++j) {
			this->va[index * 4 + j].color = color;
		}
	}
	void resize(qpl::vec2f dimension) {
		this->va.resize(64 * 4u);
		this->va.set_primitive_type(qsf::primitive_type::quads);
		
		this->square_width = dimension.min() / 8.0f;
		for (qpl::size i = 0u; i < 64u; ++i) {
			auto x = i % 8u;
			auto y = i / 8u;

			auto color = (x + y) % 2 ? this->dark_square_color : this->light_square_color;

			auto square4 = qpl::vec_square4();
			for (qpl::size j = 0u; j < square4.size(); ++j) {
				this->va[i * 4 + j].position = (qpl::vec(x, y) + square4[j]) * this->square_width;
				this->va[i * 4 + j].color = color;
			}
		}
	}
	void move_piece(qpl::vec2i pos) {
		auto before = this->pieces[this->dragging_index].board_position;
		auto piece_before = this->board.get(before.x, before.y);
		this->board.get(before.x, before.y) = 0;

		std::vector<piece_graphic>::iterator erase = this->pieces.end();
		auto next_piece = this->board.get(pos.x, pos.y);
		if (next_piece) {
			erase = std::find_if(this->pieces.begin(), this->pieces.end(), [&](auto a) {
				return a.board_position == pos;
				});
		}
		this->board.get(pos.x, pos.y) = piece_before;

		this->pieces[this->dragging_index].board_position = pos;
		if (erase != this->pieces.cend()) {
			if (std::distance(this->pieces.begin(), erase) <= this->dragging_index) {
				--this->dragging_index;
			}
			this->pieces.erase(erase);
		}
	}
	
	void update(const qsf::event_info& event) {
		this->selected_animation.update(event);
		if (this->selected_animation.is_running()) {
			auto p = this->selected_animation.get_curve_progress();
			auto value = qpl::linear_interpolation(this->animation_pos_a, this->animation_pos_b, p);
			this->pieces[this->dragging_index].sprite.set_center(value);
		}

		qpl::vec2i pos = event.mouse_position() / this->square_width;
		bool just_released = event.left_mouse_released();

		qpl::size hovering_index = 0u;
		bool hovering_over_piece = false;
		bool outside = this->out_of_bounds(pos.x, pos.y);
		if (!outside) {
			auto piece = this->board.get(pos.x, pos.y);
			if (piece) {
				hovering_over_piece = true;
				if (event.left_mouse_clicked()) {
					for (qpl::size i = 0u; i < this->pieces.size(); ++i) {
						if (this->pieces[i].board_position == pos) {
							hovering_index = i;
						}
					}
				}
			}
		}

		if (event.left_mouse_clicked() && hovering_over_piece) {
			if (this->selected_animation.is_running()) {
				auto center = (this->animation_pos_b + 0.5) * this->square_width;
				this->pieces[this->dragging_index].sprite.set_center(center);
			}

			bool found_in_possible_moves = std::ranges::find(this->possible_moves, qpl::vec2s(pos)) != this->possible_moves.cend();

			if (!found_in_possible_moves) {
				this->dragging = true;
				this->dragging_start_position = event.mouse_position();
				this->dragging_index = hovering_index;
				this->possible_moves = this->get_possible_moves(pos);
			}
		}

		bool valid_dragging = dragging && this->dragging_index != qpl::size_max;
		bool valid_release = just_released && this->dragging_index != qpl::size_max;
		if (valid_dragging) {
			this->pieces[this->dragging_index].sprite.set_center(event.mouse_position());
		}

		bool valid_hover = false;
		bool valid_drop = false;
		bool dropped_selected = false;
		if (event.left_mouse_clicked()) {
			this->clicked_on_possible_move = false;
		}
		if (!outside) {
			for (auto& i : this->possible_moves) {
				auto center = (i + 0.5f) * this->square_width;
				qpl::hitbox hitbox;
				hitbox.set_dimension(qpl::vec(this->square_width, this->square_width) * 0.9);
				hitbox.set_center(center);
				if (hitbox.contains(event.mouse_position())) {
					if (event.left_mouse_clicked()) {
						this->clicked_on_possible_move = true;
					}
					valid_hover = true;
				}
			}
		}
		if (valid_hover && valid_release && !this->clicked_on_possible_move) {
			valid_drop = true;
		}
		if (valid_hover && just_released && this->clicked_on_possible_move) {
			dropped_selected = true;
			this->selected_animation.reset_and_start();
			this->animation_pos_a = this->pieces[this->dragging_index].sprite.get_center();
			this->animation_pos_b = (pos + 0.5) * this->square_width;

			if (valid_drop) {
				this->selected_animation.set_duration(0.05);
			}
			else {
				this->selected_animation.set_duration(0.2);
			}
		}

		if (!outside) {
			if (this->hover_square != pos) {
				this->highlight_square(this->hover_square, false);
			}
			this->hover_square = pos;
			this->highlight_square(this->hover_square, valid_hover);
		}

		if (valid_release) {
			if (valid_drop || dropped_selected) {
				this->move_piece(pos);
				if (valid_drop) {
					this->pieces[this->dragging_index].set_position(this->square_width);
				}
			}
			else {
				this->pieces[this->dragging_index].set_position(this->square_width);
			}
		}

		if (just_released) {
			this->dragging = false;
			this->highlight_square(this->hover_square, false);

			qpl::vec2s drag_pos = this->dragging_start_position / this->square_width;
			if (drag_pos != pos) {
				this->possible_moves.clear();
			}
		}
		if (hovering_over_piece) {
			qpl::winsys::set_cursor_hand();
		}
		else {
			qpl::winsys::set_cursor_normal();
		}
	}
	void draw(qsf::draw_object& draw) const {
		draw.draw(this->va);
		for (auto& i : this->possible_moves) {
			qsf::circle circle;
			circle.set_radius(this->square_width / 4.0f);
			circle.set_color(qpl::rgb::black().with_alpha(50));
			circle.set_center((i + 0.5) * this->square_width);
			draw.draw(circle);
		}

		for (qpl::size i = 0u; i < this->pieces.size(); ++i) {
			if (i != this->dragging_index) {
				draw.draw(this->pieces[i]);
			}
		}
		if (this->dragging_index != qpl::size_max) {
			draw.draw(this->pieces[this->dragging_index]);
		}

	}
};

struct main_state : qsf::base_state {
	void init() override {
		this->board_graphic.init();

		this->call_on_resize();
	}
	void call_on_resize() override {
		this->board_graphic.resize(this->dimension());
		this->board_graphic.update_board();
	}
	void updating() override {
		this->update(this->board_graphic);
	}
	void drawing() override {
		this->draw(this->board_graphic);
	}

	board_graphic board_graphic;
};

int main() try {
	qsf::framework framework;
	framework.set_title("QPL");
	framework.set_dimension({ 1400u, 950u });

	framework.add_state<main_state>();
	framework.game_loop();
}
catch (std::exception& any) {
	qpl::println("caught exception:\n", any.what());
	qpl::system_pause();
}