#pragma once
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

#include <stdio.h>
#include <string.h>
#include <vector>


int LoadObjFile(char* name);
