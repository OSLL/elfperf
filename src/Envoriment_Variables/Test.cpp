/*
 * Test.cpp
 *
 *  Created on: Oct 27, 2012
 *      Author: aleksey
 */
#include "EnvorimentVariable.h"

int main() {

	setenv("USER", "printf open list", 1);
	char **p = getVariableList("USER");
	int size = sizeof(p);
	for (int i = 0, size = sizeof(p); i < size; ++i) {
		std::cout << p[i] << "\n"; // Whill be print: printf open list
	}

	return 0;
}

