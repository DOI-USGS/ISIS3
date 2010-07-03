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
  obj.AddGroup(Isis::PvlGroup("Algorithm"));
  Isis::PvlGroup &mg = obj.FindGroup("Algorithm");
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
  mg += Isis::PvlKeyword("Tolerance", 0.3);
  Doit(obj);
  cout << endl;

  cout << "-----------------------------------" << endl;
  cout << "Test for wrong ChipInterpolator Keyword value" << endl;
  cout << "-----------------------------------" << endl;
  mg += Isis::PvlKeyword("ChipInterpolator", "None");
  Doit(obj);
  mg["ChipInterpolator"] = "BiLinearType";
  cout << endl;

  obj.AddGroup(Isis::PvlGroup("PatternChip"));
  Isis::PvlGroup &pc = obj.FindGroup("PatternChip");
  pc += Isis::PvlKeyword("Lines", 90);
  pc += Isis::PvlKeyword("Samples", 90);

  cout << "-------------------------------" << endl;
  cout << "Test for missing Lines Keyword" << endl;
  cout << "-------------------------------" << endl;
  pc.DeleteKeyword("Lines");
  Doit(obj);
  cout << endl;

  cout << "---------------------------------" << endl;
  cout << "Test for missing Samples Keyword" << endl;
  cout << "---------------------------------" << endl;
  pc += Isis::PvlKeyword("Lines", 90);
  pc.DeleteKeyword("Samples");
  Doit(obj);
  cout << endl;

  cout << "---------------------------------" << endl;
  cout << "Test for invalid Lines Keyword" << endl;
  cout << "---------------------------------" << endl;
  pc += Isis::PvlKeyword("Samples", 90);
  pc["Lines"] = -90;
  Doit(obj);
  cout << endl;

  cout << "---------------------------------" << endl;
  cout << "Test for invalid Samples Keyword" << endl;
  cout << "---------------------------------" << endl;
  pc["Lines"] = 90;
  pc["Samples"] = -90;
  Doit(obj);
  cout << endl;

  cout << "----------------------------------" << endl;
  cout << "Test for missing SearchChip group" << endl;
  cout << "----------------------------------" << endl;
  pc["Samples"] = 90;
  Doit(obj);
  cout << endl;

  obj.AddGroup(Isis::PvlGroup("SearchChip"));
  Isis::PvlGroup &sc = obj.FindGroup("SearchChip");
  sc += Isis::PvlKeyword("Lines", 150);
  sc += Isis::PvlKeyword("Samples", 150);

  cout << "-------------------------------" << endl;
  cout << "Test for missing Lines Keyword" << endl;
  cout << "-------------------------------" << endl;
  sc.DeleteKeyword("Lines");
  Doit(obj);
  cout << endl;

  cout << "---------------------------------" << endl;
  cout << "Test for missing Samples Keyword" << endl;
  cout << "---------------------------------" << endl;
  sc += Isis::PvlKeyword("Lines", 150);
  sc.DeleteKeyword("Samples");
  Doit(obj);
  cout << endl;

  sc += Isis::PvlKeyword("Samples", 150);
  Doit(obj);
  cout << endl;

  cout << "-------------------------------------------" << endl;
  cout << "Testing Error = PatternNotEnoughValidData" << endl;
  cout << "-------------------------------------------" << endl;
  p_ar->PatternChip()->SetValidRange(0.009, 0.01);
  p_ar->SetPatternValidPercent(100);
  DoRegister();
  //Reset the range and the valid percent
  p_ar->PatternChip()->SetValidRange(0, 1);
  p_ar->SetPatternValidPercent(50);
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

  cout << "\n----------------------------------------------------" << endl;
  cout << "Testing Error = SurfaceModelEccentricityRatioNotMet" << endl;
  cout << "----------------------------------------------------" << endl;
  p_ar->SetSurfaceModelEccentricityRatio(1);
  DoRegister();
  //Reset the surface model Eccentricity ratio
  p_ar->SetSurfaceModelEccentricityRatio(2);
  Doit(obj);

  cout << "\n----------------------------------------------------" << endl;
  cout << "Testing Error = SurfaceModelResidualToleranceNotMet" << endl;
  cout << "----------------------------------------------------" << endl;
  p_ar->SetSurfaceModelResidualTolerance(0.01);
  DoRegister();
  //Reset the surface model residual tolerance
  p_ar->SetSurfaceModelResidualTolerance(0.1);
  Doit(obj);

  cout << "\n---------------------------------------------" << endl;
  cout << "Testing Error = SurfaceModelDistanceInvalid" << endl;
  cout << "---------------------------------------------" << endl;
  p_ar->SetSurfaceModelDistanceTolerance(.01);
  DoRegister();
  //Reset the surface model distance tolerance
  p_ar->SetSurfaceModelDistanceTolerance(1.5);
  Doit(obj);

  cout << "\n----------------" << endl;
  cout << "Testing Success" << endl;
  cout << "----------------" << endl;
  DoRegister();

  delete p_ar;
  return 0;

}

void Doit(Isis::PvlObject &obj) {
  try {
    Pvl lab;
    lab.AddObject(obj);
    p_ar = AutoRegFactory::Create(lab);
    Cube c;
    c.Open("$base/testData/search.cub");
    p_ar->SearchChip()->TackCube(75.0, 75.0);
    p_ar->SearchChip()->Load(c);
    Cube d;
    d.Open("$base/testData/pattern.cub");
    p_ar->PatternChip()->TackCube(45.0, 45.0);
    p_ar->PatternChip()->Load(d);
    p_ar->SetEccentricityTesting(true);
    p_ar->SetResidualTesting(true);
  }
  catch(Isis::iException &error) {
    error.Clear();
    iString err = error.what();

    // We need to get rid of the contents of the second []  on each line to ensure
    //  file paths do not persist.
    iString thisLine;
    iString formattedErr;

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
  AutoReg::RegisterStatus status = p_ar->Register();
  if(status == AutoReg::Success) {
    std::cout << "Position = " << p_ar->CubeSample() << " " <<  p_ar->CubeLine() << std::endl;
  }
  Pvl regstats = p_ar->RegistrationStatistics();
  std::cout  << regstats << std::endl;
}
