#include <ir_Daikin.h>
#ifndef AC_CONTROLLER
#define AC_CONTROLLER
#include "ACController.cpp"
#endif


class DaikinAC: public ACController {
  private:
    const uint16_t kIrLed = 4;
    IRDaikinESP* ac;

  public:


    DaikinAC() {
      ac = new IRDaikinESP(kIrLed);
    }

    virtual void initialize() {
      ac->begin();
    };

    virtual void setTemp(int temp) {
      //validate is happening in the internal class
      ac->setTemp(temp);
    };

    virtual void setFanSpeed(int fanSpeed) {
      ac->setFan(fanSpeed);
    };

    virtual void setMode(ACMode mode) {
      switch (mode) {
        case AUTO :
          ac->setMode(kDaikinAuto);
          break;       // and exits the switch
        case COOL :
          ac->setMode(kDaikinCool);
          break;
        case DRY :
          ac->setMode(kDaikinDry);
          break;
        case HEAT :
          ac->setMode(kDaikinHeat);
          break;
      }
    };

    virtual void toogle(bool state) {
      if (state) {
        ac->on();
      } else {
        ac->off();
      }
    };

    virtual void emit() {
      ac->send();
    }


};
