#include "square.hpp"

std::string square_name(qpl::vec2s pos) {
	auto rank = qpl::char_cast((pos.y) + 'a');
	auto file = qpl::char_cast((7 - pos.x) + '1');
	return qpl::to_string(rank, file);
}
