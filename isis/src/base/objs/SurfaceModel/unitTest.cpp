/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <vector>
#include <iostream>
#include "SurfaceModel.h"
#include "IException.h"
#include "Preference.h"

int main() {
  Isis::Preference::Preferences(true);

  try {
    Isis::SurfaceModel s;

    double x[] = {0, 1, 1};
    double y[] = {0, 1, -1};
    double z[] = {1, 6, 2};

    std::vector<double> x1, y1, z1;
    x1.push_back(-1);
    x1.push_back(-1);
    x1.push_back(0);
    y1.push_back(1);
    y1.push_back(-1);
    y1.push_back(1);
    z1.push_back(2);
    z1.push_back(3);
    z1.push_back(2);

    std::cout << "add 1" << std::endl;
    s.AddTriplets(x, y, z, 3);
    std::cout << "add 2" << std::endl;
    s.AddTriplets(x1, y1, z1);
    std::cout << "Solve" << std::endl;
    s.Solve();
    std::cout << s.Evaluate(0, 0) << std::endl;
    std::cout << s.Evaluate(1, 1) << std::endl;
    std::cout << s.Evaluate(1, -1) << std::endl;
    std::cout << s.Evaluate(-1, 1) << std::endl;
    std::cout << s.Evaluate(-1, -1) << std::endl;
    std::cout << s.Evaluate(0, 1) << std::endl;

    double mx, my;
    s.MinMax(mx, my);

    std::cout << mx << " " << my << std::endl;
    std::cout << s.Evaluate(mx, my) << std::endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }
}
