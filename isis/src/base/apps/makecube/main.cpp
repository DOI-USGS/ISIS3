#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"

using namespace std;
using namespace Isis;

/**
 *
 * @author 2012-02-22 Steven Lambright
 *
 * @internal
 */
class ConstantValueFunctor {
  public:
    ConstantValueFunctor(double value) {
      m_value = value;
    }


    ConstantValueFunctor(const ConstantValueFunctor &other) {
      m_value = other.m_value;
    }


    void operator()(Buffer &output) const {
      for (int i = 0; i < output.size(); i++) {
        output[i] = m_value;
      }
    }


    ConstantValueFunctor &operator=(const ConstantValueFunctor &rhs) {
      m_value = rhs.m_value;
      return *this;
    }


  private:
    double m_value;
};

void IsisMain() {
  // Create a process by line object
  ProcessByLine p;

  // Get the value to put in the cube
  UserInterface &ui = Application::GetUserInterface();
  QString pixels = ui.GetString("PIXELS");

  double value = Null;
  if(pixels == "NULL") {
    value = NULL8;
  }
  else if(pixels == "LIS") {
    value = LOW_INSTR_SAT8;
  }
  else if(pixels == "LRS") {
    value = LOW_REPR_SAT8;
  }
  else if(pixels == "HIS") {
    value = HIGH_INSTR_SAT8;
  }
  else if(pixels == "HRS") {
    value = HIGH_REPR_SAT8;
  }
  else {
    value = ui.GetDouble("VALUE");
  }

  // Need to pick good min/maxs to ensure the user's value
  // doesn't get saturated
  CubeAttributeOutput &att = ui.GetOutputAttribute("TO");
  if(IsValidPixel(value)) {
    if(value == 0.0) {
      att.setMinimum(value);
      att.setMaximum(value + 1.0);
    }
    if(value < 0.0) {
      att.setMinimum(value);
      att.setMaximum(-value);
    }
    else {
      att.setMinimum(-value);
      att.setMaximum(value);
    }
  }
  else {
    att.setMinimum(0.0);
    att.setMaximum(1.0);
  }

  // Get the size of the cube and create the cube
  int samps = ui.GetInteger("SAMPLES");
  int lines = ui.GetInteger("LINES");
  int bands = ui.GetInteger("BANDS");
  p.SetOutputCube(ui.GetCubeName("TO"), att, samps, lines, bands);

  // Make the cube
  p.ProcessCubeInPlace(ConstantValueFunctor(value), false);
  p.EndProcess();
}
