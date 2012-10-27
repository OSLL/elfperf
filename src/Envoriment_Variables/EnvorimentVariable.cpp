/*
 * EnvorimentVariable.cpp
 *
 *  Created on: Oct 27, 2012
 *      Author: aleksey
 */
#include "EnvorimentVariable.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


char **getVariableList(const char* name) {
	char *p = getenv(name);
	if (*p == NULL) {
		char **arr = new char*[0];
		return arr;
	}
	int counter = 0;
	while (*p != NULL) {
		if (*p == ' ') {
			counter++;
		}
		p++;
	}
	char *temp = getenv(name);
	char *result = NULL;
	char **arr = new char*[counter];

	result = strtok(temp, " ");
	int k = 0;
	while (result != NULL) {
		arr[k] = result;
		result = strtok(NULL, " ");
		k++;
	}

	return arr;
}


