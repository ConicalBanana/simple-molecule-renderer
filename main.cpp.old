#include<iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <valarray>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include<GLUT/glut.h>
#else
#include<GL/gl.h>
#endif

#include "Element.hpp"
#include "Callback.hpp"
#include "Util.hpp"

/* Global Variables (Configs) */
//Init options
bool binaryLighting = 0;  // Binary/two-tone lighting toggle
bool flatColoring = 0;    // Uniform flat coloring for 3D-to-2D effect

const GLdouble RADIUS_SCALING_FACTOR = 0.3;
const GLdouble CYLINDER_RADIUS = 0.1;
const GLint resolution = 100;
const GLint slices = resolution;
const GLint stacks = resolution;

//colors
GLfloat WHITE[3] =		{1.0, 1.0, 1.0};
GLfloat BLACK[3] =		{0.0, 0.0, 0.0};
GLfloat* BACKGROUND_COLOR = BLACK;

// element id
const int ID_HYDROGEN = 1 - 1;
const int ID_CARBON = 6 - 1;
const int ID_OXYGEN = 8 - 1;

/* Prototypes */
void drawAtom(int element_id);
void setLightColor(GLfloat light_color[3]);
void setEnvLight();
void buildDisplayList();

int wd;                   /* GLUT window handle */


int main(int argc, char *argv[])
{
	/* initialize GLUT, let it extract command-line 
	 GLUT options that you may provide 
	 - NOTE THE '&' BEFORE argc */
	glutInit(&argc, argv);
	
	/* specify the display to be single 
     buffered and color as RGBA values */
	glutInitDisplayMode(GLUT_DEPTH);
	
	/* set the initial window size */
	glutInitWindowSize((int) glcb::width, (int) glcb::height);
	
	/* create the window and store the handle to it */
	wd = glutCreateWindow("2D renderer" /* title */ );
	
	/* --- register callbacks with GLUT --- */
	
	/* register function to handle window resizes */
	glutReshapeFunc(glcb::reshape);
	
	setLightColor(WHITE);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear buffers to preset values
	glEnable(GL_DEPTH_TEST);  // enable or disable server-side GL capabilities
	/*
	glCullFace specifies whether front- or back-facing facets are culled (as specified by mode) when facet culling is enabled.
	Facet culling is initially disabled.
	To enable and disable facet culling, call the glEnable and glDisable commands with the argument GL_CULL_FACE.
	Facets include triangles, quadrilaterals, polygons, and rectangles.
	*/
	glCullFace(GL_BACK);  // specify whether front- or back-facing facets can be culled
	glEnable(GL_CULL_FACE);
	// Backgound color setting
	//Specify the red, green, blue, and alpha values used when the color buffers are cleared. The initial values are all 0.
	glClearColor(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2], 0.0);
	
	buildDisplayList();
	setEnvLight();
	
	// Print control instructions
	std::cout << "\n=== Molecule Renderer Controls ===" << std::endl;
	std::cout << "ESC: Exit application" << std::endl;
	std::cout << "L/l: Toggle lighting effect" << std::endl;
	std::cout << "B/b: Toggle binary/two-tone lighting" << std::endl;
	std::cout << "F/f: Toggle flat coloring (3D-to-2D effect)" << std::endl;
	std::cout << "Mouse: Left click + drag to rotate" << std::endl;
	std::cout << "Mouse: Middle click + drag to zoom" << std::endl;
	std::cout << "================================\n" << std::endl;
	
	// Callbacks
	glutDisplayFunc(glcb::displayCallback);
	glutMouseFunc(glcb::mouseCallback);
	glutMotionFunc(glcb::motionCallback);
	glutKeyboardFunc(glcb::keyboardCallback);
	
	/* start the GLUT main loop */
	glutMainLoop();
	
	return 0;
}

//Sphere
void drawAtom(int element_id)
{
    GLfloat color[3];
    color[0] = elem::COLOR_ARRAY[element_id][0];
    color[1] = elem::COLOR_ARRAY[element_id][1];
    color[2] = elem::COLOR_ARRAY[element_id][2];

    GLfloat radius = elem::VDWR_ARRAY[element_id] * RADIUS_SCALING_FACTOR;
    // without light
	// glColor3fv(color);
	// with light
	setLightColor(color);
	
	GLUquadric *myQuad;
	myQuad=gluNewQuadric();
	
	//Création de la sphere
	gluSphere(myQuad , radius , slices , stacks);
}

