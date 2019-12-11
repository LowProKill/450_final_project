#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <ctype.h>
#include <string>

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
#include "bmp_to_texture.h"
#include "load_obj.h"
#include "sphere.h"


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.


// title of these windows:

const char* WINDOWTITLE = { "Jonathan Hull" };
const char* GLUITITLE = { "User Interface Window" };

unsigned char* texture;
unsigned char* ground_tex;
unsigned char* wall_tex;
unsigned char* world_tex;

float light0Colour[3] = { 1, 0, 0 };
float light1Colour[3] = { 0, 0, 1 };
float light2Colour[3] = { 0, 1, 0 };
float light3Colour[3] = { 0.5, 0.5, 0.5 };

// what the glui package defines as true and false:

const int GLUITRUE = { true };
const int GLUIFALSE = { false };

// the escape key:

#define ESCAPE		0x1b

// initial window size:

const int INIT_WINDOW_SIZE = { 600 };


auto previous_time = std::chrono::high_resolution_clock::now();

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };


// minimum allowable scale factor:

const float MINSCALE = { 0.05f };


// active mouse buttons (or them together):

const int LEFT = { 4 };
const int MIDDLE = { 2 };
const int RIGHT = { 1 };


// which projection:

enum Projections
{
	ORTHO,
	PERSP
};


// which button:

enum ButtonVals
{
	RESET,
	QUIT
};


// window background color (rgba):

const GLfloat BACKCOLOR[] = { 0., 0., 0., 1. };


// line width for the axes:

const GLfloat AXES_WIDTH = { 3. };


// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char* ColorNames[] =
{
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};


// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE = { GL_LINEAR };
const GLfloat FOGDENSITY = { 0.30f };
const GLfloat FOGSTART = { 1.5 };
const GLfloat FOGEND = { 4. };

float rotate = 0;
const int num_frame = 22;
const int num_jump_frame = 69;
const int num_obst = 1;
const int frame_29 = 0;
const float speed = 0.3;

// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to use the z-buffer
GLuint	obj_list[num_frame];			// object display list
GLuint  jump_list[num_jump_frame];
GLuint  obst_list[num_obst];
GLuint  ground_list;
GLuint	wall_list;
GLuint  torus_list;
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		light0;
int		light1;
int		light2;
int		light3;
int		level_of_specular = 1;
int		anim = 1;
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees
bool  jumping = false;
float pos = 0;
float obst_pos[num_obst];
int frame = 0;
int jump_frame = 0;
bool  game_started = false;
bool  obst_through[num_obst];
int   score = 0;

// function prototypes:

void	Animate();
void	Display();
void	DoLight0Menu(int);
void	DoLight1Menu(int);
void	DoLight2Menu(int);
void	DoLight3Menu(int);
void    DoLevelOfSpecularMenu(int);
void    DoAnimMenu(int);
void	DoAxesMenu(int);
void	DoColorMenu(int);
void	DoDepthBufferMenu(int);
void	DoDepthFightingMenu(int);
void	DoDepthMenu(int);
void	DoDebugMenu(int);
void	DoMainMenu(int);
void	DoProjectMenu(int);
void	DoRasterString(float, float, float, char*);
void	DoStrokeString(float, float, float, float, char*);
float	ElapsedSeconds();
void	InitGraphics();
void	InitLists();
void	InitMenus();
void	Keyboard(unsigned char, int, int);
void	MouseButton(int, int, int, int);
void	MouseMotion(int, int);
void	Reset();
void	Resize(int, int);
void	Visibility(int);
void	initTexture();

void	Axes(float);
void	HsvRgb(float[3], float[3]);

// main program:

int
main(int argc, char* argv[])
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit(&argc, argv);


	// setup all the graphics stuff:

	InitGraphics();


	// create the display structures that will not change:

	initTexture();

	InitLists();


	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset();


	// setup all the user interface stuff:

	InitMenus();


	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow(MainWindow);
	glutMainLoop();


	// this is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void
