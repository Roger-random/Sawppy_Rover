
#ifndef lewansoul_h
#define lewansoul_h

#include "arduino.h"

class LewanSoul
{
  public:
    LewanSoul(bool debug);
    
    void setup();

    // ID of desired servo to move, and destination angle from -120 to 120 degrees
    void moveTo(int id, float destination);

    // ID of desired servo to spin, and velocity from -100 to 100 percent
    void spinAt(int id, float velocity);

  private:
    bool _debug;
};

#endif // lewansoul_h
