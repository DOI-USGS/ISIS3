#include "Isis.h"
#include "Transform.h"
#include "Interpolator.h"
#include "ProcessRubberSheet.h"

using namespace std;
class UnitTestTrans : public Isis::Transform {
  private:
    int p_outSamps;
    int p_outLines;

  public:
    // constructor
    UnitTestTrans (const int inSamps, const int inLines) {
      p_outSamps = inSamps;
      p_outLines = inLines;
    };

    // destructor
    ~UnitTestTrans () {};

    // Instantiation of pure virtual members
    int OutputSamples () const {return p_outSamps;};
    int OutputLines () const {return p_outLines;};
    bool Xform (double &inSample, double &inLine,
                    const double outSample,
                    const double outLine) {
      inSample = outSample;
      if (outSample > 64) {
        inSample = 127 - (outSample - 64);
      }
      
      inLine = outLine;

      static int saveSamp = 0;
      static int saveLine = 0;
      if ((outSample != saveSamp+1) || (outLine != saveLine)) {
        cout << "Output Sample:Line = " << outSample << ":" << outLine << endl;
        saveSamp = (int)(outSample + 0.5);
        saveLine = (int)(outLine + 0.5);
      }
      else {
        saveSamp++;
      }

      return true;
    };
};

void IsisMain() {

  Isis::Preference::Preferences(true);

  void myBandChange (const int b);

  Isis::ProcessRubberSheet p;
  p.BandChange (myBandChange);
  Isis::Transform *trans = new UnitTestTrans(126, 126);
  
  Isis::Interpolator *interp;
  interp = new Isis::Interpolator(Isis::Interpolator::NearestNeighborType);

  cout << "Testing Isis::ProcessRubberSheet Class ... " << endl;
  p.SetInputCube("FROM");
  p.SetOutputCube ("TO", 126, 126, 2);
  p.StartProcess(*trans, *interp);
  p.EndProcess();
  cout << endl;

  try {
    cout << "Testing NO input with one output error ..." << endl;
    p.SetOutputCube ("TO",1,1,1);
    p.StartProcess (*trans, *interp);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  try {
    cout << "Testing one input with NO output error ..." << endl;
    p.SetInputCube ("FROM");
    p.StartProcess (*trans, *interp);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  delete trans;
  delete interp;
  remove("/tmp/isisRubberSheet_01.cub");
}


void myBandChange (const int band) {
  cout << "The band changed to :" << band << endl;
}


