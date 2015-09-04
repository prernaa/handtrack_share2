#include "NiteHelper.h"

NiteHelper::NiteHelper(QObject *parent) : KinectHelper(parent)
{
    bool success = initialize_device();
    if(!success)
        qDebug()<<"Failed to initialize Kinect";
}
int NiteHelper::initialize_device(){
    niteRc = nite::STATUS_OK;
    /// Initialize the NiTE library
    niteRc = nite::NiTE::initialize();
    if(niteRc!=nite::STATUS_OK)
    {
        qDebug()<<"NiTE initialization error! ";
        nite::NiTE::shutdown();
        return false;
    }
    /// Initialize the hand tracker
    niteRc = handTracker.create(&device);
    if(niteRc!=nite::STATUS_OK)
    {
        qDebug()<<"Couldn't create hand tracker! ";
        nite::NiTE::shutdown();
        return false;
    }
    handTracker.startGestureDetection(nite::GESTURE_WAVE);
    handTracker.startGestureDetection(nite::GESTURE_CLICK);
    return true;
}
void NiteHelper::findHands(){
    nite::HandTrackerFrameRef handFrame;
    std::vector <nite::Point3f> handCoord;

    niteRc = handTracker.readFrame(&handFrame);
    qDebug()<<handFrame.getFrameIndex();
    if(niteRc!=nite::STATUS_OK) qDebug()<<"Failed to read hand frame!";
    const nite::Array<nite::GestureData>& gestures = handFrame.getGestures();
    for (int i = 0; i < gestures.getSize(); ++i)
    {
        qDebug()<<"hand gesture!";
        if (gestures[i].isComplete())
        {
            nite::HandId newId;
            qDebug()<<"Found hand";
            handTracker.startHandTracking(gestures[i].getCurrentPosition(), &newId);
        }
    }

    const nite::Array<nite::HandData>& hands = handFrame.getHands();

    for (int i = 0; i < hands.getSize(); ++i)
    {
        const nite::HandData& hand = hands[i];
        if (hand.isTracking()) handCoord.push_back(hand.getPosition());
    }

    /// We take "deep" copies of the color_front_buffer and depth_front_buffer
    mutex()->lock();
    QImage colorImg = colorBuffer().copy();
    mutex()->unlock();
    mutex()->lock();
    QImage depthImg = depthBuffer().copy();
    mutex()->unlock();

    /// convert QImage depthImg to cv::Mat for thresholding and other processing
    /// Note: this depthMat will already be normalized since depthImg is normalized
    //qDebug()<<"color: "<<colorImg.format();
    //qDebug()<<"depth"<<depthImg.format();

    cv::Mat depthMat(depthImg.height(),depthImg.width(),CV_8UC4,(uchar*)depthImg.bits(),depthImg.bytesPerLine());
    ///Threshold the image if we have the hand position
    std::vector <nite::Point3f> depthCoord(handCoord.size());
    float farthest=0;
    for (int i=0; i<handCoord.size(); i++){
        handTracker.convertHandCoordinatesToDepth(handCoord[i].x, handCoord[i].y, handCoord[i].z, &depthCoord[i].x, &depthCoord[i].y);
        depthCoord[i].z = handCoord[i].z;
        /// depthCoord MUST be normalized to get value between 0-255
        unsigned maxdist=10000;
        if(depthCoord[i].z>maxdist) depthCoord[i].z=maxdist;
        if(depthCoord[i].z)
        {
            depthCoord[i].z = (depthCoord[i].z*255)/maxdist;
            //depth = (maxdist-depth)*255/maxdist+1;
        }
        if(depthCoord[i].z>farthest) farthest = depthCoord[i].z;
        depthMat = (depthMat>farthest-60) & (depthMat<farthest+60);
        cv::circle( depthMat, cv::Point(depthCoord[i].x, depthCoord[i].y), 1, cv::Scalar( 0x02,0x60,0xFF ), 4 );
    }
    cv::imshow("hey", depthMat);
}
