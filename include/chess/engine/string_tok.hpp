#pragma once

#include <string>
#include <list>

class StringTok {
public:
	StringTok(const std::string& s);
	void addDelimiter(const std::string& delimiter);
	void reset();
	std::string getNext();

private:
	std::list<std::string> _subStrings;
	std::string _s;
};