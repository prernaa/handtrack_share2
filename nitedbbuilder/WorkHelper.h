#include "KinectHelper.h"
#include "NiteHelper.h"


class WorkHelper : public QObject{
    Q_OBJECT
private:
    NiteHelper& khelper;
public:
    WorkHelper(NiteHelper& k) : khelper(k){}

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

    }
};
