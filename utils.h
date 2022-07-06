#ifndef UTILS_H_
#define UTILS_H_

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

//  macro for error handling
#define DIE(assertion, des)												\
	do {																\
		if (assertion) {								\
			fprintf(stderr, "Error at %s %d: %s\n", __FILE__, __LINE__, des);\
			exit(errno);													\
		}																	\
	} while (0)

#endif  //  UTILS_H_
