#include "KinectHelper.h"
//#define DEBUG_QUEUE
#ifdef DEBUG_QUEUE
    #undef DEBUG_QUEUE
    #define DEBUG_QUEUE if(1)
#else
    #undef DEBUG_QUEUE
    #define DEBUG_QUEUE if(0)
#endif

KinectHelper::KinectHelper(QObject *parent) : QObject(parent){
    /// Initialize and open device, create depth stream
    /// NOTE: by doing this in the constructor it remains in the main thread,
    /// so the plugin will lock until Kinect is initialized
    bool success = initialize_device();
    if(!success)
        qDebug()<<"Failed to initialize Kinect";

    /// GUI Elements
    color_label = NULL;
    depth_label = NULL;

    /// Instantiate listeners
    depthListener = new ModeKinectListener(this);
    colorListener = new ModeKinectListener(this);
    depth.addNewFrameListener(depthListener);
    color.addNewFrameListener(colorListener);

    /// Buffer pointers always valid
    points_front_buffer = &points_buffer_1;
    points_back_buffer  = &points_buffer_2;
    color_front_buffer = &color_buffer_1;
    color_back_buffer = &color_buffer_2;
    depth_front_buffer = &depth_buffer_1;
    depth_back_buffer = &depth_buffer_2;

    /// Avoid displaying stuff when data is not ready
    has_consumed_first_frame = false;

    ///Initialize the data sizes
    {
        VideoFrameRef frame;
        Status status = depth.readFrame(&frame);
        if(!status == STATUS_OK)
            throw StarlabException("Failed to initialize Kinect");

        /// Allocate memory
        points_buffer_1.resize(frame.getHeight(), frame.getWidth());
        points_buffer_2.resize(frame.getHeight(), frame.getWidth());
        color_buffer_1 = QImage(frame.getWidth(),frame.getHeight(),QImage::Format_RGB888);
        color_buffer_2 = QImage(frame.getWidth(),frame.getHeight(),QImage::Format_RGB888);
        depth_buffer_1 = QImage(frame.getWidth(),frame.getHeight(),QImage::Format_RGB888);
        depth_buffer_2 = QImage(frame.getWidth(),frame.getHeight(),QImage::Format_RGB888);

        /// Communicate the viewport to the GUI
        compute_frame_bbox(frame);
        emit scene_bbox_updated(_bbox);
    }

    /// Allow passing VideoFrameRefs
    qRegisterMetaType<VideoFrameRef>("VideoFrameRef");///< Allow pass reference frame as signals
}

BBox3 KinectHelper::compute_frame_bbox(VideoFrameRef frame){
    // qDebug() << "KinectThread::setBoundingBox(VideoFrameRef frame)";

    /// Get depth data from the frame just fetched
    const openni::DepthPixel* pDepth = (const openni::DepthPixel*)frame.getData();
    float wx,wy,wz;
    BBox3 box; box.setNull();
    for (int y = 0; y<frame.getHeight(); ++y){
        for(int x = 0; x<frame.getWidth(); ++x){
            DepthPixel d = *pDepth;
            pDepth++;
            CoordinateConverter::convertDepthToWorld(depth, x, y, d, &wx, &wy, &wz);
            box.extend( Vector3(wx,wy,wz) );
        }
    }
    this->_bbox = box;
}

void KinectHelper::start(){
    connect(depthListener, SIGNAL(new_frame_available(VideoFrameRef)), this, SLOT(updateDepthFrame(VideoFrameRef)));
    connect(colorListener, SIGNAL(new_frame_available(VideoFrameRef)), this, SLOT(updateColorFrame(VideoFrameRef)));
    connect(depthListener, SIGNAL(new_frame_available(VideoFrameRef)), this, SLOT(consume()));
    connect(colorListener, SIGNAL(new_frame_available(VideoFrameRef)), this, SLOT(consume()));
}

void KinectHelper::drawCloud(){
    // qDebug() << "KinectHelper::drawCloud()" << QThread::currentThreadId();
    Q_ASSERT(!mutex()->tryLock());
    if(!data_ready()) return;

    /// The lock allows us to just use a reference to the buffer
    PImage& data = (*points_front_buffer);
    glDisable(GL_LIGHTING);
    glColor3d(1.0,0.0,0.0);
    glBegin(GL_POINTS);
    for(int c=0; c<data.cols(); c++)
        for(int r=0; r<data.rows(); r++)
            glVertex3dv( data(r,c).data() );
    glEnd();
    glEnable(GL_LIGHTING);
}