Animate()
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:
	auto now = std::chrono::high_resolution_clock::now();
	auto delta_time = now - previous_time;
	if (anim == 1)
	{
		rotate += delta_time.count() / 6000000;
		if (rotate >= 360) { rotate -= 360; }
		if (delta_time.count() >= 1.0)
		{
			previous_time = now;
			if (jumping)
			{
				jump_frame++;
				if (jump_frame >= num_jump_frame) { jump_frame = 0; jumping = 0; frame = 0; }
			}
			frame++;
			if (frame >= num_frame) { frame = 0; }
		}
		if (game_started)
		{
			for (int i = 0; i < num_obst; i++)
			{
				obst_pos[i] -= speed * delta_time.count() * 0.0000001;
				if (obst_pos[i] >= -12.5 && obst_pos[i] <= -11.5 && !obst_through[i])
				{
					if (!jumping || jump_frame <= 20 || jump_frame >= 35) { printf("hit\n"); Reset(); }
					else { score++; }
					obst_through[i] = true;
				}
				if (obst_pos[i] < -50) { obst_pos[i] = float(rand() % 30 + 50); obst_through[i] = false; }
			}
		}
	}
	else { previous_time = now; }




	// force a call to Display( ) next time it is convenient:

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

float*
Array3(float a, float b, float c)
{
	static float array[4];
	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

float*
MulArray3(float factor, float array0[3])
{
	static float array[4];
	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}

void
SetPointLight(int ilight, float x, float y, float z, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(ilight, GL_DIFFUSE, Array3(0.5 * r, 0.5 * g, 0.5 * b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}

void
SetSpotLight(int ilight, float x, float y, float z, float xdir, float ydir, float zdir, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_SPOT_DIRECTION, Array3(xdir, ydir, zdir));
	glLightf(ilight, GL_SPOT_EXPONENT, 1.);
	glLightf(ilight, GL_SPOT_CUTOFF, 45.);
	glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}

void
SetMaterial(float r, float g, float b, float shininess)
{
	glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4f, (float*)Colors[WHITE]));
	glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., (float*)Colors[WHITE]));
	glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
	glMaterialf(GL_BACK, GL_SHININESS, 2.f);
	glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(shininess, (float*)Colors[WHITE]));
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

// draw the complete scene:

void
Display()
{
	if (DebugOn != 0)
	{
		fprintf(stderr, "Display\n");
	}


	// set which window we want to do the graphics into:

	glutSetWindow(MainWindow);


	// erase the background:

	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (DepthBufferOn != 0)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);


	// specify shading to be flat:




	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (0)
		glOrtho(-3., 3., -3., 3., 0.1, 1000.);
	else
		gluPerspective(90., 1., 0.1, 1000.);


	// place the objects into the scene:

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// set the eye position, look-at position, and up-vector:
	if (1) { gluLookAt(-5, 3, 15, 2., -7, -5, 0, 1, 0); }
	else
	{
		gluLookAt(0, -6.5, 25, 0, -6.5, 0, 0, 1, 0);

		// rotate the scene:

		glRotatef((GLfloat)Yrot, 0., 1., 0.);
		glRotatef((GLfloat)Xrot, 1., 0., 0.);

		// uniformly scale the scene:

		if (Scale < MINSCALE)
			Scale = MINSCALE;
		glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
	}

	// set the fog parameters:

	if (DepthCueOn != 0)
	{
		glFogi(GL_FOG_MODE, FOGMODE);
		glFogfv(GL_FOG_COLOR, FOGCOLOR);
		glFogf(GL_FOG_DENSITY, FOGDENSITY);
		glFogf(GL_FOG_START, FOGSTART);
		glFogf(GL_FOG_END, FOGEND);
		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}


	// possibly draw the axes:

	if (AxesOn != 0)
	{
		//glColor3fv(&Colors[WhichColor][0]);
		//glCallList(AxesList);
	}


	// since we are using glScalef( ), be sure normals get unitized:
	int x = 1024;
	int y = 512;
	int fourdy96 = 4096;
	int level = 0;
	int ncomps = 3;
	int border = 0;
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, level, ncomps, fourdy96, fourdy96, border, GL_RGB, GL_UNSIGNED_BYTE, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	SetMaterial(1, 1, 1, 0.8);

	// draw the current object:

	if (frame_29 == 1) { frame = 14; }

	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);




	glPushMatrix();
	glTranslatef(-3, 0, 0);
	if (1 == 1) { SetPointLight(GL_LIGHT3, 0, 0, 0, light3Colour[0], light3Colour[1], light3Colour[2]); }
	else { glDisable(GL_LIGHT3); }
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glColor3f(light3Colour[0], light3Colour[1], light3Colour[2]);
	//glutSolidSphere(0.2, 16, 16);
	glEnable(GL_LIGHTING);
	glPopMatrix();




	glPushMatrix();
	glTranslatef(-12., -7., -5.);
	glRotatef(90, 0, 1, 0);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	if (!jumping) { glCallList(obj_list[frame]); }
	else { glCallList(jump_list[jump_frame]); }
	glPopMatrix();

	//for(int i = 0; i < num_frame; i++)
	{
		glPushMatrix();
		glDisable(GL_TEXTURE_2D);
		glColor3f(0, 1, 0);
		glTranslatef(obst_pos[0], -7, -5);
		glScalef(1, 2, 1);
		glCallList(obst_list[0]);
		glPopMatrix();
	}


	SetMaterial(1, 1, 1, 0);
	glPushMatrix();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	int grass_size = 128;
	glTexImage2D(GL_TEXTURE_2D, level, ncomps, grass_size, grass_size, border, GL_RGB, GL_UNSIGNED_BYTE, ground_tex);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glScalef(10, 10, 10);
	glMatrixMode(GL_MODELVIEW);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
	glTranslatef(-2, -10, 2);
	glRotatef(90, 0, 1, 0);
	glScalef(4, 3, 4);
	glCallList(ground_list);
	//glCallList(wall_list);
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);
	glColor3f(0., 1., 1.);


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	char buf[32];

	snprintf(buf, 32, "Score = %d", score);

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0., 100., 0., 100.);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1., 1., 1.);
	DoRasterString(5., 90., 0., buf);


	// swap the double-buffered framebuffers:

	glutSwapBuffers();


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush();
}

