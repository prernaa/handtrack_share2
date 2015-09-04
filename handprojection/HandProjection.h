#include <QtOpenGL/QGLWidget>
#include <QtOpenGL>
#include "GLUT/glut.h"
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#include "HandModel_Cylinders.h"
#include "HandModel.h"

class HandProjection : public QGLWidget
{
    Q_OBJECT
    HandModel_Cylinders hand;
private:
    //float intrinsic_fovy;
    //float intrinsic_aspectratio;
    float intrinsic_fx;
    float intrinsic_fy;

public:
    HandProjection(QWidget *parent = NULL) : QGLWidget(parent){}
    void setIntrinsics(float fx, float fy){intrinsic_fx=fx; intrinsic_fy=fy;}

protected:
    void initializeGL() {
        //glDisable(GL_DEPTH_TEST);
        glEnable(GL_DEPTH_TEST);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(1.0, 1.0, 1.0, 0);
    }

    void resizeGL(int w, int h) {
#if 0
#endif
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
    void paintGL() {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glColor3f(1.0,1.0,1.0);
        hand.draw();
    }
};
