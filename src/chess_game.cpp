#pragma once
#include "chess_game.hpp"

void chess_game::init() {
	qsf::get_texture("light").setSmooth(true);
	this->light_sprite.set_texture(qsf::get_texture("light"));
	this->king_check_sprite.set_texture(qsf::get_texture("light"));
	this->king_check_sprite.set_color(qpl::rgb::red().with_alpha(255));

	this->texture.loadFromFile("resources/pieces.png");
	this->texture.setSmooth(true);
	this->board.starting_position();
	this->light_hover_animation.set_duration(0.2);

	for (auto& i : this->promotion_pieces) {
		i.sprite.set_texture(this->texture);
	}
}

void chess_game::reset_board() {
	this->board.starting_position();
	this->promotion_white = this->promotion_black = false;
	this->possible_moves.clear();
	this->update_board();
	this->king_check = false;
}
void chess_game::update_board() {
	this->pieces.clear();
	qpl::size ctr = 0u;
	for (auto& i : this->board.board) {
		if (i) {
			auto x = ctr % 8u;
			auto y = ctr / 8u;
			auto scale = this->square_width / piece_dimension.x;

			this->pieces.push_back({});
			this->pieces.back().board_position = qpl::vec(x, y);
			this->pieces.back().sprite.set_texture(this->texture);
			this->pieces.back().sprite.set_texture_rect(get_piece_hitbox(i));
			this->pieces.back().sprite.set_scale(scale);
			this->pieces.back().update_position(this->square_width);

		}
		++ctr;
	}
}
void chess_game::highlight_square(qpl::vec2s pos, bool highlight) {
	auto index = pos.y * 8u + pos.x;
	auto color = (pos.x + pos.y) % 2 ? this->dark_square_color : this->light_square_color;
	if (highlight) {
		color.darken(0.25);
	}

	for (qpl::size j = 0u; j < qpl::vec_square4.size(); ++j) {
		this->va[index * 4 + j].color = color;
	}
}
void chess_game::resize(qpl::vec2f dimension) {
	this->va.resize(this->board.size() * 4u);
	this->va.set_primitive_type(qsf::primitive_type::quads);

	this->square_width = dimension.min() / 8.0f;
	for (qpl::size i = 0u; i < this->board.size(); ++i) {
		auto x = i % 8u;
		auto y = i / 8u;

		auto color = (x + y) % 2 ? this->dark_square_color : this->light_square_color;

		for (qpl::size j = 0u; j < qpl::vec_square4.size(); ++j) {
			this->va[i * 4 + j].position = (qpl::vec(x, y) + qpl::vec_square4[j]) * this->square_width;
			this->va[i * 4 + j].color = color;
		}
	}


	auto scale = (this->square_width / piece_dimension.x) * 1.5f;
	auto left = (dimension - qpl::vec((this->promotion_pieces.size()) * piece_dimension.x * scale, piece_dimension.y)) / 2;


	qpl::hitbox hitbox;
	hitbox.set_position(left);
	hitbox.set_dimension(piece_dimension * scale * qpl::vec(this->promotion_pieces.size(), 1));

	this->promotion_background.set_hitbox(hitbox.increased(this->square_width / 3));
	this->promotion_background.set_slope_dimension(this->square_width * 0.75f);

	for (qpl::size i = 0u; i < this->promotion_pieces.size(); ++i) {
		this->promotion_pieces[i].sprite.set_scale(scale);
		this->promotion_pieces[i].sprite.set_position(left + qpl::vec(piece_dimension.x * scale * i, 0));
	}
	this->light_sprite.set_scale((this->square_width / piece_dimension.x) * 0.65f);
	this->light_sprite.set_color(this->light_sprite_color.with_alpha(0));

	this->king_check_sprite.set_scale((this->square_width / piece_dimension.x) * 0.35f);
}

