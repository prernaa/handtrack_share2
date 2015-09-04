#include "HandModel.h"

class HandModel_Cylinders : public Hand{
    void palm();
    void thumb();
    void finger();
    void middle();
    void anular();
    void pinky();

public:
    HandModel_Cylinders();
    void draw();
};
