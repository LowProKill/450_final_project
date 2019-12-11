#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"

void MjbSphere(float radius, int slices, int stacks, bool distort);
void init_time();
//std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<__int64, std::ratio<1, 1000000000>>> time,