void chess_game::handle_en_pessant(qpl::vec2s to) {
	if (this->selected_move.en_pessant) {
		this->selected_move.en_pessant = false;

		qpl::i32 pawn_movement = 1;
		if (is_black(this->board.get(to))) {
			pawn_movement = -1;
		}

		auto target = qpl::vec2i(to) + qpl::vec(0, pawn_movement);
		this->board.remove_piece(target);

		auto erase = std::find_if(this->pieces.begin(), this->pieces.end(), [&](auto a) {
			return a.board_position == target;
			});
		if (erase != this->pieces.cend()) {
			this->pieces.erase(erase);
			this->find_dragging_piece();
		}
	}
}
void chess_game::handle_castling(qpl::vec2s to) {
	if (this->selected_move.castle) {
		this->selected_move.castle = false;
		if (to == to_vec(square::g1)) {
			this->move_piece(to_vec(square::h1), to_vec(square::f1));
		}
		else if (to == to_vec(square::c1)) {
			this->move_piece(to_vec(square::a1), to_vec(square::d1));
		}
		else if (to == to_vec(square::g8)) {
			this->move_piece(to_vec(square::h8), to_vec(square::f8));
		}
		else if (to == to_vec(square::c8)) {
			this->move_piece(to_vec(square::a8), to_vec(square::d8));
		}
	}
}
void chess_game::handle_promotion(qpl::vec2s to) {

	this->promotion_white = is_white_pawn(this->board.get(to)) && to.y == 0u;
	this->promotion_black = is_black_pawn(this->board.get(to)) && to.y == 7u;

	if (this->promotion_white || this->promotion_black) {
		this->selected_position = to;
		this->possible_moves.clear();
	}

	if (this->promotion_white) {
		this->promotion_pieces[0].sprite.set_texture_rect(get_piece_hitbox(white_queen));
		this->promotion_pieces[1].sprite.set_texture_rect(get_piece_hitbox(white_rook));
		this->promotion_pieces[2].sprite.set_texture_rect(get_piece_hitbox(white_bishop));
		this->promotion_pieces[3].sprite.set_texture_rect(get_piece_hitbox(white_knight));
		this->promotion_background.set_color(qpl::rgb::black().with_alpha(200));
	}
	if (this->promotion_black) {
		this->promotion_pieces[0].sprite.set_texture_rect(get_piece_hitbox(black_queen));
		this->promotion_pieces[1].sprite.set_texture_rect(get_piece_hitbox(black_rook));
		this->promotion_pieces[2].sprite.set_texture_rect(get_piece_hitbox(black_bishop));
		this->promotion_pieces[3].sprite.set_texture_rect(get_piece_hitbox(black_knight));
		this->promotion_background.set_color(qpl::rgb::white().with_alpha(200));
	}
}
void chess_game::move_piece(qpl::vec2s from, qpl::vec2s to) {
	auto before = this->get_dragging_piece().board_position;

	std::vector<piece_graphic>::iterator erase = this->pieces.end();
	auto next_piece = this->board.get(to);
	if (next_piece) {
		erase = std::find_if(this->pieces.begin(), this->pieces.end(), [&](auto a) {
			return a.board_position == to;
			});
	}
	this->board.move_piece(from, to);
	this->get_piece(from).board_position = to;

	if (erase != this->pieces.cend()) {
		this->pieces.erase(erase);
		this->find_dragging_piece();
	}

	this->setup_new_move_animation(to);
	this->handle_castling(to);
	this->handle_en_pessant(to);
	this->handle_promotion(to);

}
void chess_game::promote_piece(qpl::vec2s position, piece to) {
	this->board.get(position) = to;
	this->get_piece(position).sprite.set_texture_rect(get_piece_hitbox(to));
}

void chess_game::next_turn() {
	this->board.whites_turn = !this->board.whites_turn;
}

void chess_game::setup_new_move_animation(qpl::vec2s to) {

	this->move_animations.insert(std::pair(to, move_animation{}));
	auto& animation = this->move_animations[to];

	animation.animation.reset_and_start();
	animation.a = this->get_piece(to).sprite.get_center();
	animation.b = (to + 0.5) * this->square_width;

	if (this->valid_drop) {
		animation.animation.set_duration(0.1);
	}
	else {
		animation.animation.set_duration(0.2);
	}
}
void chess_game::update_move_animation(const qsf::event_info& event) {
	std::vector<qpl::vec2s> removes;
	for (auto& move_animation : this->move_animations) {
		auto& animation = move_animation.second.animation;
		auto a = move_animation.second.a;
		auto b = move_animation.second.b;

		animation.update(event);
		if (animation.is_running()) {
			auto p = animation.get_curve_progress();
			auto value = qpl::linear_interpolation(a, b, p);
			this->get_piece(move_animation.first).sprite.set_center(value);
		}
		if (animation.just_finished()) {
			removes.push_back(move_animation.first);
		}
	}
	for (auto& i : removes) {
		this->move_animations.erase(i);
	}
}
void chess_game::find_dragging_piece() {
	for (qpl::size i = 0u; i < this->pieces.size(); ++i) {
		if (this->pieces[i].board_position == this->mouse_board_position) {
			this->dragging_index = i;
		}
	}
}

