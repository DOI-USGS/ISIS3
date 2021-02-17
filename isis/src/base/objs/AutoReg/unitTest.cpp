/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <cstdlib>
#include <iostream>
#include "AutoReg.h"
#include "AutoRegFactory.h"
#include "Chip.h"
#include "Cube.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"

using namespace std;
using namespace Isis;

AutoReg *p_ar;

int main() {
  Isis::Preference::Preferences(true);
  void Doit(Isis::PvlObject & obj);
  void DoRegister();

  cout << "Unit test for Isis::AutoReg ..." << endl << endl;

  cout << "---------------------------------" << endl;
  cout << "Test for missing Algorithm Group" << endl;
  cout << "---------------------------------" << endl;
  PvlObject obj("AutoRegistration");
  Doit(obj);
  cout << endl;

  cout << "------------------------------" << endl;
  cout << "Test for missing Name Keyword" << endl;
  cout << "------------------------------" << endl;
  obj.addGroup(Isis::PvlGroup("Algorithm"));
  Isis::PvlGroup &mg = obj.findGroup("Algorithm");
  Doit(obj);
  cout << endl;

  cout << "------------------------------" << endl;
  cout << "Test for invalid Name Keyword" << endl;
  cout << "------------------------------" << endl;
  mg += Isis::PvlKeyword("Name", "MaxCor");
  Doit(obj);
  cout << endl;

  cout << "-----------------------------------" << endl;
  cout << "Test for missing Tolerance Keyword" << endl;
  cout << "-----------------------------------" << endl;
  mg["Name"] = "MaximumCorrelation";
  Doit(obj);
  cout << endl;

  cout << "-----------------------------------" << endl;
  cout << "Test for missing PatternChip Group" << endl;
  cout << "-----------------------------------" << endl;
  mg += Isis::PvlKeyword("Tolerance", "0.3");
  Doit(obj);
  cout << endl;

  cout << "-----------------------------------" << endl;
  cout << "Test for wrong ChipInterpolator Keyword value" << endl;
  cout << "-----------------------------------" << endl;
  mg += Isis::PvlKeyword("ChipInterpolator", "None");
  Doit(obj);
  mg["ChipInterpolator"] = "BiLinearType";
  cout << endl;

  cout << "-----------------------------------" << endl;
  cout << "Test for wrong Gradient Keyword value" << endl;
  cout << "-----------------------------------" << endl;
  mg += Isis::PvlKeyword("Gradient", "Random");
  Doit(obj);
  mg["Gradient"] = "None";
  cout << endl;

  obj.addGroup(Isis::PvlGroup("PatternChip"));
  Isis::PvlGroup &pc = obj.findGroup("PatternChip");
  pc += Isis::PvlKeyword("Lines", "90");
  pc += Isis::PvlKeyword("Samples", "90");

  cout << "-------------------------------" << endl;
  cout << "Test for missing Lines Keyword" << endl;
  cout << "-------------------------------" << endl;
  pc.deleteKeyword("Lines");
  Doit(obj);
  cout << endl;

  cout << "---------------------------------" << endl;
  cout << "Test for missing Samples Keyword" << endl;
  cout << "---------------------------------" << endl;
  pc += Isis::PvlKeyword("Lines", "90");
  pc.deleteKeyword("Samples");
  Doit(obj);
  cout << endl;

  cout << "---------------------------------" << endl;
  cout << "Test for invalid Lines Keyword" << endl;
  cout << "---------------------------------" << endl;
  pc += Isis::PvlKeyword("Samples", "90");
  pc["Lines"] = "-90";
  Doit(obj);
  cout << endl;

  cout << "---------------------------------" << endl;
  cout << "Test for invalid Samples Keyword" << endl;
  cout << "---------------------------------" << endl;
  pc["Lines"] = "90";
  pc["Samples"] = "-90";
  Doit(obj);
  cout << endl;

  cout << "----------------------------------" << endl;
  cout << "Test for missing SearchChip group" << endl;
  cout << "----------------------------------" << endl;
  pc["Samples"] = "90";
  Doit(obj);
  cout << endl;

  obj.addGroup(Isis::PvlGroup("SearchChip"));
  Isis::PvlGroup &sc = obj.findGroup("SearchChip");
  sc += Isis::PvlKeyword("Lines", "150");
  sc += Isis::PvlKeyword("Samples", "150");

  cout << "-------------------------------" << endl;
  cout << "Test for missing Lines Keyword" << endl;
  cout << "-------------------------------" << endl;
  sc.deleteKeyword("Lines");
  Doit(obj);
  cout << endl;

  cout << "---------------------------------" << endl;
  cout << "Test for missing Samples Keyword" << endl;
  cout << "---------------------------------" << endl;
  sc += Isis::PvlKeyword("Lines", "150");
  sc.deleteKeyword("Samples");
  Doit(obj);
  cout << endl;

  sc += Isis::PvlKeyword("Samples", "150");
  Doit(obj);
  cout << endl;

  cout << "-------------------------------------------" << endl;
  cout << "Testing for invalid value for PatternChip ValidPercent" << endl;
  cout << "-------------------------------------------" << endl;
  try {
    p_ar->SetPatternValidPercent(-1);
  }
  catch (IException &e) {
    e.print();
  }
  cout << endl;
  cout << "-------------------------------------------" << endl;
  cout << "Testing for invalid value for SearchChip SubchipValidPercent" << endl;
  cout << "-------------------------------------------" << endl;
  try {
    p_ar->SetSubsearchValidPercent(102);
  }
  catch (IException &e) {
    e.print();
  }
  cout << endl;
  cout << "-------------------------------------------" << endl;
  cout << "Testing for invalid value for PatternChip ZScoreMinimum" << endl;
  cout << "-------------------------------------------" << endl;
  try {
    p_ar->SetPatternZScoreMinimum(0);
  }
  catch (IException &e) {
    e.print();
  }
  cout << endl;
  cout << "-------------------------------------------" << endl;
  cout << "Testing for invalid value for PatternChip WindowSize" << endl;
  cout << "-------------------------------------------" << endl;
  try {
    p_ar->SetSurfaceModelWindowSize(1);
  }
  catch (IException &e) {
    e.print();
  }
  cout << endl;
  try {
    p_ar->SetSurfaceModelWindowSize(4);
  }
  catch (IException &e) {
    e.print();
  }
  cout << endl;
  cout << "-------------------------------------------" << endl;
  cout << "Testing for invalid value for SurfaceModel DistanceTolerance" << endl;
  cout << "-------------------------------------------" << endl;
  try {
    p_ar->SetSurfaceModelDistanceTolerance(0);
  }
  catch (IException &e) {
    e.print();
  }
  cout << endl;
  cout << "-------------------------------------------" << endl;
  cout << "Testing for invalid value for Algorithm ReductionFactor" << endl;
  cout << "-------------------------------------------" << endl;
  try {
    p_ar->SetReductionFactor(0);
  }
  catch (IException &e) {
    e.print();
  }
  cout << endl;

  cout << "-------------------------------------------" << endl;
  cout << "Testing Error = PatternNotEnoughValidData" << endl;
  cout << "-------------------------------------------" << endl;
  p_ar->PatternChip()->SetValidRange(0.009, 0.01);
  p_ar->SetPatternValidPercent(100);
  DoRegister();

  //Reset the range and the valid percents
  p_ar->PatternChip()->SetValidRange(0, 1);
  p_ar->SetPatternValidPercent(50);
  p_ar->SetSubsearchValidPercent(50);
  Doit(obj);

  cout << "\n------------------------------------" << endl;
  cout << "Testing Error = PatternZScoreNotMet" << endl;
  cout << "------------------------------------" << endl;
  p_ar->SetPatternZScoreMinimum(9.0);
  DoRegister();
  //Reset the Pattern ZScore Minimum
  p_ar->SetPatternZScoreMinimum(1.0);
  Doit(obj);

  cout << "\n------------------------------" << endl;
  cout << "Testing Error = FitChipNoData" << endl;
  cout << "------------------------------" << endl;
  for(int line = 1; line <= p_ar->SearchChip()->Lines(); line++) {
    for(int samp = 1; samp <= p_ar->SearchChip()->Samples(); samp++) {
      p_ar->SearchChip()->SetValue(samp, line, Isis::Null);
    }
  }
  DoRegister();
  Doit(obj);

  cout << "\n----------------------------------------" << endl;
  cout << "Testing Error = FitChipToleranceNotMet" << endl;
  cout << "----------------------------------------" << endl;
  p_ar->SetTolerance(0.9);
  DoRegister();
  //Reset the Tolerance
  p_ar->SetTolerance(0.3);
  Doit(obj);

  cout << "\n-----------------------------------------------" << endl;
  cout << "Testing Error = SurfaceModelNotEnoughValidData" << endl;
  cout << "-----------------------------------------------" << endl;
  p_ar->SetSurfaceModelWindowSize(39);
  DoRegister();
  //Reset the surface model Window size
  p_ar->SetSurfaceModelWindowSize(5);
  Doit(obj);

  cout << "\n---------------------------------------------" << endl;
  cout << "Testing Error = SurfaceModelDistanceInvalid" << endl;
  cout << "---------------------------------------------" << endl;
  p_ar->SetSurfaceModelDistanceTolerance(.01);
  DoRegister();
  //Reset the surface model distance tolerance
  p_ar->SetSurfaceModelDistanceTolerance(1.5);
  Doit(obj);

  cout << "\n---------------------" << endl;
  cout << "Testing SuccessPixel" << endl;
  cout << "---------------------" << endl;
  p_ar->SetSubPixelAccuracy(false);
  DoRegister();
  //Reset to sub-pixel accuracy
  p_ar->SetSubPixelAccuracy(true);
  Doit(obj);

  cout << "\n------------------------" << endl;
  cout << "Testing SuccessSubPixel" << endl;
  cout << "------------------------" << endl;
  DoRegister();
  Doit(obj);

  cout << "\n---------------------" << endl;
  cout << "Testing Gradient" << endl;
  cout << "---------------------" << endl;
  p_ar->SetGradientFilterType("Sobel");
  p_ar->SetTolerance(0.18);
  DoRegister();
  cout << "Goodness of Fit = " << p_ar->GoodnessOfFit() << endl;

  //reset to minimum difference algorithm
  cout << "\n---------------------" << endl;
  cout << "Testing Minimum Difference Algorithm" << endl;
  cout << "---------------------" << endl;
  mg = obj.findGroup("Algorithm");
  mg["Name"] = "MinimumDifference";
  Doit(obj);
  p_ar->SetSubPixelAccuracy(false);
  DoRegister();
  cout << "Goodness of Fit = " << p_ar->GoodnessOfFit() << endl;
  

  delete p_ar;
  return 0;
}

