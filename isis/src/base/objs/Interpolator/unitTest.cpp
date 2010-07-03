#include <iostream>
#include <iomanip>
#include <sstream>
#include "iException.h"
#include "Interpolator.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[])
{
  Isis::Preference::Preferences(true);

  // Test for an invalid interpolator
  cout.setf(ios::showpoint);
  cout << setprecision(15) << setw(17); 
  cout << "Testing invalid interpolator" << endl;
  double buf[] = {99.5};
  
  Isis::Interpolator *interp = new Isis::Interpolator ();
  try {
    double val = interp->Interpolate (10.0, 25.0, buf);
    cout << "  " << 99.5 << " = " << val << endl;
  }
  catch (Isis::iException &e) {
    cout << "  Cought invalid interpolator error." << endl;
    e.Report (false);
  }
  
  // Test Nearest Neighbor 
  cout << "Testing Nearest Neighbor" << endl;
  interp->SetType (Isis::Interpolator::NearestNeighborType);
  cout << "  99.5 = " << interp->Interpolate (25.99, 10.0, buf) << endl;
  delete interp;


  // Test BiLinear with a good buffer
  cout << "Testing Bilinear" << endl;
  double buf2[] = {1.0, 2.0,
                   3.0, 4.0};
  interp = new Isis::Interpolator (Isis::Interpolator::BiLinearType);
  cout << "  1.0 = " << interp->Interpolate (25.0, 10.0, buf2) << endl;  
  cout << "  2.5 = " << interp->Interpolate (25.5, 10.5, buf2) << endl;
  cout << "  4.0 = " << interp->Interpolate (25.9999999999999, 10.9999999999999,
                                           buf2) << endl;

  // Test BiLinear with invalid data in buffer
  buf2[1] = Isis::NULL8;
  cout << "  1.0 = " << interp->Interpolate (25.0, 10.0, buf2) << endl;  
  delete interp;


  // Test Cubic Convolution with a good buffer
  cout << "Testing Cubic Convolution" << endl;
  double buf3[] = { 1.0,  2.0,  3.0,  4.0,
                    5.0,  6.0,  7.0,  8.0,
                    9.0, 10.0, 11.0, 12.0,
                   13.0, 14.0, 15.0, 16.0};
  interp = new Isis::Interpolator (Isis::Interpolator::CubicConvolutionType);
  cout << "  6.0 = " << interp->Interpolate (25.0, 10.0, buf3) << endl;
  cout << "  " << 8.5 << " = " << interp->Interpolate (25.5, 10.5, buf3) << endl;
  cout << "  " << 11.0 << " = " <<
          interp->Interpolate (25.9999999999999, 10.9999999999999, buf3) << endl;

  // Test Cubic Convolution with invalid data in edge of buffer
  buf3[7] = Isis::HIGH_INSTR_SAT8;
  cout << "  10.0 = " << interp->Interpolate (25.0, 10.9999999999999, buf3) << endl;  

  // Test Cubic Convolution with invalid data in middle of buffer
  buf3[7] = 8.0;
  buf3[6] = Isis::LOW_INSTR_SAT8;
  cout << "  6.0 = " << interp->Interpolate (25.999, 10.999, buf3) << endl;  
}
