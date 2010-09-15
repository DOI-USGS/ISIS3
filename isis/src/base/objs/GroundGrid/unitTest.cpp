#include "Isis.h"

#include <iostream>

#include "Cube.h"
#include "UniversalGroundMap.h"
#include "GroundGrid.h"
#include "Progress.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  /*
    The output of this class directly correlates to the output of the "grid"
    application when mode=ground - if that application is correct, then this
    object is correct and vice versa.
   */
  Cube someCube;

  cout << "Reading cube..." << endl;
  someCube.Open("$base/testData/ab102401_ideal.cub");

  cout << "Create universal ground map..." << endl;
  UniversalGroundMap gmap(someCube);

  cout << "Create grid..." << endl;
  Progress progress;
  GroundGrid grid(&gmap, false, someCube.Samples(), someCube.Lines());

  grid.CreateGrid(0, 0, 0.1, 0.1, &progress, 0.5, 0.5);

  cout << "\n\nFirst line on grid: \n" << endl;
  for(int i = 0; i < someCube.Samples(); i++) {
    cout << grid.PixelOnGrid(i, 0) << " ";
    if(i % 35 == 0) cout << endl;
  }

  cout << endl;
}