void KinectHelper::drawColor(){
    //Q_ASSERT(!mutex()->tryLock());

    /// Don't bother drawing empty image
    if(!data_ready())
        return;

    /// Have you setColorLabel?
    if(color_label==NULL)
        return;

    color_label->setPixmap(QPixmap::fromImage(*color_front_buffer));
}
void KinectHelper::drawDepth(){
    //Q_ASSERT(!mutex()->tryLock());

    /// Don't bother drawing empty image
    if(!data_ready())
        return;

    /// Have you setColorLabel?
    if(depth_label==NULL)
        return;

    depth_label->setPixmap(QPixmap::fromImage(*depth_front_buffer));

    if(depth_label_2==NULL)
        return;

    depth_label_2->setPixmap(QPixmap::fromImage(*depth_front_buffer));
}
void KinectHelper::updateDepthFrame(openni::VideoFrameRef frame){
    /// Fetch new depth frame from the frame listener class
    DEBUG_QUEUE qDebug() << "queued depth frame#" << frame.getFrameIndex();
    depthQueue.enqueue(frame);
}

void KinectHelper::updateColorFrame(openni::VideoFrameRef frame){
    DEBUG_QUEUE qDebug() << "queued color frame#" << frame.getFrameIndex();
    colorQueue.enqueue(frame);
}

void KinectHelper::queuesStatus(){
    int depthItem = depthQueue.empty() ? -1 : depthQueue.head().getFrameIndex();
    int colorItem = colorQueue.empty() ? -1 : colorQueue.head().getFrameIndex();
    qDebug() << "SIZE D#" << depthQueue.size() << "C#" << colorQueue.size();
    qDebug() << "HEAD D#" << depthItem << "C#" << colorItem;
}

void KinectHelper::consume(){
    // qDebug() << "KinectThread::consume()" << QThread::currentThreadId();

    if(depthQueue.empty() || colorQueue.empty()) return;

    DEBUG_QUEUE queuesStatus();

    while( (!depthQueue.empty()) && colorQueue.head().getFrameIndex()>depthQueue.head().getFrameIndex() ){
        DEBUG_QUEUE qDebug() << "dQueue pop" << depthQueue.head().getFrameIndex();
        depthQueue.dequeue();
    }

    if(depthQueue.empty() || colorQueue.empty()) return;

    while( (!colorQueue.empty()) && depthQueue.head().getFrameIndex()>colorQueue.head().getFrameIndex() ){
        DEBUG_QUEUE qDebug() << "cQueue pop" << colorQueue.head().getFrameIndex();
        colorQueue.dequeue();
    }

    if(depthQueue.empty() || colorQueue.empty()) return;

    DEBUG_QUEUE qDebug() << "CONSUMED";
    DEBUG_QUEUE qDebug() << "depth frame#" << depthQueue.head().getFrameIndex();
    DEBUG_QUEUE qDebug() << "color frame#" << colorQueue.head().getFrameIndex();

    /// Finally consume
    consume_depth( depthQueue.dequeue() );
    consume_color( colorQueue.dequeue() );
    has_consumed_first_frame = true;
}

void KinectHelper::consume_depth(VideoFrameRef frame){
    /// Get depth data from the frame just fetched
    const openni::DepthPixel* pDepth = (const openni::DepthPixel*)frame.getData(); // for point cloud
    const openni::DepthPixel* iDepth = (const openni::DepthPixel*)frame.getData(); // for depth image

    /// Depth Image
    (*depth_back_buffer) = QImage(frame.getWidth(), frame.getHeight(), QImage::Format_RGB32);

    for (unsigned nY=0; nY<frame.getHeight(); nY++)
    {
        uchar *imageptr = (*depth_back_buffer).scanLine(nY);
        for (unsigned nX=0; nX < frame.getWidth(); nX++)
        {
            unsigned depth = *iDepth;
            unsigned maxdist=10000;
            if(depth>maxdist) depth=maxdist;
            if(depth)
            {
                depth = (depth*255)/maxdist;
                //depth = (maxdist-depth)*255/maxdist+1;
            }
            // depth: 0: invalid
            // depth: 255: closest
            // depth: 1: furtherst (maxdist distance)
            // Here we could do depth*color, to show the colored depth
            imageptr[0] = depth;
            imageptr[1] = depth;
            imageptr[2] = depth;
            imageptr[3] = 0xff;
            iDepth++;
            imageptr+=4;
        }
    }


    _mutex.lock();
    qSwap(depth_front_buffer, depth_back_buffer);
    _mutex.unlock();


    /// Point Cloud

    float wx,wy,wz;

    /// NaN values must be handled well and not displayed by OpenGL
    for (int y = 0; y<frame.getHeight(); ++y){
        for(int x = 0; x<frame.getWidth(); ++x){
            DepthPixel d = *pDepth;
            pDepth++;
            /// Convert depth coordinates to world coordinates to remove camera intrinsics
            CoordinateConverter::convertDepthToWorld(depth, x, y, d, &wx, &wy, &wz);
            /// Change the coordinates of the vertices in the model
            (*points_back_buffer)(y,x) = Vector3(wx,wy,wz);
        }
    }

    /// Now swap front and back buffers
    _mutex.lock();
        qSwap(points_front_buffer, points_back_buffer);
    _mutex.unlock();
}

