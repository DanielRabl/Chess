#include <qpl/qpl.hpp>

#include "chess_game.hpp"


struct main_state : qsf::base_state {
	void init() override {
		this->chess_game.init();

		this->call_on_resize();
		this->clear_color = qpl::rgb(90, 70, 50).darkened(0.15);
	}
	void call_on_resize() override {
		this->chess_game.resize(this->dimension());
		this->chess_game.update_board();
	}
	void updating() override {
		this->update(this->chess_game);
	}
	void drawing() override {
		this->draw(this->chess_game);
	}

	chess_game chess_game;
};

void client() {
	qsf::socket socket;
	socket.connect(sf::IpAddress::LocalHost, 2000);

	qpl::println("connected");

	auto input = "123";
	socket.send(input);
	qpl::println("ON CLIENT : received ", socket.receive());
}

void server() {
	qsf::socket socket;

	qsf::socket listener;
	listener.listen(2000);
	listener.accept(socket);

	qpl::println("connected");

	auto input = "321";
	socket.send(input);
	qpl::println("ON CLIENT : received ", socket.receive());
}

int main() try {
	//server();
	//client();

	qsf::framework framework;
	framework.set_title("QPL");
	framework.set_antialiasing_level(3);
	framework.add_texture("light", "resources/light512.png");
	framework.set_dimension({ 1400u, 950u });

	framework.add_state<main_state>();
	framework.game_loop();
}
catch (std::exception& any) {
	qpl::println("caught exception:\n", any.what());
	qpl::system_pause();
}