void
initTexture()
{
	int x = 1024;
	int y = 512;
	int fourdy96 = 4096;
	int grass_size = 128;
	texture = BmpToTexture("daniel/Daniel_packed0_diffuse.bmp", &fourdy96, &fourdy96);
	ground_tex = BmpToTexture("daniel/ground_text.bmp", &grass_size, &grass_size);
	wall_tex = NULL;
	world_tex = BmpToTexture("worldtex.bmp", &x, &y);
	int level = 0;
	int ncomps = 3;
	int border = 0;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, level, ncomps, x, y, border, GL_RGB, GL_UNSIGNED_BYTE, texture);
}


void
DoAnimMenu(int id)
{
	anim = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}
void
DoLevelOfSpecularMenu(int id)
{
	level_of_specular = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}
void
DoLight0Menu(int id)
{
	light0 = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}
void
DoLight1Menu(int id)
{
	light1 = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}
void
DoLight2Menu(int id)
{
	light2 = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}
void
DoLight3Menu(int id)
{
	light0 = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}
void
DoAxesMenu(int id)
{
	AxesOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}
void
DoColorMenu(int id)
{
	WhichColor = id - RED;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}
void
DoDebugMenu(int id)
{
	DebugOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}
void
DoDepthBufferMenu(int id)
{
	DepthBufferOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}
void
DoDepthFightingMenu(int id)
{
	DepthFightingOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}
