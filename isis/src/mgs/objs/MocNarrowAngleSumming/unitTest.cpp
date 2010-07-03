using namespace std;

#include <iostream>
#include <iomanip>

#include "MocNarrowAngleSumming.h"
#include "Preference.h"
int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "Unit test for Isis::Mgs::MocNarrowAngleSumming" << endl;

  cout << "Testing csum=1, ss=1" << endl;
  Isis::Mgs::MocNarrowAngleSumming s1(1,1);
  cout << s1.Detector(1.0) << endl;
  cout << s1.Detector(2.0) << endl;
  cout << s1.Detector(3.0) << endl;
  cout << s1.Sample(s1.Detector(1.0)) << endl;
  cout << s1.Sample(s1.Detector(2.0)) << endl;
  cout << s1.Sample(s1.Detector(3.0)) << endl;
  cout << endl;

  cout << "Testing csum=2, ss=1" << endl;
  Isis::Mgs::MocNarrowAngleSumming s2(2,1);
  cout << s2.Detector(1.0) << endl;
  cout << s2.Detector(2.0) << endl;
  cout << s2.Detector(3.0) << endl;
  cout << s2.Sample(s2.Detector(1.0)) << endl;
  cout << s2.Sample(s2.Detector(2.0)) << endl;
  cout << s2.Sample(s2.Detector(3.0)) << endl;
  cout << endl;

  cout << "Testing csum=3, ss=10" << endl;
  Isis::Mgs::MocNarrowAngleSumming s3(3,10);
  cout << s3.Detector(1.0) << endl;
  cout << s3.Detector(2.0) << endl;
  cout << s3.Detector(3.0) << endl;
  cout << s3.Sample(s3.Detector(1.0)) << endl;
  cout << s3.Sample(s3.Detector(2.0)) << endl;
  cout << s3.Sample(s3.Detector(3.0)) << endl;
}

