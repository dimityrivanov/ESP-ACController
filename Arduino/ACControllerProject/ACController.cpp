#ifndef AC_UNIT
#define AC_UNIT
#include "ACUnit.cpp"
#endif

class ACController {
  public:
    virtual void initialize() = 0;
    virtual void toogle(bool state) = 0;
    virtual void setTemp(int temp) = 0;
    virtual void setFanSpeed(int fanSpeed) = 0;
    virtual void setMode(ACMode mode) = 0;
    virtual void emit() = 0;
};
