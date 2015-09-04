#include "KinectHelper.h"
#include "NiTE.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class NiteHelper : public KinectHelper
{
    Q_OBJECT
    nite::Status niteRc;
    nite::HandTracker handTracker;
    QImage colorImg;
    QImage depthImg;
public:
    NiteHelper(QObject* parent=0);
    ~NiteHelper(){
        nite::NiTE::shutdown();
    }
private:
    int initialize_device();
public slots:
    void findHands();
};

