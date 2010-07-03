#include "ControlPointList.h"
#include "iException.h"

#include <string>
#include <iostream>
using namespace std;

int main() {
  Isis::ControlPointList cpl("points.lis"); //list of Control Point Ids in the file

  int size = cpl.Size();

  //print point ids in the list
  for(int i = 0; i < size; i++) {
    std::cout << cpl.ControlPointId(i) << "\n";
  }

  // index out of range
  try {
    std::cout << cpl.ControlPointId(size) << "\n";
  }
  catch(Isis::iException &e) {
    e.Report(false);
  }

  try {
    std::cout << cpl.ControlPointIndex("new0007") << "\n";
    std::cout << cpl.ControlPointIndex("new0036") << "\n";
    std::cout << cpl.ControlPointIndex("new0000") << "\n"; //not found - invalid point
  }
  catch(Isis::iException &e) {
    e.Report(false);
  }
}
