#include    "BaseMobility.h"

/****************************************
 *
 *  2014_03_03
 *  Sangsoo Jeong
 *
 *   Stationary Mobility do not provide any mobility function to RSU.
 *   However, Mobility module is necessary for Mixim framework.
 *   That's why this empty module is used
 *
 ***************************************/

class MIXIM_API StationaryMobility : public BaseMobility {
public:
    virtual void initialize(int stage);
    virtual void initializePosition();
    virtual void move();
};
