#pragma once

#include "board.hpp"

struct piece_graphic {
	qsf::sprite sprite;
	qpl::vec2s board_position;

	void draw(qsf::draw_object& draw) const {
		draw.draw(this->sprite);
	}
	void update_position(qpl::f32 square_width) {
		this->sprite.set_center((this->board_position + 0.5f) * square_width);
	}
};

struct move_animation {
	move_animation() {
		this->animation.set_duration(0.2);
	}
	qpl::animation animation;
	qpl::vec2f a;
	qpl::vec2f b;
};

struct chess_game {
	board board;

	std::unordered_map<qpl::vec2s, move_animation> move_animations;

	qpl::rgb light_square_color = qpl::rgb(240, 217, 181);
	qpl::rgb dark_square_color = qpl::rgb(181, 136, 99);
	qpl::vec2s hover_square;
	qpl::vec2s mouse_board_position;
	qpl::vec2s selected_position;
	qpl::size hovering_index = 0u;
	qpl::size dragging_index = 0u;
	qpl::f32 square_width;

	move selected_move;

	bool hovering_over_piece = false;
	bool hovering_outside = false;
	bool dragging = false;
	bool clicked_on_possible_move = false;
	bool valid_hover = false;
	bool valid_drop = false;
	bool placed_selected = false;
	bool valid_placement = false;
	bool valid_dragging = false;
	bool promotion_black = false;
	bool promotion_white = false;
	bool king_check = false;

	std::vector<move> possible_moves;
	std::vector<piece_graphic> pieces;

	std::array<piece_graphic, 4> promotion_pieces;
	qsf::smooth_rectangle promotion_background;
	qpl::size promotion_hover_index = 0u;

	qpl::animation light_hover_animation;
	qsf::sprite light_sprite;
	qsf::sprite king_check_sprite;
	qpl::rgba light_sprite_color = qpl::rgb::cyan().with_alpha(100);
	sf::Texture texture;
	qsf::vertex_array va;

	void init();

	void reset_board();
	void update_board();
	void highlight_square(qpl::vec2s pos, bool highlight);
	void resize(qpl::vec2f dimension);

	void handle_en_pessant(qpl::vec2s to);
	void handle_castling(qpl::vec2s to);
	void handle_promotion(qpl::vec2s to);
	void move_piece(qpl::vec2s from, qpl::vec2s to);
	void promote_piece(qpl::vec2s position, piece to);

	void next_turn();

	void setup_new_move_animation(qpl::vec2s to);
	void update_move_animation(const qsf::event_info& event);
	void find_dragging_piece();

	piece_graphic& get_piece(qpl::vec2s pos);
	const piece_graphic& get_piece(qpl::vec2s pos) const;

	piece_graphic& get_dragging_piece();
	const piece_graphic& get_dragging_piece() const;

	void update_hovering_index(const qsf::event_info& event);
	void update_dragging(const qsf::event_info& event);
	void update_check_possible_moves(const qsf::event_info& event);
	void update_hightlighting();
	void update_piece_placement();

	void update_piece_promotion(const qsf::event_info& event);
	void update(const qsf::event_info& event);

	void draw(qsf::draw_object& draw) const;
};