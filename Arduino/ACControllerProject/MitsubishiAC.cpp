#include <ir_Mitsubishi.h>
#ifndef AC_CONTROLLER
#define AC_CONTROLLER
#include "ACController.cpp"
#endif


class MitsubishiAC: public ACController {
  private:
    const uint16_t kIrLed = 4;
    IRMitsubishiAC* ac;
    
  public:


    MitsubishiAC() {
      ac = new IRMitsubishiAC(kIrLed);
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
          ac->setMode(kMitsubishiAcAuto);
          break;       // and exits the switch
        case COOL :
          ac->setMode(kMitsubishiAcCool);
          break;
        case DRY :
          ac->setMode(kMitsubishiAcDry);
          break;
        case HEAT :
          ac->setMode(kMitsubishiAcHeat);
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
