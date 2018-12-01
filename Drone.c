#include "Drone.h"


Drone newDrone(GLfloat z, GLfloat x, GLfloat y, GLfloat r, GLfloat p) {
    
    Drone bird;
    bird.forbac = z;
    bird.lefrig = x;
    bird.updown = y;
    bird.turn = r;
    bird.props = p;
    
    return bird;
}

void rotateLocal(Drone *bird, GLfloat degree){
    glTranslatef(bird->lefrig, bird->updown, bird->forbac);
    glRotatef(degree, 0.0, 1.0, 0.0);
    glTranslatef(-1*bird->lefrig,-1*bird->updown,-1*bird->forbac);
}

void drawDrone(Drone *bird){
    glPushMatrix();// Whole drone
    
    rotateLocal(bird, bird->turn); //CTM = T(1)*R_y(turn)*T(-1)
    glTranslatef(bird->lefrig, bird->updown, bird->forbac);
    //CTM = T(1)*R_y(turn)*T(-1)*T(lefrig,forbac)
    
    
    // Apply transformations to construct drone, modify this!
    
    
    //Copy CTM
    //CTM =T(1)*R_y(turn)*T(-1)*T(lefrig,forbac)*T4*S(1.5)
    // p' = CTM*p
    glPushMatrix(); //Camera on front of Drone
    
    glTranslatef(0.0,4.0,4.0);
    glScalef(1.5,1.5,1.5);
    
    glutSolidSphere(1.0,50,50);
    
    glPopMatrix();//Restore matrix
    glColor3f(1.0,0.0,0.0);
    //Copy CTM
    //CTM = T(1)*R_y(turn)*T(-1)*T(lefrig,forbac)
    //CTM = T(1)*R_y(turn)*T(-1)*T(lefrig,forbac)*T(4)*R_y(90)*S(5,2,2)
    // p' = CTM*p
    glPushMatrix(); //Body
    glTranslatef(0.0, 4.0, 0.0);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glScalef(5.0, 2.0, 2.0);
    glutSolidSphere(1.0, 30, 30); //Drone Body
    glPopMatrix(); //Body
    //Restore CTM so that CTM = T(1)*R_y(turn)*T(-1)*T(lefrig,forbac)
    
    //Copy CTM
    // CTM = T(1)*R_y(turn)*T(-1)*T(lefrig,forbac)
    //CTM = T(1)*R_y(turn)*T(-1)*T(lefrig,forbac)*R_y(45)*R_y(90*j)
    glPushMatrix(); //Separate arms from body
    glRotatef(45.0,0.0,1.0,0.0);
    
    for(int j=0; j<4; j++){ //Make all drone arms at once
        glRotatef(90.0*j,0.0,1.0,0.0);
        glPushMatrix();                                             //Copy CTM, CTM = T(1)*R_y(turn)*T(-1)*T(lefrig,forbac)*R_y(45)*R_y(90*j)
        glTranslatef(3.0, 4.0,0.0); //Lift off ground           //CTM = T(1)*R_y(turn)*T(-1)*T(lefrig,forbac)*R_y(45)*R_y(90*j)*T(3,4,0)
        glPushMatrix();                                         //Copy, CTM = (1)*R_y(turn)*T(-1)*T(lefrig,forbac)*R_y(45)*R_y(90*j)*T(3,4,0)
        glTranslatef(3.0,0.0,0.0); //End of Prop            //CTM = (1)*R_y(turn)*T(-1)*T(lefrig,forbac)*R_y(45)*R_y(90*j)*T(3,4,0)*T(3,0,0)
        glPushMatrix();                                     //Copy, CTM = (1)*R_y(turn)*T(-1)*T(lefrig,forbac)*R_y(45)*R_y(90*j)*T(3,4,0)*T(3,0,0)
        glTranslatef(0.0,0.5,0.0); //Top of peg         // CTM = (1)*R_y(turn)*T(-1)*T(lefrig,forbac)*R_y(45)*R_y(90*j)*T(3,4,0)*T(3,0,0)*T(0,0.5,0)
        glPushMatrix();                                 //Copy, CTM = (1)*R_y(turn)*T(-1)*T(lefrig,forbac)*R_y(45)*R_y(90*j)*T(3,4,0)*T(3,0,0)*T(0,0.5,0)
        glRotatef(bird->props,0.0,1.0,0.0);  // CTM = (1)*R_y(turn)*T(-1)*T(lefrig,forbac)*R_y(45)*R_y(90*j)*T(3,4,0)*T(3,0,0)*T(0,0.5,0)*R_y(props)*T(0.35)*S(4.5,0.15,1.0)
        glTranslatef(0.0,0.35,0.0);
        glScalef(4.5,0.15,1.0);
        glutSolidCube(1.0); // Propeller                // p' = CTM*p
        glPopMatrix();                                 //Restore CTM
        glScalef(0.25,0.75,0.25);                           // CTM = (1)*R_y(turn)*T(-1)*T(lefrig,forbac)*R_y(45)*R_y(90*j)*T(3,4,0)*T(3,0,0)*T(0,0.5,0)*S(0.25,0.75,0.25)
        glutSolidCube(1.0); // Peg                          // p' = CTM*p
        glPopMatrix();                               // Restore, CTM = (1)*R_y(turn)*T(-1)*T(lefrig,forbac)*R_y(45)*R_y(90*j)*T(3,4,0)*T(3,0,0)
        glutSolidCube(1.0); //Motor                       // p' = CTM*p
        glPopMatrix();                                      // Restore, CTM = (1)*R_y(turn)*T(-1)*T(lefrig,forbac)*R_y(45)*R_y(90*j)*T(3,4,0)
        glScalef(5.0, 0.75, 0.75);                                 // CTM = (1)*R_y(turn)*T(-1)*T(lefrig,forbac)*R_y(45)*R_y(90*j)*T(3,4,0)*S(5,0.75,0.75)
        glutSolidCube(1.0); //Prop                              // p' = CTM*p
        glPopMatrix();                                      //Restore, CTM = CTM = T(1)*R_y(turn)*T(-1)*T(lefrig,forbac)*R_y(45)*R_y(90*j)
    }
    
    glPopMatrix(); // End arms                                  // Restore, CTM = T(1)*R_y(turn)*T(-1)*T(lefrig,forbac)
    
    glPopMatrix(); //Transformations                    // Restore, CTM = null
}
