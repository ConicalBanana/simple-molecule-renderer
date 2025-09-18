#include<iostream>
#include<GLUT/glut.h>


// OpenGL callbacks
namespace glcb{
    extern GLdouble width = 1280.0;
    extern GLdouble height = 800.0;   /* window width and height */

    //Viewer options (GluLookAt)
    const float ZNEAR = 1.0;
    const float ZFAR = 100.0;
    const float ORTHO_SCALING = 0.01;

    //Mouse modifiers
    float depth = 8;
    float phi=0, theta=0;
    float downX, downY;
    bool leftButton = false, middleButton = false;

    extern bool lightEffect = 1;
    
    void mouseCallback(int button, int state, int x, int y);
    void motionCallback(int x, int y);
    void keyboardCallback(unsigned char ch, int x, int y);

    void reshape(int w, int h);
    void displayCallback(void);
}

// GL window reshape callback.
void glcb::reshape(int w, int h)
{
	/* save new screen dimensions */
	width = (GLdouble) w;
	height = (GLdouble) h;
	
  	/* tell OpenGL to use the whole window for drawing */
	glViewport(0, 0, (GLsizei) width, (GLsizei) height);
	
  	/* do an orthographic parallel projection with the coordinate
     system set to first quadrant, limited by screen/window size */
	glMatrixMode(GL_PROJECTION);  // See also: https://www.cnblogs.com/justsong/p/11760875.html
	glLoadIdentity();  // replace the current matrix with the identity matrix
	gluOrtho2D(0.0, width, 0.0, height); // define a 2D orthographic projection matrix

	// aspect = width/height;
	glutPostRedisplay();  // marks the current window as needing to be redisplayed.
}

/* Callbacks */
void glcb::mouseCallback(int button, int state, int x, int y)
{
	downX = x; downY = y;
	leftButton = ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN));
	middleButton = ((button == GLUT_MIDDLE_BUTTON) &&  (state == GLUT_DOWN));
}

void glcb::motionCallback(int x, int y)
{
	if (leftButton) //Rotate
	{
		phi += (x-downX)/4.0;
		theta += (downY-y)/4.0;
	}
	if (middleButton) //Scale
	{
		if (depth + (downY - y)/10.0 < ZFAR-10 && depth + (downY - y)/10.0 > ZNEAR+3)
			depth += (downY - y)/10.0;
	}
	downX = x;
	downY = y;
	
	glutPostRedisplay();
}

// exit with [esc] keyboard button
void glcb::keyboardCallback(unsigned char ch, int x, int y)
{
	switch (ch)
	{
		//Esc button will exit app
		case 27:	
			exit(0);
			break;
		// Toggle binary lighting with 'b' key
		case 'b':
		case 'B':
			// binaryLighting = !binaryLighting;
			// if (binaryLighting) flatColoring = false;  // Disable flat when binary is on
			// setEnvLight();  // Update lighting settings
			// std::cout << "Binary lighting: " << (binaryLighting ? "ON" : "OFF") 
			//           << std::endl;
			break;
		// Toggle flat coloring with 'f' key
		case 'f':
		case 'F':
			// flatColoring = !flatColoring;
			// if (flatColoring) binaryLighting = false;  // Disable binary when flat is on
			// setEnvLight();  // Update lighting settings
			// std::cout << "Flat coloring (3D-to-2D): " << (flatColoring ? "ON" : "OFF") 
			//           << std::endl;
			break;
		// Toggle normal lighting with 'l' key
		case 'l':
		case 'L':
			// lightEffect = !lightEffect;
			// std::cout << "Lighting effect: " << (lightEffect ? "ON" : "OFF") 
			//           << std::endl;
			break;
	}
	glutPostRedisplay();
}

void glcb::displayCallback(void)
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(
		-ORTHO_SCALING*width,
		ORTHO_SCALING*width,
		-ORTHO_SCALING*height,
		ORTHO_SCALING*height, 
		ZNEAR,
		ZFAR
	);
	// gluPerspective(fovy, aspect, ZNEAR, ZFAR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//gluLookAt(eyex, eyey, eyez, atx, aty, atz, upx, upy, upz);
	gluLookAt(0, 0, 2, 0, 0, 0, 0, 1, 0);
	
	
	//Motion Options
	glTranslatef(0.0, 0.0, -depth);
	glRotatef(-theta, 1.0, 0.0, 0.0);
	glRotatef(phi, 0.0, 1.0, 0.0);
	
	//Light Effect Toggle :)
	if (lightEffect) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}

	glCallList(1);
	glFlush(); 
}

