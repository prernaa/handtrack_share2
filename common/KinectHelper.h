#pragma once
#include <QThread>
#include <QtConcurrentRun>
#include <QDebug>
#include <QtOpenGL>
#include <QQueue>

#include "OpenNI.h"
#include "Types.h"

using namespace openni;

/// FD
class ModeKinectListener;
class KinectWidget;

class KinectHelper : public QObject{
    Q_OBJECT

/// @{ data
public:
    typedef Eigen::Vector3d Point;
    typedef Eigen::Matrix<Point, Eigen::Dynamic, Eigen::Dynamic> PImage; ///< an image of Points
private:
    typedef QQueue<VideoFrameRef> DepthQueue;
    typedef QQueue<VideoFrameRef> ColorQueue;

private:
    BBox3 _bbox;
public:
    BBox3 bbox(){ return _bbox; }
    QMutex* mutex(){ return &_mutex; }
private:
    QTimer* timer;
    QMutex _mutex;
    PImage* points_front_buffer; ///< &cloud_buffer_1
    PImage* points_back_buffer;  ///< &cloud_buffer_2
    PImage points_buffer_1;
    PImage points_buffer_2;

    QImage* color_front_buffer; ///< &color_buffer_1
    QImage* color_back_buffer;  ///< &color_buffer_2
    QImage color_buffer_1;
    QImage color_buffer_2;

    QImage* depth_front_buffer; ///< &depth_buffer_1
    QImage* depth_back_buffer;  ///< &depth_buffer_2
    QImage depth_buffer_1;
    QImage depth_buffer_2;
public:
    /// Lock mutex before accessing this resource!!
    QImage& colorBuffer(){ return *color_front_buffer; }
    /// Lock mutex before accessing this resource!!
    QImage& depthBuffer(){ return *depth_front_buffer; }
    /// Lock mutex before accessing this resource!!
    PImage& pointBuffer(){ return *points_front_buffer; }

protected:
    Status rc;
    Device device;
    VideoStream depth, color;
    ModeKinectListener* depthListener;
    ModeKinectListener* colorListener;


/// @}

/// @{
    bool has_consumed_first_frame; ///< false
    bool data_ready(){ return has_consumed_first_frame; }
/// @}
///
/// @{ constructor/destructor
public:
    KinectHelper(QObject* parent=0);
    ~KinectHelper(){
        //depth.removeNewFrameListener(depthListener);
        //color.removeNewFrameListener(colorListener);
        depth.stop();
        color.stop();
        depth.destroy();
        color.destroy();
        device.close();
        OpenNI::shutdown();
    }
    void start();
private:
    int initialize_device();
/// @}

/// @{
public slots:
    /// called whenever a new depth frame is received. This function moves the
    /// vertices and modifies the mesh
    void updateDepthFrame(VideoFrameRef frame);
    /// called whenever a new color frame is received. It converts the color frame
    /// into a QImage which is passed to the main (GUI) thread through a signal
    void updateColorFrame(VideoFrameRef frame);
public slots:
    /// slot to consume the queue elements
    void consume();
private:
    void queuesStatus(); ///< debugging helper
    DepthQueue   depthQueue;
    ColorQueue   colorQueue;
    void consume_depth(VideoFrameRef frame);
    void consume_color(VideoFrameRef frame);
/// @}

/// @{ hooks to be used in client application
private:
    BBox3 compute_frame_bbox(VideoFrameRef frame);
public slots:
    /// Draws the point cloud in OpenGL
    void drawCloud();
signals:
    /// Updates the bounding box
    void scene_bbox_updated(BBox3 box);
/// @}


/// @{ Color GUI support
public slots:
    /// call this from the main GUI thread
    void drawColor();
    void drawDepth();
public:
    /// Sets label that "drawLabel" slot will work on
    void setColorLabel(QLabel* label){ this->color_label = label; }
    void setDepthLabel(QLabel* label){ this->depth_label = label; }
    void setDepthLabel2(QLabel* label){ this->depth_label_2 = label; }
private:
    QLabel* color_label; ///< NULL
    QLabel* depth_label; ///< NULL
    QLabel* depth_label_2; ///< NULL
/// @}

/// Kinect camera intrinsics
private:
    float intrinsic_depth_fx;
    float intrinsic_depth_fy;
    //float fovY;
    //float aspectRatio;
public:
    float getIntrinsicDepthFx(){return intrinsic_depth_fx;}
    float getIntrinsicDepthFy(){return intrinsic_depth_fy;}
    //float getIntrinsicFovY(){return fovY;}
    //float getIntrinsicAspectRatio(){return aspectRatio;}
};


/// The Frame Listener class emits a signal whenever it receives a new frame
class ModeKinectListener : public QObject, public VideoStream::NewFrameListener{
    Q_OBJECT
public:
    ModeKinectListener(QObject* parent) : QObject(parent){}
private:
    VideoFrameRef newFrame;
public:
    void onNewFrame(VideoStream& stream){
        stream.readFrame(&newFrame);
        /// Emits a signal whenever a new frame is available
        emit new_frame_available(newFrame);
    }
signals:
    void new_frame_available(VideoFrameRef);
};
