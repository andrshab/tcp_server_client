#ifndef PARSER_H
#define PARSER_H

#include <vector>

#define PARSER_DELIMITER ' '

using namespace std;

class Parser
{
public:
	vector<string> values;
	void parse(char *buffer);
	void parse(const char *buffer);
};

#endif
