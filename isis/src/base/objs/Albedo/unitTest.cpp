/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <iostream>
#include <cstdlib>
#include "PhotoModel.h"
#include "PhotoModelFactory.h"
#include "NormModel.h"
#include "NormModelFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "IException.h"
#include "Preference.h"

using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);
  double result;
  double mult;
  double base;

  std::cout << "UNIT TEST for Albedo normalization function" << std::endl << std::endl;

  PvlGroup algp("Algorithm");
  algp += PvlKeyword("Name", "Lambert");

  PvlObject op("PhotometricModel");
  op.addGroup(algp);

  PvlGroup algn("Algorithm");
  algn += PvlKeyword("Name", "Albedo");

  PvlObject on("NormalizationModel");
  on.addGroup(algn);

  Pvl pvl;
  pvl.addObject(op);
  pvl.addObject(on);
  std::cout << pvl << std::endl << std::endl;

  try {
    PhotoModel *pm = PhotoModelFactory::Create(pvl);
    NormModel *nm = NormModelFactory::Create(pvl, *pm);

    std::cout << "Test phase=0.0, incidence=0.0, emission=0.0, dn=.0800618902 ..." << std::endl;
    nm->CalcNrmAlbedo(0.0, 0.0, 0.0, 0.0, 0.0, .0800618902, result, mult, base);
    std::cout << "Normalization value = " << result << std::endl << std::endl;

    std::cout << "Test phase=86.7207248, incidence=51.7031305, emission=38.9372914, dn=.0797334611 ..." << std::endl;
    nm->CalcNrmAlbedo(86.7207248, 51.7031305, 38.9372914, 51.7031305, 38.9372914, .0797334611, result, mult, base);
    std::cout << "Normalization value = " << result << std::endl << std::endl;

    std::cout << "Test phase=180, incidence=90, emission=90, dn=.0794225037 ..." << std::endl;
    nm->CalcNrmAlbedo(180.0, 90.0, 90.0, 90.0, 90.0, .0794225037, result, mult, base);
    std::cout << "Normalization value = " << result << std::endl << std::endl;
  }
  catch(IException &e) {
    e.print();
  }

  return 0;
}
