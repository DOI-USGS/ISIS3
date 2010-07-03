#include <iostream>
#include "Statistics.h"
#include "iException.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[])
{
  Isis::Preference::Preferences(true);

  Isis::Statistics s;
  s.SetValidRange(1.0, 6.0);

  s.AddData(5.0);
  s.AddData(5.0);
  s.Average();
  cout << "Average:        " << s.Average() << endl;
  s.Reset();

  double a[10];
  a[0] = 1.0;
  a[1] = 2.0;
  a[2] = 3.0;
  a[3] = Isis::NULL8;
  a[4] = Isis::HIGH_REPR_SAT8;
  a[5] = Isis::LOW_REPR_SAT8;
  a[6] = Isis::HIGH_INSTR_SAT8;
  a[7] = Isis::LOW_INSTR_SAT8;
  a[8] = 10.0;
  a[9] = -1.0;

  s.AddData(a,8);
  cout << "Average:        " << s.Average() << endl;
  cout << "Variance:       " << s.Variance() << endl;
  cout << "Std Deviation:  " << s.StandardDeviation() << endl;
  cout << "Minimum:        " << s.Minimum() << endl;
  cout << "Maximum:        " << s.Maximum() << endl;
  cout << "ChebyShev Min:  " << s.ChebyshevMinimum() << endl;
  cout << "ChebyShev Max:  " << s.ChebyshevMaximum() << endl;
  cout << "Best Minimum:   " << s.BestMinimum() << endl;
  cout << "Best Maximum:   " << s.BestMaximum() << endl;
  cout << "Total Pixels:   " << s.TotalPixels() << endl;
  cout << "Valid Pixels:   " << s.ValidPixels() << endl;
  cout << "Null Pixels:    " << s.NullPixels() << endl;
  cout << "Lis Pixels:     " << s.LisPixels() << endl;
  cout << "Lrs Pixels:     " << s.LrsPixels() << endl;
  cout << "His Pixels:     " << s.HisPixels() << endl;
  cout << "Hrs Pixels:     " << s.HrsPixels() << endl;
  cout << "Sum:            " << s.Sum() << endl;
  cout << "SumSquare:      " << s.SumSquare() << endl;
  cout << endl;

  double b[4];
  b[0] = 4.0;
  b[1] = 5.0;
  b[2] = 6.0;
  b[3] = Isis::NULL8;

  s.AddData (b,4);
  cout << "Average:        " << s.Average() << endl;
  cout << "Variance:       " << s.Variance() << endl;
  cout << "Std Deviation:  " << s.StandardDeviation() << endl;
  cout << "Minimum:        " << s.Minimum() << endl;
  cout << "Maximum:        " << s.Maximum() << endl;
  cout << "ChebyShev Min:  " << s.ChebyshevMinimum() << endl;
  cout << "ChebyShev Max:  " << s.ChebyshevMaximum() << endl;
  cout << "Best Minimum:   " << s.BestMinimum() << endl;
  cout << "Best Maximum:   " << s.BestMaximum() << endl;
  cout << "Total Pixels:   " << s.TotalPixels() << endl;
  cout << "Valid Pixels:   " << s.ValidPixels() << endl;
  cout << "Null Pixels:    " << s.NullPixels() << endl;
  cout << "Lis Pixels:     " << s.LisPixels() << endl;
  cout << "Lrs Pixels:     " << s.LrsPixels() << endl;
  cout << "His Pixels:     " << s.HisPixels() << endl;
  cout << "Hrs Pixels:     " << s.HrsPixels() << endl;
  cout << "Sum:            " << s.Sum() << endl;
  cout << "SumSquare:      " << s.SumSquare() << endl;
  cout << endl;

  s.RemoveData (a,3);
  cout << "Average:        " << s.Average() << endl;
  cout << "Variance:       " << s.Variance() << endl;
  cout << "Std Deviation:  " << s.StandardDeviation() << endl;
  cout << "ChebyShev Min:  " << s.ChebyshevMinimum() << endl;
  cout << "ChebyShev Max:  " << s.ChebyshevMaximum() << endl;
  cout << "Total Pixels:   " << s.TotalPixels() << endl;
  cout << "Valid Pixels:   " << s.ValidPixels() << endl;
  cout << "Null Pixels:    " << s.NullPixels() << endl;
  cout << "Lis Pixels:     " << s.LisPixels() << endl;
  cout << "Lrs Pixels:     " << s.LrsPixels() << endl;
  cout << "His Pixels:     " << s.HisPixels() << endl;
  cout << "Hrs Pixels:     " << s.HrsPixels() << endl;
  cout << "Sum:            " << s.Sum() << endl;
  cout << "SumSquare:      " << s.SumSquare() << endl;
  cout << endl;

  s.Reset();
  cout << "Average:        " << s.Average() << endl;
  cout << "Variance:       " << s.Variance() << endl;
  cout << "Std Deviation:  " << s.StandardDeviation() << endl;
  cout << "ChebyShev Min:  " << s.ChebyshevMinimum() << endl;
  cout << "ChebyShev Max:  " << s.ChebyshevMaximum() << endl;
  cout << "Total Pixels:   " << s.TotalPixels() << endl;
  cout << "Valid Pixels:   " << s.ValidPixels() << endl;
  cout << "Null Pixels:    " << s.NullPixels() << endl;
  cout << "Lis Pixels:     " << s.LisPixels() << endl;
  cout << "Lrs Pixels:     " << s.LrsPixels() << endl;
  cout << "His Pixels:     " << s.HisPixels() << endl;
  cout << "Hrs Pixels:     " << s.HrsPixels() << endl;
  cout << "Sum:            " << s.Sum() << endl;
  cout << "SumSquare:      " << s.SumSquare() << endl;
  cout << endl;

  try {
    s.RemoveData (a,8);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }

  try {
    s.Minimum ();
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }

  try {
    s.Maximum ();
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }
  
  try {
    s.ChebyshevMinimum (0.0);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }

  try {
    s.ChebyshevMaximum (100.0);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }
}
