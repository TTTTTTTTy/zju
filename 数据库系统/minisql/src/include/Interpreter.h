
#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <fstream>
#include "API.h"
class Interpreter {
public:

	API* api;
	string fileName;
	int interpreter(string s);

	string getWord(string s, int *st);

	Interpreter() {}
	~Interpreter() {}
};

#endif