void KinectHelper::consume_color(VideoFrameRef frame){
    /// Fetch new color frame from the frame listener class

    /// Get color data from the frame just fetched
    const openni::RGB888Pixel* imageBuffer = (const openni::RGB888Pixel*) frame.getData();

    (*color_back_buffer) = QImage((uchar*) imageBuffer, frame.getWidth(), frame.getHeight(), QImage::Format_RGB888);

    _mutex.lock();
    qSwap(color_front_buffer, color_back_buffer);
    _mutex.unlock();
}

/// @todo exceptions instead of qDebug()
int KinectHelper::initialize_device(){

    rc = STATUS_OK;

    /// Fetch the device URI to pass to Device::open()
    const char* deviceURI = ANY_DEVICE;

    /// Initialize the device
    rc = OpenNI::initialize();
    if(rc!=openni::STATUS_OK)
    {
        qDebug()<<"Initialization Errors (if any): "<<OpenNI::getExtendedError();
        OpenNI::shutdown();
        return false;
    }


    /// Open the device using the previously fetched device URI
    rc = device.open(deviceURI);
    if (rc != openni::STATUS_OK)
    {
        qDebug()<<"Device open failed: "<<openni::OpenNI::getExtendedError();
        OpenNI::shutdown();
        return false;
    }

    /// Create the depth stream
    rc = depth.create(device, openni::SENSOR_DEPTH);

    if (rc == openni::STATUS_OK)
    {
        /// start the depth stream, if its creation was successful
        rc = depth.start();

        if (rc != openni::STATUS_OK)
        {
            qDebug()<<"Couldn't start depth stream: "<<openni::OpenNI::getExtendedError();
            depth.destroy();
            return false;
        }
    }
    else
    {
        qDebug()<<"Couldn't find depth stream: "<<openni::OpenNI::getExtendedError();
        return false;
    }

    if (!depth.isValid())
    {
        qDebug()<<"No valid depth streams. Exiting";
        OpenNI::shutdown();
        return false;
    }

    /// Storing depth intrinsic parameters
    intrinsic_depth_fx = 1.0/tan(depth.getHorizontalFieldOfView()*0.5)/(depth.getVideoMode().getResolutionX()*0.5);
    intrinsic_depth_fy = 1.0/tan(depth.getVerticalFieldOfView()*0.5)/(depth.getVideoMode().getResolutionY()*0.5);
    /// lets consider storing only field of view
    //fovY = depth.getVerticalFieldOfView(); // returns vertical field of view in radians
    //aspectRatio = ((float)depth.getVideoMode().getResolutionX())/depth.getVideoMode().getResolutionY();


    /// Create the color stream
    rc = color.create(device, openni::SENSOR_COLOR);

    if (rc == openni::STATUS_OK)
    {
        /// start the color stream, if its creation was successful
        rc = color.start();

        if (rc != openni::STATUS_OK)
        {
            qDebug()<<"Couldn't start color stream: "<<openni::OpenNI::getExtendedError();
            color.destroy();
            return false;
        }
    }
    else
    {
        qDebug()<<"Couldn't find color stream: "<<openni::OpenNI::getExtendedError();
        return false;
    }

    if (!color.isValid())
    {
        qDebug()<<"No valid color streams. Exiting";
        OpenNI::shutdown();
        return false;
    }
#ifndef NOSYNC
    /// Enable depth/color frame synchronization
    rc = device.setDepthColorSyncEnabled(true);
    if (rc != openni::STATUS_OK)
    {
        qDebug()<<"Could not synchronise device";
        OpenNI::shutdown();
        return false;
    }
#endif

    /// Depth image is transformed to have the same apparent vantage point as the RGB image
    rc = device.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR);
    if (rc != openni::STATUS_OK)
    {
        qDebug()<<"Could not set Image Registration Mode";
        OpenNI::shutdown();
        return false;
    }

    return true;
}
