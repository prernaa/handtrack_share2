#include <QApplication>
#include <QMainWindow>
#include <qglviewer.h>
#include <QDockWidget>
#include "../handmodels/HandModel.h" ///< move to common

#include "SurfaceMesh/SurfaceMesh.h"
#include "SurfaceMesh/Renderer/Smooth.h"
#include "SurfaceMesh/Renderer/Wireframe.h"
#include "SurfaceMesh/IO/Obj.h"

class Viewer : public QGLViewer{
public: 
    Hand& hand;
    
public:
    Viewer(Hand& hand): hand(hand){
        /// forbids spinning
        camera()->frame()->setSpinningSensitivity(100);
    }
private:
    void init(){
        glClearColor(1.0, 1.0, 1.0, 0.0);
        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
        hand.init();
    }

    void draw(){
        glColor3d(.5,.5,.5);
        hand.draw();
    }
};

class HandModel_Mesh : public Hand, public SurfaceMesh{
public:
    Renderer::Wireframe renderer_wireframe;
    Renderer::Smooth    renderer_smooth;
    RendererBase& renderer;
    void init(){ renderer.init(); }    
    void draw(){ renderer.render(); }
public:
    HandModel_Mesh() : 
        renderer_wireframe(*this),
        renderer_smooth(*this),
        renderer(renderer_smooth){}
};

int main(int argc, char* argv[]){
    QApplication app(argc, argv);
    QMainWindow main;

    /// Hand models
    HandModel_Mesh hand;
    SurfaceMeshIOObj(hand).read("testhand.obj");
    // SurfaceMeshIOObj(hand).write("testhand.obj");
    
    /// Setup viewer
    Viewer* view = new Viewer(hand);
    main.setCentralWidget(view);
 
    /// Show gui and start event loop
    main.show();
    return app.exec();
}

