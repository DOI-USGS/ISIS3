/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>
#include <iostream>
#include <iomanip>

#include "VecFilter.h"
#include "IException.h"
#include "Preference.h"

using namespace std;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  Isis::VecFilter v;
  vector<double> filtorig;
  vector<double> filtlow;
  vector<double> filthigh;
  int boxsize;
  double indata[25] = {2602.6042, 2533.2345, 2547.0729, 2532.9832, 2543.8435,
                       2548.5079, 2557.5828, 2541.5233, 2553.8828, 2554.7766, 2563.4105, 2546.5308,
                       2554.9052, 2556.8047, 2563.0255, 2546.2865, 2554.3044, 2553.0937, 2564.721,
                       2545.9481, 2556.6338, 2556.4804, 2565.3126, 2546.2027, 2556.7211
                      };

  for(int i = 0; i < 25; i++) {
    filtorig.push_back(indata[i]);
  }

  boxsize = 3;
  filtlow = v.LowPass(filtorig, boxsize);
  filthigh = v.HighPass(filtorig, filtlow);

  cout << "Size of original vector: " << filtorig.size() << endl;
  cout << "Size of lowpass vector: " << filtlow.size() << endl;
  cout << "Size of highpass vector: " << filthigh.size() << endl;
  cout << "Filter size: " << boxsize << endl;

  cout << "Original vector values: " << endl;
  for(int i = 0; i < 25; i++) {
    cout << std::setw(10) << filtorig[i];
    if(i%5 == 4) {
      cout << endl;
    }
  }

  cout << endl<<  "Lowpass filtered vector values: " << endl;
  for(int i = 0; i < 25; i++) {
    cout << setw(10) << filtlow[i];
    if(i%5 == 4) {
      cout << endl;
    }
  }

  cout << endl << "Highpass filtered vector values: " << endl;
  for(int i = 0; i < 25; i++) {
    cout << setw(10) << filthigh[i];
    if(i%5 == 4) {
      cout << endl;
    }
  }

  filtlow.clear();
  filthigh.clear();

  boxsize = 5;
  filtlow = v.LowPass(filtorig, boxsize);
  filthigh = v.HighPass(filtorig, filtlow);

  cout << "\nSize of original vector: " << filtorig.size() << endl;
  cout << "Size of lowpass vector: " << filtlow.size() << endl;
  cout << "Size of highpass vector: " << filthigh.size() << endl;
  cout << "Filter size: " << boxsize << endl;

  cout << "Original vector values: " << endl;
  for(int i = 0; i < 25; i++) {
    cout << setw(10) << filtorig[i];
    if(i%5 == 4) {
      cout << endl;
    }
  }

  cout << "\nLowpass filtered vector values: " << endl;
  for(int i = 0; i < 25; i++) {
    cout << setw(10) << filtlow[i];
    if(i%5 == 4) {
      cout << endl;
    }
  }

  cout << "\nHighpass filtered vector values: " << endl;
  for(int i = 0; i < 25; i++) {
    cout << setw(10) << filthigh[i];
    if(i%5 == 4) {
      cout << endl;
    }
  }
  int iMaxPoints = 2, iTemp=0;
  vector<int>validPoints;
  for(int i = 0; i < 25; i++) {
    iTemp = i%3;
    if(!iTemp) {
      iTemp = 2;
    }
    validPoints.push_back(iTemp);
    
  }
  filthigh = v.HighPass(filtorig, filtlow, validPoints, iMaxPoints, "DIVIDE");
  
  cout << "\nSize of original vector: " << filtorig.size() << endl;
  cout << "Size of lowpass vector: " << filtlow.size() << endl;
  cout << "Size of highpass vector: " << filthigh.size() << endl;
  cout << "Filter size: " << boxsize << endl;
  
  cout << "Original vector values: " << endl;
  for(int i = 0; i < 25; i++) {
    cout << setw(10) << filtorig[i];
    if(i%5 == 4) {
      cout << endl;
    }
  }
  
  cout << "Valid Points vector values: " << endl;
  for(int i = 0; i < 25; i++) {
    cout << setw(5) << validPoints[i];
    if(i%5 == 4) {
      cout << endl;
    }
  }

  cout << "\nLowpass filtered vector values: " << endl;
  for(int i = 0; i < 25; i++) {
    cout << setw(10) << filtlow[i];
    if(i%5 == 4) {
      cout << endl;
    }
  }

  cout << "\nHighpass filtered vector values: " << endl;
  for(int i = 0; i < 25; i++) {
    cout << setw(10) << filthigh[i];
    if(i%5 == 4) {
      cout << endl;
    }
  }
  cout << endl;
}