void
DoDepthMenu(int id)
{
	DepthCueOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// main menu callback:

void
DoMainMenu(int id)
{
	switch (id)
	{
	case RESET:
		Reset();
		break;

	case QUIT:
		// gracefully close out the graphics:
		// gracefully close the graphics window:
		// gracefully exit the program:
		glutSetWindow(MainWindow);
		glFinish();
		glutDestroyWindow(MainWindow);
		exit(0);
		break;

	default:
		fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoProjectMenu(int id)
{
	WhichProjection = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}




// use glut to display a string of characters using a raster font:

void
DoRasterString(float x, float y, float z, char* s)
{
	glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString(float x, float y, float z, float ht, char* s)
{
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	float sf = ht / (119.05f + 33.33f);
	glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}
	glPopMatrix();
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds()
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet(GLUT_ELAPSED_TIME);

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus()
{
	glutSetWindow(MainWindow);

	int numColors = sizeof(Colors) / (3 * sizeof(int));
	int colormenu = glutCreateMenu(DoColorMenu);
	for (int i = 0; i < numColors; i++)
	{
		glutAddMenuEntry(ColorNames[i], i);
	}

	int losMenu = glutCreateMenu(DoLevelOfSpecularMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);
	glutAddMenuEntry("Just Why", 5);

	int axesmenu = glutCreateMenu(DoAxesMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int animMenu = glutCreateMenu(DoAnimMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int light0menu = glutCreateMenu(DoLight0Menu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int light1menu = glutCreateMenu(DoLight1Menu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int light2menu = glutCreateMenu(DoLight2Menu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int light3menu = glutCreateMenu(DoLight3Menu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthcuemenu = glutCreateMenu(DoDepthMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int debugmenu = glutCreateMenu(DoDebugMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int projmenu = glutCreateMenu(DoProjectMenu);
	glutAddMenuEntry("Orthographic", ORTHO);
	glutAddMenuEntry("Perspective", PERSP);


	int mainmenu = glutCreateMenu(DoMainMenu);
	//glutAddSubMenu("View", viewmenu);
	//glutAddSubMenu("Distort", distortmenu);
	//glutAddSubMenu("Texture", texturemenu);
	//glutAddSubMenu("Axes", axesmenu);
	//glutAddSubMenu("Colors", colormenu);
	//glutAddSubMenu("Depth Buffer", depthbuffermenu);
	//glutAddSubMenu("Depth Fighting", depthfightingmenu);
	//glutAddSubMenu("Depth Cue", depthcuemenu);
	//glutAddSubMenu("Projection", projmenu);
	//glutAddMenuEntry("Reset", RESET);
	//glutAddSubMenu("Debug", debugmenu);
	glutAddSubMenu("Animation", animMenu);
	glutAddSubMenu("light0", light0menu);
	glutAddSubMenu("light1", light1menu);
	glutAddSubMenu("light2", light2menu);
	glutAddSubMenu("light3", light3menu);
	glutAddSubMenu("Specular", losMenu);
	glutAddMenuEntry("Quit", QUIT);

	// attach the pop-up menu to the right mouse button:

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics()
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// set the initial window configuration:

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

	// open the window and set its title:

	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);

	// set the framebuffer clear values:

	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(NULL);
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(-1, NULL, 0);
	glutIdleFunc(Animate);

	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "glewInit Error\n");
	}
	else
		fprintf(stderr, "GLEW initialized OK\n");
	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

}



// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void make_obj_list(int i, char* obj_names)
{
	glPushMatrix();
	obj_list[i] = glGenLists(1);
	glNewList(obj_list[i], GL_COMPILE);
	glScalef(4, 4, 4);
	glColor3f(1, 0, 0.5);
	LoadObjFile(obj_names);
	printf("Loaded obj %s\n", obj_names);
	glEndList();
	glPopMatrix();
}

void make_jump_list(int i, char* obj_names)
{
	glPushMatrix();
	jump_list[i] = glGenLists(1);
	glNewList(jump_list[i], GL_COMPILE);
	glScalef(4, 4, 4);
	glColor3f(1, 0, 0.5);
	LoadObjFile(obj_names);
	printf("Loaded obj %s\n", obj_names);
	glEndList();
	glPopMatrix();
}

void
InitLists()
{
	char* obj_names[num_frame] = {
		"daniel/Daniel_000001.obj",
		"daniel/Daniel_000002.obj",
		"daniel/Daniel_000003.obj",
		"daniel/Daniel_000004.obj",
		"daniel/Daniel_000005.obj",
		"daniel/Daniel_000006.obj",
		"daniel/Daniel_000007.obj",
		"daniel/Daniel_000008.obj",
		"daniel/Daniel_000009.obj",
		"daniel/Daniel_000010.obj",
		"daniel/Daniel_000011.obj",
		"daniel/Daniel_000012.obj",
		"daniel/Daniel_000013.obj",
		"daniel/Daniel_000014.obj",
		"daniel/Daniel_000015.obj",
		"daniel/Daniel_000016.obj",
		"daniel/Daniel_000017.obj",
		"daniel/Daniel_000018.obj",
		"daniel/Daniel_000019.obj",
		"daniel/Daniel_000020.obj",
		"daniel/Daniel_000021.obj",
		"daniel/Daniel_000022.obj"
	};
	char* jump_names[num_jump_frame] = {
	  "jump/untitled_000001.obj",
	  "jump/untitled_000002.obj",
	  "jump/untitled_000003.obj",
	  "jump/untitled_000004.obj",
	  "jump/untitled_000005.obj",
	  "jump/untitled_000006.obj",
	  "jump/untitled_000007.obj",
	  "jump/untitled_000008.obj",
	  "jump/untitled_000009.obj",
	  "jump/untitled_000010.obj",
	  "jump/untitled_000011.obj",
	  "jump/untitled_000012.obj",
	  "jump/untitled_000013.obj",
	  "jump/untitled_000014.obj",
	  "jump/untitled_000015.obj",
	  "jump/untitled_000016.obj",
	  "jump/untitled_000017.obj",
	  "jump/untitled_000018.obj",
	  "jump/untitled_000019.obj",
	  "jump/untitled_000020.obj",
	  "jump/untitled_000021.obj",
	  "jump/untitled_000022.obj",
	  "jump/untitled_000023.obj",
	  "jump/untitled_000024.obj",
	  "jump/untitled_000025.obj",
	  "jump/untitled_000026.obj",
	  "jump/untitled_000027.obj",
	  "jump/untitled_000028.obj",
	  "jump/untitled_000029.obj",
	  "jump/untitled_000030.obj",
	  "jump/untitled_000031.obj",
	  "jump/untitled_000032.obj",
	  "jump/untitled_000033.obj",
	  "jump/untitled_000034.obj",
	  "jump/untitled_000035.obj",
	  "jump/untitled_000036.obj",
	  "jump/untitled_000037.obj",
	  "jump/untitled_000038.obj",
	  "jump/untitled_000039.obj",
	  "jump/untitled_000040.obj",
	  "jump/untitled_000041.obj",
	  "jump/untitled_000042.obj",
	  "jump/untitled_000043.obj",
	  "jump/untitled_000044.obj",
	  "jump/untitled_000045.obj",
	  "jump/untitled_000046.obj",
	  "jump/untitled_000047.obj",
	  "jump/untitled_000048.obj",
	  "jump/untitled_000049.obj",
	  "jump/untitled_000050.obj",
	  "jump/untitled_000051.obj",
	  "jump/untitled_000052.obj",
	  "jump/untitled_000053.obj",
	  "jump/untitled_000054.obj",
	  "jump/untitled_000055.obj",
	  "jump/untitled_000056.obj",
	  "jump/untitled_000057.obj",
	  "jump/untitled_000058.obj",
	  "jump/untitled_000059.obj",
	  "jump/untitled_000060.obj",
	  "jump/untitled_000061.obj",
	  "jump/untitled_000062.obj",
	  "jump/untitled_000063.obj",
	  "jump/untitled_000064.obj",
	  "jump/untitled_000065.obj",
	  "jump/untitled_000066.obj",
	  "jump/untitled_000067.obj",
	  "jump/untitled_000068.obj",
	  "jump/untitled_000069.obj"
	};

	//char* obj_name = (char *)malloc(sizeof(char) * 30); 
	if (frame_29 == 1) { make_obj_list(14, obj_names[14]); }
	else
	{
		for (int i = 0; i < num_frame; i++) { make_obj_list(i, obj_names[i]); }
		for (int i = 0; i < num_jump_frame; i++) { make_jump_list(i, jump_names[i]); }
	}

	glColor3f(0, 0, 0);
	glPushMatrix();
	ground_list = glGenLists(1);
	glNewList(ground_list, GL_COMPILE);
	glColor3f(0.5, 0.5, 0);
	LoadObjFile("daniel/ground.obj");
	glEndList();
	glPopMatrix();

	glPushMatrix();
	wall_list = glGenLists(1);
	glNewList(wall_list, GL_COMPILE);
	glColor3f(0.5, 0.5, 0);
	LoadObjFile("daniel/wall.obj");
	glEndList();
	glPopMatrix();

	glPushMatrix();
	obst_list[0] = glGenLists(1);
	glNewList(obst_list[0], GL_COMPILE);
	glColor3f(0, 1, 0);
	//glScalef(0.4, 1, 0.4);
	LoadObjFile("daniel/obst.obj");
	glEndList();
	glPopMatrix();
}


// the keyboard callback:

void
Keyboard(unsigned char c, int x, int y)
{
	if (DebugOn != 0)
		fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

	switch (c)
	{
	case ' ':
		if (!game_started) { score = 0; }
		jumping = true;
		game_started = true;
		break;
	case 'q':
	case 'Q':
	case ESCAPE:
		DoMainMenu(QUIT);	// will not return here
		break;				// happy compiler

	default:
		fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
	}

	// force a call to Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// called when the mouse button transitions down or up:

void
MouseButton(int button, int state, int x, int y)
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if (DebugOn != 0)
		fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);


	// get the proper button bit mask:

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		b = LEFT;		break;

	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;

	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;

	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}


	// button down sets the bit, up clears the bit:

	if (state == GLUT_DOWN)
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}


// called when the mouse moves while a button is down:

void
MouseMotion(int x, int y)
{
	if (DebugOn != 0)
		fprintf(stderr, "MouseMotion: %d, %d\n", x, y);


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if ((ActiveButton & LEFT) != 0)
	{
		Xrot += (ANGFACT * dy);
		Yrot += (ANGFACT * dx);
	}


	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);

		// keep object from turning inside-out or disappearing:

		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset()
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale = 1.0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;

	game_started = false;
	float position = 80;
	for (int i = 0; i < num_obst; i++) { obst_pos[i] = position; position += 25; obst_through[i] = false; }
}


// called when user resizes the window:

void
Resize(int width, int height)
{
	if (DebugOn != 0)
		fprintf(stderr, "ReSize: %d, %d\n", width, height);

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// handle a change to the window's visibility:

void
Visibility(int state)
{
	if (DebugOn != 0)
		fprintf(stderr, "Visibility: %d\n", state);

	if (state == GLUT_VISIBLE)
	{
		glutSetWindow(MainWindow);
		glutPostRedisplay();
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = {
		0.f, 1.f, 0.f, 1.f
};

static float xy[] = {
		-.5f, .5f, .5f, -.5f
};

static int xorder[] = {
		1, 2, -3, 4
};

static float yx[] = {
		0.f, 0.f, -.5f, .5f
};

static float yy[] = {
		0.f, .6f, 1.f, 1.f
};

static int yorder[] = {
		1, 2, 3, -2, 4
};

static float zx[] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
};

static float zy[] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
};

static int zorder[] = {
		1, 2, 3, 4, -5, 6
};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes(float length)
{
	glBegin(GL_LINE_STRIP);
	glVertex3f(length, 0., 0.);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., length, 0.);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 0., length);
	glEnd();

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 4; i++)
	{
		int j = xorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 5; i++)
	{
		int j = yorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 6; i++)
	{
		int j = zorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
	}
	glEnd();

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb(float hsv[3], float rgb[3])
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while (h >= 6.)	h -= 6.;
	while (h < 0.) 	h += 6.;

	float s = hsv[1];
	if (s < 0.)
		s = 0.;
	if (s > 1.)
		s = 1.;

	float v = hsv[2];
	if (v < 0.)
		v = 0.;
	if (v > 1.)
		v = 1.;

	// if sat==0, then is a gray:

	if (s == 0.0)
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:

	float i = floor(h);
	float f = h - i;
	float p = v * (1.f - s);
	float q = v * (1.f - s * f);
	float t = v * (1.f - (s * (1.f - f)));

	float r, g, b;			// red, green, blue
	switch ((int)i)
	{
	case 0:
		r = v;	g = t;	b = p;
		break;

	case 1:
		r = q;	g = v;	b = p;
		break;

	case 2:
		r = p;	g = v;	b = t;
		break;

	case 3:
		r = p;	g = q;	b = v;
		break;

	case 4:
		r = t;	g = p;	b = v;
		break;

	case 5:
		r = v;	g = p;	b = q;
		break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}
