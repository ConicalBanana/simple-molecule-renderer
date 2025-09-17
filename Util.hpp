#include<GLUT/glut.h>
#include<cmath>


//SRC: http://lifeofaprogrammergeek.blogspot.com/2008/07/rendering-cylinder-between-two-points.html
//will make a cylender between 2 pts :D
void renderCylinder(float x1, float y1, float z1, float x2,float y2, float z2, float radius, GLUquadricObj *quadric)
{
	float vx = x2-x1;
	float vy = y2-y1;
	float vz = z2-z1;
	float ax,rx,ry,rz;
	float len = sqrt( vx*vx + vy*vy + vz*vz );
	
	glPushMatrix();
	glTranslatef( x1,y1,z1 );
	if (fabs(vz) < 0.0001)
	{
		glRotatef(90, 0,1,0);
		ax = 57.2957795*-atan( vy / vx );
		if (vx < 0)
		{
			
		}
		rx = 1;
		ry = 0;
		rz = 0;
	}
	else
	{
		ax = 57.2957795*acos( vz/ len );
		if (vz < 0.0)
			ax = -ax;
		rx = -vy*vz;
		ry = vx*vz;
		rz = 0;
	}
	glRotatef(ax, rx, ry, rz);
	gluQuadricOrientation(quadric,GLU_OUTSIDE);
	gluCylinder(quadric, radius, radius, len, slices, stacks);
	glPopMatrix();
}
