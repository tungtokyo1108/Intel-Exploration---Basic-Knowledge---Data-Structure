#pragma once
/*
*
*  Created on: Nov 11, 2018
*      Student (MIG Virtual Developer): Tung Dang
*/

#include <sys/types.h>
#ifdef _WIN32
#include <basetsd.h>

#define HAVE_MODE_T 1

/*
* This is a massive pain to have be an "int" due to the pthread implementation 
*/
using pid_t = int;
using ssize_t = SSIZE_T;
using mode_t = unsigned short;

#endif 