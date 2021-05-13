/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <cstdlib>
#include "PhotoModel.h"
#include "PhotoModelFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "IException.h"
#include "Preference.h"

using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);

  void doit(Pvl & lab);

  std::cout << "UNIT TEST for Isis::PhotoModel" <<
            std::endl << std::endl;

  std::cout << "Testing missing PhotometricModel object ..." <<
            std::endl;
  Pvl lab;
  doit(lab);

  lab.addObject(PvlObject("PhotometricModel"));
  std::cout << "Testing missing Algorithm group ..." <<
            std::endl;
  doit(lab);

  lab.findObject("PhotometricModel").addGroup(PvlGroup("Algorithm"));
  std::cout << "Testing missing Name keyword ..." << std::endl;
  doit(lab);

  // We can't do this in the unit test because it prints out an absolute path
  // to the isis root which changes.
  //lab.findObject("PhotometricModel").findGroup("Algorithm").
  //    addKeyword(PvlKeyword("Name","Bogus"));
  //std::cout << "Testing unsupported photometric model ..." << std::endl;
  //doit(lab);

  lab.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Name", "Minnaert"), Pvl::Replace);

  std::cout << "Testing supported photometric model ..." << std::endl;
  doit(lab);

  lab.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Name", "Lambert"), Pvl::Replace);
  PhotoModel *pm = PhotoModelFactory::Create(lab);

  std::cout << "Testing photometric model PhtTopder method ..." << std::endl;
  try {
    double result;
    result = pm->PhtTopder(0.0, 0.0, 0.0);
    std::cout << "Results from PhtTopder = " << result <<
              std::endl << std::endl;
    result = pm->PhtTopder(86.7226722, 51.7002388, 38.9414439);
    std::cout << "Results from PhtTopder = " << result <<
              std::endl << std::endl;
  }
  catch(IException &e) {
    e.print();
  }

  std::cout << "Test PhtAcos ..." << std::endl;
  try {
    double result;

    result = PhotoModel::PhtAcos(1.0);
    std::cout << "Results from PhtAcos = " << result << std::endl;
    std::cout << "        Actual value = " << 0      << std::endl
              << std::endl;
    result = PhotoModel::PhtAcos(.999999939);
    std::cout << "Results from PhtAcos = " << result      << std::endl;
    std::cout << "        Actual value = " << 0.000349285 << std::endl
              << std::endl;
    result = PhotoModel::PhtAcos(-.861393443);
    std::cout << "Results from PhtAcos = " << result      << std::endl;
    std::cout << "        Actual value = " << 2.608802982 << std::endl
              << std::endl;
  }
  catch(IException &e) {
    e.print();
  }

  std::cout << std::endl;

  return 0;
}

void doit(Pvl &lab) {
  try {
    //PhotoModel *pm = PhotoModelFactory::Create(lab);
    PhotoModelFactory::Create(lab);
  }
  catch(IException &error) {
    error.print();
  }
  std::cout << std::endl;
}
