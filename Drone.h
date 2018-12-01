#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

typedef struct Drone
{
    GLfloat forbac;
    GLfloat lefrig;
    GLfloat updown;
    GLfloat turn;
    GLfloat props;
    
    
}Drone;

Drone newDrone(GLfloat z, GLfloat x, GLfloat y, GLfloat r, GLfloat p);
void drawDrone(Drone *bird);
void rotateLocal(Drone *bird, GLfloat degree);
