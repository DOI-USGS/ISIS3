#include <iostream>
#include "WorldMapper.h"
#include "Preference.h"

using namespace std;
class IsisDoubleMapper : public Isis::WorldMapper {
  public:
    double ProjectionX(const double worldX) const { return worldX / 2.0; };
    double ProjectionY(const double worldY) const { return worldY / 3.0; };
    double WorldX(const double projectionX) const { return projectionX * 2.0; };
    double WorldY(const double projectionY) const { return projectionY * 3.0; };
};

int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "Unit test for Isis::WorldMapper" << endl;
  
  Isis::WorldMapper *map = new IsisDoubleMapper();
  
  cout << "WorldX to ProjectionX:  " << map->ProjectionX(100.0) << endl;
  cout << "ProjectionX to WorldX:  " << map->WorldX(50.0) << endl;
  cout << "WorldY to ProjectionY:  " << map->ProjectionY(90.0) << endl;
  cout << "ProjectionY to WorldY:  " << map->WorldY(30.0) << endl;
  cout << "Resolution:             " << map->Resolution() << endl;
}