piece_graphic& chess_game::get_piece(qpl::vec2s pos) {
	for (auto& i : this->pieces) {
		if (i.board_position == pos) {
			return i;
		}
	}
	qpl::println("get_piece(", pos, ") : couldn't find the piece on board.");
}
const piece_graphic& chess_game::get_piece(qpl::vec2s pos) const {
	for (auto& i : this->pieces) {
		if (i.board_position == pos) {
			return i;
		}
	}
	qpl::println("get_piece(", pos, ") : couldn't find the piece on board.");
}

piece_graphic& chess_game::get_dragging_piece() {
	return this->pieces[this->dragging_index];
}
const piece_graphic& chess_game::get_dragging_piece() const {
	return this->pieces[this->dragging_index];
}

void chess_game::update_hovering_index(const qsf::event_info& event) {
	this->hovering_index = 0u;
	this->hovering_over_piece = false;
	this->hovering_outside = this->board.out_of_bounds(this->mouse_board_position);
	if (!this->hovering_outside) {
		auto piece = this->board.get(this->mouse_board_position);
		if (piece) {
			this->hovering_over_piece = true;
			if (event.left_mouse_clicked()) {
				for (qpl::size i = 0u; i < this->pieces.size(); ++i) {
					if (this->pieces[i].board_position == this->mouse_board_position) {
						this->hovering_index = i;
					}
				}
			}
		}
	}
}
void chess_game::update_dragging(const qsf::event_info& event) {
	if (event.left_mouse_clicked() && this->hovering_over_piece) {
		bool found_in_possible_moves = false;
		for (auto& i : this->possible_moves) {
			if (i.position == this->mouse_board_position) {
				found_in_possible_moves = true;
				break;
			}
		}

		if (!found_in_possible_moves) {
			this->dragging = true;
			this->dragging_index = this->hovering_index;
			this->selected_position = this->mouse_board_position;
			this->possible_moves = this->board.get_possible_legal_moves(this->mouse_board_position);
		}
	}
}
void chess_game::update_check_possible_moves(const qsf::event_info& event) {
	if (!this->hovering_outside) {
		for (auto& move : this->possible_moves) {
			auto center = (move.position + 0.5f) * this->square_width;
			qpl::hitbox hitbox;
			hitbox.set_dimension(qpl::vec(this->square_width, this->square_width) * 0.9);
			hitbox.set_center(center);
			if (hitbox.contains(event.mouse_position())) {
				if (event.left_mouse_clicked()) {
					this->clicked_on_possible_move = true;
				}
				this->valid_hover = true;
				this->selected_move = move;
			}
		}
	}
}
void chess_game::update_hightlighting() {
	if (!this->hovering_outside) {
		if (this->hover_square != this->mouse_board_position) {
			this->highlight_square(this->hover_square, false);
		}
		this->hover_square = this->mouse_board_position;
		this->highlight_square(this->hover_square, this->valid_hover);
	}
}
void chess_game::update_piece_placement() {
	if (this->valid_placement) {
		if (this->valid_drop || this->placed_selected) {
			this->move_piece(this->selected_position, this->mouse_board_position);
			this->next_turn();

			this->king_check = this->board.current_player_in_check();
			if (this->king_check) {
				piece piece = white_king;
				if (this->board.is_blacks_turn()) {
					piece = black_king;
				}
				auto pos = this->board.get_piece_position(piece);
				this->king_check_sprite.set_center((pos + 0.5) * this->square_width);
			}


			if (this->valid_drop) {
				this->get_dragging_piece().update_position(this->square_width);
			}
		}
		else {
			this->get_dragging_piece().update_position(this->square_width);
		}
	}
}