void Doit(Isis::PvlObject &obj) {
  try {
    Pvl lab;
    lab.addObject(obj);
    p_ar = AutoRegFactory::Create(lab);
    Cube c;
    c.open("$ISISTESTDATA/isis/src/base/unitTestData/AutoReg/search_low.cub");
    p_ar->SearchChip()->TackCube(75.0, 75.0);
    p_ar->SearchChip()->Load(c);
    Cube d;
    d.open("$ISISTESTDATA/isis/src/base/unitTestData/AutoReg/pattern.cub");
    p_ar->PatternChip()->TackCube(45.0, 45.0);
    p_ar->PatternChip()->Load(d);
  }
  catch(Isis::IException &error) {
    IString err = error.what();

    // We need to get rid of the contents of the second [] on each line to
    // ensure file paths do not persist.
    IString thisLine;
    IString formattedErr;

    while((thisLine = err.Token("\n")) != "") {
      formattedErr += thisLine.Token("[");

      if(!thisLine.empty()) {
        formattedErr += "[" + thisLine.Token("]") + "]";
      }

      if(!thisLine.empty() && thisLine.find('[') != string::npos) {
        formattedErr += thisLine.Token("[") + "[]";
        thisLine.Token("]");
        formattedErr += thisLine;
      }
      else {
        formattedErr += thisLine;
      }

      formattedErr += "\n";
    }


    std::cerr << formattedErr;
  }
}

void DoRegister() {
  p_ar->Register();
  if(p_ar->Success()) {
    std::cout << "Position = " << p_ar->CubeSample() << " " <<  p_ar->CubeLine() << std::endl;
  }
  Pvl regstats = p_ar->RegistrationStatistics();
  std::cout  << regstats << std::endl;
}
