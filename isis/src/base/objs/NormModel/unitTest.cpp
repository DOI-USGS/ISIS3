/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <cstdlib>
#include "PhotoModel.h"
#include "PhotoModelFactory.h"
#include "AtmosModel.h"
#include "AtmosModelFactory.h"
#include "NormModel.h"
#include "NormModelFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "IException.h"
#include "Preference.h"

using namespace Isis;

void doit(Pvl &lab, PhotoModel &pm);
void doit(Pvl &lab, PhotoModel &pm, AtmosModel &am);

int main() {
  Isis::Preference::Preferences(true);

  std::cout << "UNIT TEST for Isis::NormModel" << std::endl << std::endl;

  Pvl lab;
  lab.addObject(PvlObject("PhotometricModel"));
  lab.findObject("PhotometricModel").addGroup(PvlGroup("Algorithm"));
  lab.findObject("PhotometricModel").findGroup("Algorithm").addKeyword(PvlKeyword("Name", "Lambert"));
  PhotoModel *pm = PhotoModelFactory::Create(lab);

  lab.addObject(PvlObject("AtmosphericModel"));
  lab.findObject("AtmosphericModel").addGroup(PvlGroup("Algorithm"));
  lab.findObject("AtmosphericModel").findGroup("Algorithm").addKeyword(PvlKeyword("Name", "Anisotropic1"));
  AtmosModel *am = AtmosModelFactory::Create(lab, *pm);

  std::cout << "Testing missing NormalizationModel object ..." << std::endl;

  doit(lab, *pm);
  doit(lab, *pm, *am);

  lab.addObject(PvlObject("NormalizationModel"));
  std::cout << "Testing missing Algorithm group ..." << std::endl;

  doit(lab, *pm);
  doit(lab, *pm, *am);

  lab.findObject("NormalizationModel").addGroup(PvlGroup("Algorithm"));

  std::cout << "Testing missing Name keyword ..." << std::endl;
  doit(lab, *pm);
  doit(lab, *pm, *am);

  lab.findObject("NormalizationModel").findGroup("Algorithm").addKeyword(PvlKeyword("Name", "Albedo"), Pvl::Replace);
  std::cout << "Testing supported normalization model ..." << std::endl;
  doit(lab, *pm);
  doit(lab, *pm, *am);

  NormModel *nm = NormModelFactory::Create(lab, *pm);

  try {
    std::cout << "Testing normalization model get methods ..." << std::endl;
    std::cout << "AlgorithmName = " << nm->AlgorithmName() << std::endl;
  }
  catch(IException &e) {
    e.print();
  }

  std::cout << std::endl;

  return 0;
}

void doit(Pvl &lab, PhotoModel &pm) {
  try {
    NormModelFactory::Create(lab, pm);
  }
  catch(IException &error) {
    error.print();
  }
  std::cout << std::endl;
}

void doit(Pvl &lab, PhotoModel &pm, AtmosModel &am) {
  try {
    NormModelFactory::Create(lab, pm, am);
  }
  catch(IException &error) {
    error.print();
  }
  std::cout << std::endl;
}