void chess_game::update_piece_promotion(const qsf::event_info& event) {
	this->promotion_hover_index = qpl::size_max;
	for (qpl::size i = 0u; i < this->promotion_pieces.size(); ++i) {
		auto& piece = this->promotion_pieces[i];
		if (piece.sprite.get_hitbox().contains(event.mouse_position())) {
			this->light_hover_animation.go_forwards();

			this->promotion_hover_index = i;
			this->light_sprite.set_center(piece.sprite.get_center());
		}
	}
	if (this->promotion_hover_index == qpl::size_max) {
		this->light_hover_animation.go_backwards();
	}
	else if (event.left_mouse_clicked()) {
		piece piece;
		switch (this->promotion_hover_index) {
		case 0u: piece = white_queen; break;
		case 1u: piece = white_rook; break;
		case 2u: piece = white_bishop; break;
		case 3u: piece = white_knight; break;
		}
		if (this->promotion_black) {
			piece = as_black_color(piece);
		}

		this->promote_piece(this->selected_position, piece);
		this->promotion_white = this->promotion_black = false;
	}

	this->light_hover_animation.update(event);
	if (this->light_hover_animation.is_running()) {
		auto p = this->light_hover_animation.get_curve_progress();
		auto color = this->light_sprite_color;
		color.a = qpl::u8_cast(color.a * p);
		this->light_sprite.set_color(color);
	}

}
void chess_game::update(const qsf::event_info& event) {

	if (event.key_pressed(sf::Keyboard::Space)) {
		this->reset_board();
	}
	this->update_move_animation(event);

	if (this->promotion_white || this->promotion_black) {
		this->update_piece_promotion(event);
		return;
	}


	this->mouse_board_position = event.mouse_position() / this->square_width;
	bool just_released = event.left_mouse_released();

	this->update_hovering_index(event);
	this->update_dragging(event);

	this->valid_dragging = dragging && this->dragging_index != qpl::size_max;
	this->valid_placement = just_released && this->dragging_index != qpl::size_max;
	if (this->valid_dragging) {
		this->get_dragging_piece().sprite.set_center(event.mouse_position());
	}

	this->valid_hover = false;
	this->valid_drop = false;
	this->placed_selected = false;

	if (event.left_mouse_clicked()) {
		this->clicked_on_possible_move = false;
	}
	this->update_check_possible_moves(event);
	if (this->valid_hover && this->valid_placement && !this->clicked_on_possible_move) {
		this->valid_drop = true;
	}
	if (this->valid_hover && just_released && this->clicked_on_possible_move) {
		this->placed_selected = true;
	}

	this->update_hightlighting();
	this->update_piece_placement();

	if (just_released) {
		this->dragging = false;
		this->highlight_square(this->hover_square, false);

		if (this->selected_position != this->mouse_board_position) {
			this->possible_moves.clear();
		}
	}
	if (this->hovering_over_piece && !this->dragging) {
		qpl::winsys::set_cursor_hand();
	}
	else {
		qpl::winsys::set_cursor_normal();
	}
}

void chess_game::draw(qsf::draw_object& draw) const {
	draw.draw(this->va);

	if (this->king_check) {
		draw.draw(this->king_check_sprite);
	}

	for (auto& i : this->possible_moves) {
		if (i.capture) {
			auto square_size = qpl::vec(this->square_width, this->square_width);

			auto index = i.position.y * 8u + i.position.x;
			auto color = this->va[index * 4].color;

			qsf::rectangle square;
			square.set_dimension(square_size);
			square.set_color(qpl::rgb(111, 93, 64));
			square.set_center((i.position + 0.5f) * this->square_width);
			draw.draw(square);

			qsf::smooth_rectangle rect;
			rect.set_color(color);
			rect.set_slope_dimension(square_size / 3.0f);
			rect.set_dimension(square_size);
			rect.set_center((i.position + 0.5f) * this->square_width);
			draw.draw(rect);
		}
		else {
			qsf::circle circle;
			circle.set_radius(this->square_width / 4.0f);
			circle.set_color(qpl::rgb::black().with_alpha(50));
			circle.set_center((i.position + 0.5f) * this->square_width);
			draw.draw(circle);
		}
	}

	for (qpl::size i = 0u; i < this->pieces.size(); ++i) {
		if (i != this->dragging_index) {
			draw.draw(this->pieces[i]);
		}
	}
	if (this->dragging_index != qpl::size_max) {
		draw.draw(this->get_dragging_piece());
	}

	if (this->promotion_black || this->promotion_white) {
		draw.draw(this->promotion_background);
		draw.draw(this->light_sprite);
		draw.draw(this->promotion_pieces);
	}
}