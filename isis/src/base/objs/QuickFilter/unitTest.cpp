#include <iostream>
#include "QuickFilter.h"
#include "iException.h"
#include "Preference.h"

using namespace std;

class IsisFilterFEL : public Isis::QuickFilter {
  public:
    IsisFilterFEL (const int ns, const int width, const int height) :
      Isis::QuickFilter (ns,width,height) {
    }

    void AddLine (const double *buf) {
      Isis::QuickFilter::AddLine(buf);
    }

    void RemoveLine (const double *buf) {
      Isis::QuickFilter::RemoveLine(buf);
    }

    void SetMinMax (const double min, const double max) {
      Isis::QuickFilter::SetMinMax(min,max);
    }

    void SetMinimumPixels (const int min) {
      Isis::QuickFilter::SetMinimumPixels(min);
    }
};

int main (int argc, char *argv[])
{
  Isis::Preference::Preferences(true);

  cout << "Unit Test for QuickFilter Object" << endl;
  cout << "--------------------------------" << endl;

  cout << "Constructing f" << endl;
  IsisFilterFEL f(4,3,5);

  double a[4];
  a[0] = 1.0;
  a[1] = 2.0;
  a[2] = 3.0;
  
  cout << "Adding Line 1" << endl;
  a[3] = Isis::NULL8;
  f.AddLine(a);
  
  cout << "Adding Line 2" << endl;
  a[3] = Isis::LOW_REPR_SAT8;
  f.AddLine(a);

  cout << "Adding Line 3" << endl;
  a[3] = Isis::HIGH_REPR_SAT8;
  f.AddLine(a);
  
  cout << "Adding Line 4" << endl;
  a[3] = Isis::LOW_INSTR_SAT8;
  f.AddLine(a);
  
  cout << "Adding Line 5" << endl;
  a[3] = Isis::HIGH_INSTR_SAT8;
  f.AddLine(a);
  cout << endl;
 
  cout << "Boxcar Width:           " << f.Width() << endl;
  cout << "Boxcar Height:          " << f.Height() << endl;
  cout << "Half Boxcar Width:      " << f.HalfWidth() << endl;
  cout << "Half Boxcar Height:     " << f.HalfHeight() << endl;
  cout << "Samples in Line:        " << f.Samples() << endl;
  cout << endl;

  cout << "Average[0]:             " << f.Average(0) << endl;
  cout << "Average[1]:             " << f.Average(1) << endl;
  cout << "Average[2]:             " << f.Average(2) << endl;
  cout << "Average[3]:             " << f.Average(3) << endl;
  cout << "Variance[0]:            " << f.Variance(0) << endl;
  cout << "Variance[1]:            " << f.Variance(1) << endl;
  cout << "Variance[2]:            " << f.Variance(2) << endl;
  cout << "Variance[3]:            " << f.Variance(3) << endl;
  cout << "Count[0]:               " << f.Count(0) << endl;
  cout << "Count[1]:               " << f.Count(1) << endl;
  cout << "Count[2]:               " << f.Count(2) << endl;
  cout << "Count[3]:               " << f.Count(3) << endl;
  cout << endl;
  
  cout << "Changing Valid Count" << endl;
  f.SetMinimumPixels(11);
  cout << "Average[0]:             " << f.Average(0) << endl;
  cout << "Average[1]:             " << f.Average(1) << endl;
  cout << "Average[2]:             " << f.Average(2) << endl;
  cout << "Average[3]:             " << f.Average(3) << endl;
  cout << "Variance[0]:            " << f.Variance(0) << endl;
  cout << "Variance[1]:            " << f.Variance(1) << endl;
  cout << "Variance[2]:            " << f.Variance(2) << endl;
  cout << "Variance[3]:            " << f.Variance(3) << endl;
  cout << "Count[0]:               " << f.Count(0) << endl;
  cout << "Count[1]:               " << f.Count(1) << endl;
  cout << "Count[2]:               " << f.Count(2) << endl;
  cout << "Count[3]:               " << f.Count(3) << endl;
  cout << endl;

  cout << "Unloading data" << endl;
  f.RemoveLine(a);
  f.RemoveLine(a);
  f.RemoveLine(a);
  f.RemoveLine(a);
  f.RemoveLine(a);
  cout << "Average[0]:             " << f.Average(0) << endl;
  cout << "Average[1]:             " << f.Average(1) << endl;
  cout << "Average[2]:             " << f.Average(2) << endl;
  cout << "Average[3]:             " << f.Average(3) << endl;
  cout << "Variance[0]:            " << f.Variance(0) << endl;
  cout << "Variance[1]:            " << f.Variance(1) << endl;
  cout << "Variance[2]:            " << f.Variance(2) << endl;
  cout << "Variance[3]:            " << f.Variance(3) << endl;
  cout << "Count[0]:               " << f.Count(0) << endl;
  cout << "Count[1]:               " << f.Count(1) << endl;
  cout << "Count[2]:               " << f.Count(2) << endl;
  cout << "Count[3]:               " << f.Count(3) << endl;
  cout << endl;
  
  cout << "Changing Valid Range" << endl;
  f.SetMinMax (1.0,2.0);
  f.SetMinimumPixels(1);
  f.AddLine(a);
  f.AddLine(a);
  f.AddLine(a);
  f.AddLine(a);
  f.AddLine(a);
  cout << "Average[0]:             " << f.Average(0) << endl;
  cout << "Average[1]:             " << f.Average(1) << endl;
  cout << "Average[2]:             " << f.Average(2) << endl;
  cout << "Average[3]:             " << f.Average(3) << endl;
  cout << "Variance[0]:            " << f.Variance(0) << endl;
  cout << "Variance[1]:            " << f.Variance(1) << endl;
  cout << "Variance[2]:            " << f.Variance(2) << endl;
  cout << "Variance[3]:            " << f.Variance(3) << endl;
  cout << "Count[0]:               " << f.Count(0) << endl;
  cout << "Count[1]:               " << f.Count(1) << endl;
  cout << "Count[2]:               " << f.Count(2) << endl;
  cout << "Count[3]:               " << f.Count(3) << endl;
  cout << endl;


  cout << "Testing errors" << endl;
  // Band number of samples in line
  try {
    Isis::QuickFilter f2(0,3,3);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }
  
  // Bad boxcar width
  try {
    Isis::QuickFilter f2(5,0,3);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }

  // Bad boxcar height
  try {
    Isis::QuickFilter f2(5,3,0);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }

  // Width not odd
  try {
    Isis::QuickFilter f2(5,2,3);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }
  
  // Height not odd
  try {
    Isis::QuickFilter f2(5,3,2);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }

  // Minimum = maximum
  try {
    f.SetMinMax(1.0,1.0);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }
  
  // Minimum > maximum
  try {
    f.SetMinMax(2.0,1.0);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }

  // Bad number of valid points
  try {
    f.SetMinimumPixels(0);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }

  // Add too much data
  try {
    f.AddLine(a);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }
}
