#include "KinectHelper.h"
#include "detector.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class WorkHelper : public QObject{
    Q_OBJECT
private:
    KinectHelper& khelper;
    Detector haarDetector;
public:
    WorkHelper(KinectHelper& k) : khelper(k){}

public slots:
    void work(){
#ifdef TWO_WAY_OF_DOING_THINGS
        /// not this is not a reference
        QImage rgb;
        {
            QMutexLocker locker(khelper->mutex());
            rgb = khelper.colorBuffer(); ///< copy here!!!
        } ///< Lock is released here
        /// Now processing can take as long as you want...
#else
        QMutexLocker locker(khelper.mutex());
        QImage& rgb = khelper.colorBuffer();
        ///... and how you can read it without, but the
        /// lock will be released on function exit!!
#endif
        /// Converting QImage to rgb
//#if 0
        cv::Mat detect_img(rgb.height(),rgb.width(),CV_8UC3,(uchar*)rgb.bits(),rgb.bytesPerLine());
        if(!detect_img.empty()){
            std::vector<cv::Rect> hands = haarDetector.detectInFrame(detect_img);
            QPainter qPainter(&rgb);
            qPainter.setBrush(Qt::NoBrush);
            qPainter.setPen(Qt::red);
            for( int i = 0; i < hands.size(); i++ )
            {
                qPainter.drawRect(hands[i].x, hands[i].y, hands[i].width, hands[i].height);
            }
            qPainter.end();
        }
//#endif
        khelper.drawColor();
    }
};
