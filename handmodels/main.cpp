#include <QApplication>
#include <QMainWindow>
#include <qglviewer.h>
#include "HandModel.h"
#include "HandModel_Cylinders.h"
#include "QAutoGUI.h"
#include <QDockWidget>

class Viewer : public QGLViewer{
public:
    Viewer(Hand& hand):hand(hand){
        /// forbids spinning
        camera()->frame()->setSpinningSensitivity(100);
    }
private:
    Hand& hand;
    void draw(){
        glClearColor (1.0, 1.0, 1.0, 0.0);
        glShadeModel (GL_SMOOTH);
        glEnable(GL_DEPTH_TEST);
        hand.draw();
    }
};

int main(int argc, char* argv[]){
    QApplication app(argc, argv);
    QMainWindow main;

    /// Hand models
    HandModel_Cylinders hand;

    /// Setup viewer
    Viewer* view = new Viewer(hand);
    main.setCentralWidget(view);

    /// Adjust w.r.t. hand size
    // view->camera()->fitBoundingBox();

    /// Hook parameters to GUI
    Parameters p;
    {
        ParameterGroup& group = ParameterGroup::New("Finger");
        group << DoubleSliderEdit::New(hand.par(Hand::FINGER_BEND_0), hand.name(Hand::FINGER_BEND_0)).connect_to(view, SLOT(updateGL()));
        group << DoubleSliderEdit::New(hand.par(Hand::FINGER_BEND_1), hand.name(Hand::FINGER_BEND_1)).connect_to(view, SLOT(updateGL()));
        group << DoubleSliderEdit::New(hand.par(Hand::FINGER_BEND_2), hand.name(Hand::FINGER_BEND_2)).connect_to(view, SLOT(updateGL()));
        group << DoubleSliderEdit::New(hand.par(Hand::FINGER_TILT), hand.name(Hand::FINGER_TILT)).connect_to(view, SLOT(updateGL()));
        p << group;
    }
    {
        ParameterGroup& group = ParameterGroup::New("Middle");
        group << DoubleSliderEdit::New(hand.par(Hand::MIDDLE_BEND_0), hand.name(Hand::MIDDLE_BEND_0)).connect_to(view, SLOT(updateGL()));
        group << DoubleSliderEdit::New(hand.par(Hand::MIDDLE_BEND_1), hand.name(Hand::MIDDLE_BEND_1)).connect_to(view, SLOT(updateGL()));
        group << DoubleSliderEdit::New(hand.par(Hand::MIDDLE_BEND_2), hand.name(Hand::MIDDLE_BEND_2)).connect_to(view, SLOT(updateGL()));
        group << DoubleSliderEdit::New(hand.par(Hand::MIDDLE_TILT), hand.name(Hand::MIDDLE_TILT)).connect_to(view, SLOT(updateGL()));
        p << group;
    }
    {
        ParameterGroup& group = ParameterGroup::New("Anular");
        group << DoubleSliderEdit::New(hand.par(Hand::ANULAR_BEND_0), hand.name(Hand::ANULAR_BEND_0)).connect_to(view, SLOT(updateGL()));
        group << DoubleSliderEdit::New(hand.par(Hand::ANULAR_BEND_1), hand.name(Hand::ANULAR_BEND_1)).connect_to(view, SLOT(updateGL()));
        group << DoubleSliderEdit::New(hand.par(Hand::ANULAR_BEND_2), hand.name(Hand::ANULAR_BEND_2)).connect_to(view, SLOT(updateGL()));
        group << DoubleSliderEdit::New(hand.par(Hand::ANULAR_TILT), hand.name(Hand::ANULAR_TILT)).connect_to(view, SLOT(updateGL()));
        p << group;
    }
    {
        ParameterGroup& group = ParameterGroup::New("Pinky");
        group << DoubleSliderEdit::New(hand.par(Hand::PINKY_BEND_0), hand.name(Hand::PINKY_BEND_0)).connect_to(view, SLOT(updateGL()));
        group << DoubleSliderEdit::New(hand.par(Hand::PINKY_BEND_1), hand.name(Hand::PINKY_BEND_1)).connect_to(view, SLOT(updateGL()));
        group << DoubleSliderEdit::New(hand.par(Hand::PINKY_BEND_2), hand.name(Hand::PINKY_BEND_2)).connect_to(view, SLOT(updateGL()));
        group << DoubleSliderEdit::New(hand.par(Hand::PINKY_TILT), hand.name(Hand::PINKY_TILT)).connect_to(view, SLOT(updateGL()));
        p << group;
    }
    {
        ParameterGroup& group = ParameterGroup::New("Thumb");
        group << DoubleSliderEdit::New(hand.par(Hand::THUMB_BEND_0), hand.name(Hand::THUMB_BEND_0)).connect_to(view, SLOT(updateGL()));
        group << DoubleSliderEdit::New(hand.par(Hand::THUMB_BEND_1), hand.name(Hand::THUMB_BEND_1)).connect_to(view, SLOT(updateGL()));
        group << DoubleSliderEdit::New(hand.par(Hand::THUMB_TILT), hand.name(Hand::THUMB_TILT)).connect_to(view, SLOT(updateGL()));
        p << group;
    }

    /// Show controller widget
    ParametersDockWidget dockwidget;
    dockwidget.setWindowTitle("Hand model parameters tester");
    dockwidget.load_parameters(p);
    main.addDockWidget(Qt::RightDockWidgetArea,&dockwidget);

    /// Show gui and start event loop
    main.show();
    return app.exec();
}
