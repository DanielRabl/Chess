#pragma once

struct move {
	move(qpl::vec2s position = qpl::vec(0, 0), bool capture = false, bool castle = false, bool en_pessant = false) {
		this->position = position;
		this->capture = capture;
		this->castle = castle;
		this->en_pessant = en_pessant;
	}

	qpl::vec2s position;
	bool castle = false;
	bool en_pessant = false;
	bool capture = false;
};
