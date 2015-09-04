#include "HandModel_Cylinders.h"

#include <QtOpenGL>
#include "GLUT/glut.h"
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"

HandModel_Cylinders::HandModel_Cylinders(){

}

void HandModel_Cylinders::draw(){
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); // saving initial state
    palm();
    glPushMatrix();
        thumb();
    glPopMatrix();
    glPushMatrix();
        finger();
    glPopMatrix();
    glPushMatrix();
        middle();
    glPopMatrix();
    glPushMatrix();
        anular();
    glPopMatrix();
    glPushMatrix();
        pinky();
    glPopMatrix();
    glPopMatrix(); // restoring initial state
}
void HandModel_Cylinders::palm(){
    glPushMatrix(); /// save current matrix so that scaling doesn't effect other shapes
        glColor4f(0.6, 0.0, 0.0, 0.0);
        glScalef(0.8, 0.25, 0.27);
        glutSolidSphere (0.4, 20, 20); // displays ellipsoid by scaling sphere
    glPopMatrix();
    glPushMatrix(); /// save current matrix so that rotation and scaling only affects the cylinder
        glColor4f(0.0, 0.0, 0.6, 0.0);
        glScalef(0.8, 0.4, 0.28);
        glRotatef(-90,1.0f,0.0f,0.0f); // rotation should only affect the cyclinder
        GLUquadricObj *quadric;
        quadric = gluNewQuadric();
        gluCylinder(quadric,0.4f,0.4f,1.0f,20,20);
        gluDeleteQuadric(quadric);
    glPopMatrix(); /// retrieve the state before the cylinder

    glTranslatef(0.0f, 0.4f, 0.0f); // apply translate on the modelview matrix
    // NOW WE ARE AT THE TOP OF THE PALM FROM WHERE THE FINGERS ORIGINATE
    glPushMatrix(); /// save the state because we don't want scaling to effect other shapes
        glColor4f(0.6, 0.0, 0.0, 0.0);
        glScalef(0.8, 0.25, 0.27);
        glutSolidSphere (0.4, 20, 20);
    glPopMatrix();
    // WE ARE STILL AT THE TOP OF THE PALM FROM WHERE THE FINGERS ORIGINATE
}
void HandModel_Cylinders::thumb(){
    /// Sphere 1
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glTranslatef(-0.3f,-0.32f,-0.03f);
    glutSolidSphere (0.08, 20, 20); // r=0.08

    /// Cone 1
    glColor4f(0.0, 0.0, 0.6, 0.0);
    glRotatef(-90,1.0f,0.0f,0.0f); // initial configuration
    glRotatef(-10,0.0f,1.0f,0.0f); // initial configuration
    //glTranslatef(0.0f, 0.0f, 0.015f);
    glRotatef(par_unnorm(THUMB_TILT),0.0f,1.0f,0.0f);
    glRotatef(par_unnorm(THUMB_BEND_0),-1.0f,1.0f,0.0f);/// rotation of thumb cone 1 using parameter tc1
    GLUquadricObj *quadric;
    quadric = gluNewQuadric();
    gluCylinder(quadric,0.08f,0.065f,0.38f,20,20);
    gluDeleteQuadric(quadric);

    /// Sphere 2
    glColor4f(0.0, 0.0, 0.6, 0.0);
    glTranslatef(0.0f, 0.0f, 0.38f);
    glutSolidSphere (0.065, 20, 20);

    /// Cone 2
    glColor4f(0.0, 0.0, 0.6, 0.0);
    //glTranslatef(0.0f, 0.0f, 0.02f);
    glRotatef(par_unnorm(THUMB_BEND_1),0.0f,1.0f,0.0f);/// rotation of thumb cone 1 using parameter tc1
    quadric = gluNewQuadric();
    gluCylinder(quadric,0.065f,0.06f,0.13f,20,20);
    gluDeleteQuadric(quadric);

    /// Sphere 3
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glTranslatef(0.0f, 0.0f, 0.13f);
    glutSolidSphere (0.06, 20, 20);
}
void HandModel_Cylinders::finger(){
    glScalef(0.95f, 1.0f, 1.0f);

    /// Sphere 1
    glTranslatef(-0.25f,0.07f,0.0f);
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glutSolidSphere (0.07, 20, 20); // r=0.07

    /// Cone 1
    glColor4f(0.0, 0.0, 0.6, 0.0);
    glRotatef(-90,1.0f,0.0f,0.0f);
    //glRotatef(-90,1.0f,0.0f,-1.0f);
    //glTranslatef(0.0f, 0.0f, 0.07f);
    glRotatef(par_unnorm(FINGER_TILT),0.0f,1.0f,0.0f);
    glRotatef(par_unnorm(FINGER_BEND_0),-1.0f,0.0f,0.0f);
    GLUquadricObj *quadric;
    quadric = gluNewQuadric();
    gluCylinder(quadric,0.07f,0.067f,0.20f,20,20);
    gluDeleteQuadric(quadric);

    /// Sphere 2
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glTranslatef(0.0f, 0.0f, 0.20f);
    glutSolidSphere (0.067, 20, 20);

    /// Cone 2
    glColor4f(0.0, 0.0, 0.6, 0.0);
    //glTranslatef(0.0f, 0.0f, 0.02f);
    glRotatef(par_unnorm(FINGER_BEND_1),-1.0f,0.0f,0.0f);
    quadric = gluNewQuadric();
    gluCylinder(quadric,0.067f,0.064f,0.16f,20,20);
    gluDeleteQuadric(quadric);

    /// Sphere 3
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glTranslatef(0.0f, 0.0f, 0.16f);
    glutSolidSphere (0.064, 20, 20);

    /// Cone 3
    glColor4f(0.0, 0.0, 0.6, 0.0);
    //glTranslatef(0.0f, 0.0f, 0.02f);
    glRotatef(par_unnorm(FINGER_BEND_2),-1.0f,0.0f,0.0f);
    quadric = gluNewQuadric();
    gluCylinder(quadric,0.064f,0.059f,0.16f,20,20);
    gluDeleteQuadric(quadric);

    /// Sphere 3
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glTranslatef(0.0f, 0.0f, 0.16f);
    glutSolidSphere (0.059, 20, 20);
}
void HandModel_Cylinders::middle(){
    /// Sphere 1
    glTranslatef(-0.08f,0.1f,0.0f);
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glutSolidSphere (0.07, 20, 20);

    /// Cone 1
    glColor4f(0.0, 0.0, 0.6, 0.0);
    glRotatef(-90,1.0f,0.0f,0.0f);
    //glRotatef(-90,1.0f,0.0f,-1.0f);
    //glTranslatef(0.0f, 0.0f, 0.015f);
    glRotatef(par_unnorm(MIDDLE_TILT),0.0f,1.0f,0.0f);
    glRotatef(par_unnorm(MIDDLE_BEND_0),-1.0f,0.0f,0.0f);
    GLUquadricObj *quadric;
    quadric = gluNewQuadric();
    gluCylinder(quadric,0.07f,0.067f,0.20f,20,20);
    gluDeleteQuadric(quadric);

    /// Sphere 2
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glTranslatef(0.0f, 0.0f, 0.20f);
    glutSolidSphere (0.067, 20, 20);

    /// Cone 2
    glColor4f(0.0, 0.0, 0.6, 0.0);
    //glTranslatef(0.0f, 0.0f, 0.02f);
    glRotatef(par_unnorm(MIDDLE_BEND_1),-1.0f,0.0f,0.0f);
    quadric = gluNewQuadric();
    gluCylinder(quadric,0.067f,0.064f,0.17f,20,20);
    gluDeleteQuadric(quadric);

    /// Sphere 3
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glTranslatef(0.0f, 0.0f, 0.17f);
    glutSolidSphere (0.064, 20, 20);

    /// Cone 3
    glColor4f(0.0, 0.0, 0.6, 0.0);
    //glTranslatef(0.0f, 0.0f, 0.02f);
    glRotatef(par_unnorm(MIDDLE_BEND_2),-1.0f,0.0f,0.0f);
    quadric = gluNewQuadric();
    gluCylinder(quadric,0.064f,0.059f,0.16f,20,20);
    gluDeleteQuadric(quadric);

    /// Sphere 3
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glTranslatef(0.0f, 0.0f, 0.16f);
    glutSolidSphere (0.059, 20, 20);
}
void HandModel_Cylinders::anular(){

    glScalef(0.95f, 1.0f, 1.0f);

    /// Sphere 1
    glTranslatef(0.08f,0.1f,0.0f);
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glutSolidSphere (0.07, 20, 20);

    /// Cone 1
    glColor4f(0.0, 0.0, 0.6, 0.0);
    glRotatef(-90,1.0f,0.0f,0.0f);
    //glRotatef(-90,1.0f,0.0f,-1.0f);
    //glTranslatef(0.0f, 0.0f, 0.015f);
    glRotatef(par_unnorm(ANULAR_TILT),0.0f,1.0f,0.0f);
    glRotatef(par_unnorm(ANULAR_BEND_0),-1.0f,0.0f,0.0f);
    GLUquadricObj *quadric;
    quadric = gluNewQuadric();
    gluCylinder(quadric,0.07f,0.067f,0.15f,20,20);
    gluDeleteQuadric(quadric);

    /// Sphere 2
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glTranslatef(0.0f, 0.0f, 0.15f);
    glutSolidSphere (0.067, 20, 20);

    /// Cone 2
    glColor4f(0.0, 0.0, 0.6, 0.0);
    //glTranslatef(0.0f, 0.0f, 0.02f);
    glRotatef(par_unnorm(ANULAR_BEND_1),-1.0f,0.0f,0.0f);
    quadric = gluNewQuadric();
    gluCylinder(quadric,0.067f,0.064f,0.13f,20,20);
    gluDeleteQuadric(quadric);

    /// Sphere 3
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glTranslatef(0.0f, 0.0f, 0.13f);
    glutSolidSphere (0.064, 20, 20);

    /// Cone 3
    glColor4f(0.0, 0.0, 0.6, 0.0);
    //glTranslatef(0.0f, 0.0f, 0.02f);
    glRotatef(par_unnorm(ANULAR_BEND_2),-1.0f,0.0f,0.0f);
    quadric = gluNewQuadric();
    gluCylinder(quadric,0.064f,0.059f,0.16f,20,20);
    gluDeleteQuadric(quadric);

    /// Sphere 3
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glTranslatef(0.0f, 0.0f, 0.16f);
    glutSolidSphere (0.059, 20, 20);
}
void HandModel_Cylinders::pinky(){

    glScalef(0.9f, 1.0f, 1.0f);

    /// Sphere 1
    glTranslatef(0.25f,0.07f,0.0f);
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glutSolidSphere (0.07, 20, 20);

    /// Cone 1
    glColor4f(0.0, 0.0, 0.6, 0.0);
    glRotatef(-90,1.0f,0.0f,0.0f);
    //glRotatef(-90,1.0f,0.0f,-1.0f);
    glRotatef(par_unnorm(PINKY_TILT),0.0f,1.0f,0.0f);
    glRotatef(par_unnorm(PINKY_BEND_0),-1.0f,0.0f,0.0f);
    //glTranslatef(0.0f, 0.0f, 0.015f);
    GLUquadricObj *quadric;
    quadric = gluNewQuadric();
    gluCylinder(quadric,0.07f,0.067f,0.11f,20,20);
    gluDeleteQuadric(quadric);

    /// Sphere 2
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glTranslatef(0.0f, 0.0f, 0.11f);
    glutSolidSphere (0.067, 20, 20);

    /// Cone 2
    glColor4f(0.0, 0.0, 0.6, 0.0);
    //glTranslatef(0.0f, 0.0f, 0.02f);
    glRotatef(par_unnorm(PINKY_BEND_1),-1.0f,0.0f,0.0f);
    quadric = gluNewQuadric();
    gluCylinder(quadric,0.067f,0.064f,0.09f,20,20);
    gluDeleteQuadric(quadric);

    /// Sphere 3
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glTranslatef(0.0f, 0.0f, 0.09f);
    glutSolidSphere (0.064, 20, 20);

    /// Cone 3
    glColor4f(0.0, 0.0, 0.6, 0.0);
    //glTranslatef(0.0f, 0.0f, 0.02f);
    glRotatef(par_unnorm(PINKY_BEND_2),-1.0f,0.0f,0.0f);
    quadric = gluNewQuadric();
    gluCylinder(quadric,0.064f,0.059f,0.07f,20,20);
    gluDeleteQuadric(quadric);

    /// Sphere 3
    glColor4f(0.0, 0.6, 0.0, 0.0);
    glTranslatef(0.0f, 0.0f, 0.07f);
    glutSolidSphere (0.059, 20, 20);
}

