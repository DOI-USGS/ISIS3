/**
 * @file
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include <iostream>
#include <iomanip>

#include "MocNarrowAngleSumming.h"
#include "Preference.h"

using namespace std;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "Unit test for Isis::MocNarrowAngleSumming" << endl;

  cout << "Testing csum=1, ss=1" << endl;
  Isis::MocNarrowAngleSumming s1(1, 1);
  cout << s1.Detector(1.0) << endl;
  cout << s1.Detector(2.0) << endl;
  cout << s1.Detector(3.0) << endl;
  cout << s1.Sample(s1.Detector(1.0)) << endl;
  cout << s1.Sample(s1.Detector(2.0)) << endl;
  cout << s1.Sample(s1.Detector(3.0)) << endl;
  cout << endl;

  cout << "Testing csum=2, ss=1" << endl;
  Isis::MocNarrowAngleSumming s2(2, 1);
  cout << s2.Detector(1.0) << endl;
  cout << s2.Detector(2.0) << endl;
  cout << s2.Detector(3.0) << endl;
  cout << s2.Sample(s2.Detector(1.0)) << endl;
  cout << s2.Sample(s2.Detector(2.0)) << endl;
  cout << s2.Sample(s2.Detector(3.0)) << endl;
  cout << endl;

  cout << "Testing csum=3, ss=10" << endl;
  Isis::MocNarrowAngleSumming s3(3, 10);
  cout << s3.Detector(1.0) << endl;
  cout << s3.Detector(2.0) << endl;
  cout << s3.Detector(3.0) << endl;
  cout << s3.Sample(s3.Detector(1.0)) << endl;
  cout << s3.Sample(s3.Detector(2.0)) << endl;
  cout << s3.Sample(s3.Detector(3.0)) << endl;
}