void setLightColor(GLfloat light_color[3])
{
	if (flatColoring) {
		// Flat coloring: disable all lighting for uniform colors
		glDisable(GL_LIGHTING);
		glColor3fv(light_color);  // Set uniform color directly
		return;
	}
	
	// Re-enable lighting for other modes
	if (glcb::lightEffect) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	
	if (binaryLighting) {
		// Binary lighting: use only two intensity levels
		GLfloat bright_color[3] = {light_color[0], light_color[1], 
		                          light_color[2]};
		GLfloat dim_color[3] = {light_color[0] * 0.3f, 
		                       light_color[1] * 0.3f, 
		                       light_color[2] * 0.3f};
		
		// Set material properties for binary lighting
		GLfloat mat_ambient[] = {dim_color[0], dim_color[1], 
		                        dim_color[2], 1.0};
		GLfloat mat_diffuse[] = {bright_color[0], bright_color[1], 
		                        bright_color[2], 1.0};
		GLfloat mat_specular[] = {0.0, 0.0, 0.0, 1.0};  // No specular
		GLfloat shine[] = {1.0};  // Very low shininess
		
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, shine);
		
		// Set light properties for binary effect
		GLfloat light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
		GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		GLfloat light_specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
		
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	} else {
		// Normal lighting
		GLfloat mat_specular[]={ 1.0, 1.0, 1.0, 1.0 };
		GLfloat shine[] = {100.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular );
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_color );
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color );
		glMaterialfv(GL_FRONT, GL_SHININESS, shine);
	}
}

/* Our display items to prepare */
void buildDisplayList()
{
	// See also: https://songho.ca/opengl/gl_displaylist.html

	GLuint id;
	GLUquadric *myQuad;
	myQuad=gluNewQuadric();
	
	// Pre-compiled display lists
	id = glGenLists( 1 );
	glNewList( id, GL_COMPILE );
	glPushMatrix();
	
	// will draw from file	
	int nbElements = 0;

	std::ifstream file("caffeine.xyz");
	if (file.fail())
	{
		std::cout << "caffeine.xyz file not found" << std::endl;
		return;
	}

	std::cout << "caffeine.xyz file opened successfully" << std::endl;
	std::string buff;

	std::getline(file, buff);  // Atom count
	nbElements = std::stoi(buff);
	std::getline(file, buff);  // Title

	std::valarray<std::string> atomTypes(nbElements);
	std::valarray<std::array<GLfloat, 3>> atomePositions(nbElements);

	//for every elements, push, draw, pop
	for (int i = 0; i < nbElements; i++)
	{
		std::getline(file, buff);
		std::stringstream buff_ss(buff);
		buff_ss >> atomTypes[i] >> atomePositions[i][0] >> atomePositions[i][1] >> atomePositions[i][2];
	}
	if (file.is_open()){file.close();}

	// Draw atoms
	double exp_bond_length = 0;
	double actual_bond_length = 0;
	for (int i = 0; i < nbElements; i++)
	{
		glPushMatrix();
		glTranslatef(atomePositions[i][0], atomePositions[i][1], atomePositions[i][2]);
		drawAtom(elem::getId(atomTypes[i]));
		glPopMatrix();
		for (int j = 0; j < i; j++)
		{
			exp_bond_length = 
				elem::getExpectedBondLengh(
					elem::getId(atomTypes[i]),
					elem::getId(atomTypes[j])
				);
			actual_bond_length = 
				sqrt(
					pow(atomePositions[i][0] - atomePositions[j][0], 2) +
					pow(atomePositions[i][1] - atomePositions[j][1], 2) +
					pow(atomePositions[i][2] - atomePositions[j][2], 2)
				);
			if (actual_bond_length < exp_bond_length)
			{
				renderCylinder(
					atomePositions[i][0], atomePositions[i][1], atomePositions[i][2],
					atomePositions[j][0], atomePositions[j][1], atomePositions[j][2],
					CYLINDER_RADIUS, myQuad
				);
			}
		}
	}

	glPopMatrix();
	glEndList();

	glutSwapBuffers();
}

void setEnvLight() {
	if (flatColoring) {
		// For flat coloring, disable all lighting
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glShadeModel(GL_FLAT);
	} else if (binaryLighting) {
		// For binary lighting, use a more directional light source
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		GLfloat light_pos[] = {10.0, 10.0, 10.0, 0.0};  // Directional light
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
		
		// Set strong ambient light for binary effect
		GLfloat global_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
		
		// Disable smooth shading for sharper transitions
		glShadeModel(GL_FLAT);
	} else {
		// Normal lighting setup
		if (glcb::lightEffect) {
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
		}
		GLfloat light_pos[] = {5.0, 5.0, 5.0, -1.0};
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
		
		// Default ambient light
		GLfloat global_ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
		
		// Enable smooth shading
		glShadeModel(GL_SMOOTH);
	}
}