#include <QApplication>
#include <QLayout>
#include "WorkHelper.h"
#include "HandProjection.h"

int main(int argc, char* argv[]){
    QApplication app(argc, argv);
    try{
        /// setup a main window with widget
        QMainWindow* window = new QMainWindow;
        QWidget* widget = new QWidget(window);
        QHBoxLayout *layout = new QHBoxLayout;
        widget->setLayout(layout);
        window->setCentralWidget(widget);

        /// initialize kinect -> GUI freezes till Kinect is initialised
        KinectHelper khelper;

        /// Setup fixed framerate
        QTimer timer;
        timer.setSingleShot(false);
        const int FPS = 30;
        timer.setInterval((1.0/FPS)*1000.0);

        /// visualize color buffer
        if(false){
            QLabel* color_label = new QLabel();
            layout->addWidget(color_label);
            khelper.setColorLabel(color_label);
            QObject::connect(&timer, SIGNAL(timeout()), &khelper, SLOT(drawColor()), Qt::DirectConnection );
        }

        /// depth buffer
        if(true){
            QLabel* depth_label = new QLabel();
            layout->addWidget(depth_label);
            /// @todo !!
            khelper.setDepthLabel(depth_label);
            QObject::connect(&timer, SIGNAL(timeout()), &khelper, SLOT(drawDepth()), Qt::DirectConnection );
        }

        /// QGLViewer
        if(true){
            HandProjection* handView = new HandProjection(NULL);
            handView->setIntrinsics(khelper.getIntrinsicDepthFx(), khelper.getIntrinsicDepthFy());
            handView->resize(800,800);
            handView->show();
        }


        /// Hook up worker
        WorkHelper whelper(khelper);
        QObject::connect(&timer, SIGNAL(timeout()), &whelper, SLOT(work()) );

        /// move kinect computation to thread
        QThread* k_thread = new QThread(&khelper);
        khelper.moveToThread(k_thread);
        k_thread->start();

        /// Start processing
        timer.start();
        khelper.start();

        /// Show
        window->show();

        /// Starts the GUI event loop
        return app.exec();

    } catch(...) {
        exit(0);
    }
}
