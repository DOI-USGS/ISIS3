#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

#include "Spice.h"
#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"
#include "Quaternion.h"
#include "iException.h"
#include "Preference.h"

using namespace std;

int main () {
  Isis::Preference::Preferences(true);

  // Test the matrix constructor
  std::vector<double> inMat(9);
  std::vector<double> outMat(9);

  //call eul2m to make a matrix and fill the vector
  eul2m_c (0, 77.2*rpd_c(), -100.94*rpd_c(), 1, 3, 1, 
           (SpiceDouble (*)[3]) (&inMat[0]) );
  
  Isis::Quaternion q1 ( inMat );

  outMat = q1.ToMatrix();

  // Take care of Solaris round-off.  Find a better way later.
  if (abs(outMat[6]) < .000000000000001) outMat[6] = 0.; 

  // compare inMat and outMat
  cout << " Input matrix:"<<inMat[0]<<" "<<inMat[1]<<" "<<inMat[2] << endl
       << "              "<<inMat[3]<<" "<<inMat[4]<<" "<<inMat[5]<< endl
       << "              "<<inMat[6]<<" "<<inMat[7]<<" "<<inMat[8]<< endl;
  cout << "Output Matrix:"<<outMat[0]<<" "<<outMat[1]<<" "<<outMat[2]<< endl
       << "              "<<outMat[3]<<" "<<outMat[4]<<" "<<outMat[5]<< endl
       << "              "<<outMat[6]<<" "<<outMat[7]<<" "<<outMat[8]<< endl;

  // compare inquat and q1
  std::vector<double> inquat(4);
  m2q_c ( (SpiceDouble *) &inMat[0], (SpiceDouble *) &inquat[0]);
  cout << " Naif quaternion from matrix:  "<<" "<<inquat[0]<<" "<<inquat[1]<<" "
       << inquat[2]<<
" "<<inquat[3]<<endl;
  cout << " Class quaternion:             "<<" "<<q1[0]<<" "<<q1[1]<<" "<<q1[2]
       <<" "<<q1[3] << endl;

  // compare angles
  double cvt = dpr_c();
  std::vector<double> angles =  q1.ToAngles( 1, 3, 1 );

  // Take care of Solaris round-off.  Find a better way later.
  if (abs(angles[2])< .000000000000001) angles[2] = 0.; 

  cout << "Output angles: "<<angles[0]*cvt<<" "<<angles[1]*cvt<<" "
       <<angles[2]*cvt<<endl;
  cout << " Input angles: -100.94 77.2 0."<<endl;


  //Test the quaternion constructor
  Isis::Quaternion q2 ( inquat );
  cout << "Class constructed quaternion:  "<<" "<<q2[0]<<" "<<q2[1]<<" "<<q2[2]
       <<" "<<q2[3] << endl;
  


  // Test the empty constructor
  Isis::Quaternion q3;
  cout << "Empty Quaternion:  " << q3[0] << " " << q3[1] << " " << q3[2] << " "
       << q3[3] << endl;


  // Test = operator
  q3 = q2;
  cout << "Filled Quaternion:  "<< q3[0] << " " << q3[1] << " " << q3[2] << " "
       << q3[3] << endl;


  // Test *= operator
  std::vector<double> multMat(9);
  
  multMat[0] = 0.;
  multMat[1] = 1.;
  multMat[2] = 0.;
  multMat[3] = -1.;
  multMat[4] = 0.;
  multMat[5] = 0.;
  multMat[6] = 0.;
  multMat[7] = 0.;
  multMat[8] = 1.;
  
  Isis::Quaternion multQ ( multMat );

  mxm_c ( (SpiceDouble *) &inMat[0], (SpiceDouble *) &multMat[0], 
        (SpiceDouble (*) [3]) &outMat[0]);
        
  SpiceDouble naifQ[4];
  
  m2q_c ( (SpiceDouble *) &outMat[0], naifQ);
  q2 *= multQ;
  cout << "Naif mult  :  " << naifQ[0] << " " << naifQ[1] << " " << naifQ[2] <<
       " " << naifQ[3] << endl;
  cout << "Quat mult*=:  " << q2[0] << " " << q2[1] << " " << q2[2] << " " 
       << q2[3] << endl;

  // Test the * operator with a quaternion
  Isis::Quaternion q4;

  q4  =  q3 * multQ;  

  cout << "Quat mult: * :" << q4[0] << " " << q4[1] << " " << q4[2] << " "
       << q4[3] << endl;

  // Test the * operator with a scalar
  Isis::Quaternion q5;

  q5 = q1 * 2.;
  cout << "Quat scalar mult: " << q5[0] << " " << q5[1] << " " << q5[2] << " "
       << q5[3] << endl;
  

  // Test the conjugate method
  Isis::Quaternion q6 = q5.Conjugate();

  cout << "Conjugate of above is: " << q6[0] << " " << q6[1] << " " << q6[2] 
       << " " << q6[3] << endl;

  // Test the qxv method
  std::vector<double> vecIn(3);
  vecIn[0] = 1.;
  vecIn[1] = 1.;
  vecIn[2] = 1.;

  cout << vecIn[0] << " " << vecIn[1] << " " << vecIn[2] << endl;
  
  std::vector<double> vecOut = q6.Qxv(vecIn);

  cout << "qxv output = " << vecOut[0] << " " << vecOut[1] << " " << vecOut[2]
       << endl;

  std::vector<double> mymat = q6.ToMatrix();

  cout << "mymat = " << mymat[0] << " " << mymat[1] << " " << mymat[2] << endl
    << "         " << mymat[3] << " " << mymat[4] << " " << mymat[5] << endl
    << "         " << mymat[6] << " " << mymat[7] << " " << mymat[8] << endl;

  SpiceDouble myVecOut[3];
  mxv_c ( (SpiceDouble *) &mymat[0], (SpiceDouble *) &vecIn[0], myVecOut);

  cout << "my qxv output = " << myVecOut[0] << " " << myVecOut[1] << " " 
    << myVecOut[2]  << endl;
}
