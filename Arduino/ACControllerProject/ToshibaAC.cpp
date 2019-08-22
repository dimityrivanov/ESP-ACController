#include <ir_Toshiba.h>
#ifndef AC_CONTROLLER
#define AC_CONTROLLER
#include "ACController.cpp"
#endif

class ToshibaAC: public ACController {
  private:
    const uint16_t kIrLed = 4;
    IRToshibaAC* ac;

  public:
    ToshibaAC() {
      ac = new IRToshibaAC(kIrLed);
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
          ac->setMode(kToshibaAcAuto);
          break;       // and exits the switch
        case COOL :
          ac->setMode(kToshibaAcCool);
          break;
        case DRY :
          ac->setMode(kToshibaAcDry);
          break;
        case HEAT :
          ac->setMode(kToshibaAcHeat);
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
