#include "../include/chess/engine/string_tok.hpp"

StringTok::StringTok(const std::string& s)
	: _s(s), _subStrings() {}

void StringTok::addDelimiter(const std::string& delimiter) {
	size_t start_pos = 0;
	while((start_pos = _s.find(delimiter, start_pos)) != std::string::npos) {
		size_t end_pos = start_pos + delimiter.length();
		_s = _s.substr(0, start_pos++) + '\0' + _s.substr(end_pos, _s.length() - end_pos);
	}
	reset();
}

void StringTok::reset() {
	_subStrings.clear();
	size_t end_pos = 0;
	for(size_t start_pos = 0; start_pos < _s.length();) {
		for(end_pos = start_pos + 1; end_pos < _s.length(); end_pos++) {
			if(_s[end_pos] == '\0') {
				break;
			}
		}
		_subStrings.push_back(_s.substr(start_pos, end_pos - start_pos));
		start_pos = end_pos;
		while(++start_pos < _s.length()) {
			if(_s[start_pos] != '\0') {
				break;
			}
		}
	}
}

std::string StringTok::getNext() {
	if(_subStrings.empty()) {
		return "";
	} else {
		std::string front = _subStrings.front();
		_subStrings.pop_front();
		return front;
	}
}