#include <iostream>
#include "BufferManager.h"
#include "Preference.h"

using namespace std;

int main (int argc, char *argv[])
{
  Isis::Preference::Preferences(true);
  cout << "Isis::BufferManager Unit Test" << endl << endl;

  Isis::BufferManager bm(6,4,2,3,2,1,Isis::Real);

  for (bm.begin(); !bm.end(); bm++) {
    cout << "Position:  " << bm.Sample() << " " 
                          << bm.Line() << " " 
                          << bm.Band() << endl;
  }
  cout << endl;

  Isis::BufferManager bm2(4,3,2,1,1,1,Isis::Real);

  bm2.begin();
  do {
    cout << "Position:  " << bm2.Sample() << " " 
                          << bm2.Line() << " " 
                          << bm2.Band() << endl;
  } while (bm2.next());
  cout << endl;

  return 0;
}
