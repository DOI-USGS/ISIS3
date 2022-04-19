/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Isis.h"

#include <QFile>

#include "Interpolator.h"
#include "Transform.h"
#include "ProcessRubberSheet.h"

using namespace std;
using namespace Isis;

class UnitTestTrans : public Transform {
  private:
    int p_outSamps;
    int p_outLines;

  public:
    // constructor
    UnitTestTrans(const int inSamps, const int inLines) {
      p_outSamps = inSamps;
      p_outLines = inLines;
    }

    // destructor
    ~UnitTestTrans() {}

    // Instantiation of pure virtual members
    int OutputSamples() const {
      return p_outSamps;
    }
    int OutputLines() const {
      return p_outLines;
    }
    bool Xform(double &inSample, double &inLine,
               const double outSample,
               const double outLine) {
      inSample = outSample;
      if(outSample > 64) {
        inSample = 127 - (outSample - 64);
      }

      inLine = outLine;

      static int saveSamp = 0;
      static int saveLine = 0;
      if((outSample != saveSamp + 1) || (outLine != saveLine)) {
        cout << "Output Sample:Line = " << outSample << ":" << outLine << endl;
        saveSamp = (int)(outSample + 0.5);
        saveLine = (int)(outLine + 0.5);
      }
      else {
        saveSamp++;
      }

      return true;
    }
};

void IsisMain() {

  Preference::Preferences(true);

  void myBandChange(const int b);

  ProcessRubberSheet p;
  p.BandChange(myBandChange);
  Transform *trans = new UnitTestTrans(126, 126);

  Interpolator *interp;
  interp = new Interpolator(Interpolator::NearestNeighborType);

  cout << "Testing ProcessRubberSheet Class ... " << endl;
  p.SetInputCube("FROM");
  p.SetOutputCube("TO", 126, 126, 2);
  p.StartProcess(*trans, *interp);
  p.EndProcess();
  cout << endl;

  try {
    cout << "Testing NO input with one output error ..." << endl;
    p.SetOutputCube("TO", 1, 1, 1);
    p.StartProcess(*trans, *interp);
  }
  catch(IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  try {
    cout << "Testing one input with NO output error ..." << endl;
    p.SetInputCube("FROM");
    p.StartProcess(*trans, *interp);
  }
  catch(IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  delete trans;
  delete interp;

  UserInterface &ui = Application::GetUserInterface();
  QFile::remove(ui.GetCubeName("TO"));
}


void myBandChange(const int band) {
  cout << "The band changed to :" << band << endl;
}


