/**
 * @file
 * $Date: 2009/03/17 16:58:22 $
 * $Revision: 1.1 $
 *
 *  Unless noted otherwise, the portions of Isis written by the USGS are public domain. See
 *  individual third-party library and package descriptions for intellectual property information,
 *  user agreements, and related information.
 *
 *  Although Isis has been used by the USGS, no warranty, expressed or implied, is made by the
 *  USGS as to the accuracy and functioning of such software and related material nor shall the
 *  fact of distribution constitute any such warranty, and no responsibility is assumed by the
 *  USGS in connection therewith.
 *
 *  For additional information, launch $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *  in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *  http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *  http://www.usgs.gov/privacy.html.
 */

#include <float.h>
#include <string>
#include <vector>
#include "VecFilter.h"
#include "IException.h"
#include "iString.h"

using namespace std;
const int MARKER = -999999;
namespace Isis {
  //! Constructs a VecFilter object.
  VecFilter::VecFilter() {}

  //! Destroys the VecFilter object.
  VecFilter::~VecFilter() {};

  /**
   * Perform a lowpass filter on an input vector.
   *
   * @param invec The input vector on which the lowpass filter will be
   *    performed.
   *
   * @param boxsize The size of the one dimensional boxcar to use in
   *    doing the lowpass filter. The filter size must be odd and
   *    greater than 1.
   *
   */
  vector<double> VecFilter::LowPass(vector<double> invec, int boxsize) {
    vector<double> outvec;

    // Clear the output vector
    //outvec.resize(0);

    // Boxcar size must be odd and greater than 1
    if((boxsize % 2) == 0) {
      string m = "Boxcar size must be odd and greater than 1 in [VecFilter::LowPass]";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    // Perform lowpass filter
    int halfwidth = boxsize / 2;
    int vecsize = (int)invec.size() - 1;
    for(int i = 0; i <= vecsize; i++) {
      int i1 = i - halfwidth;
      if(i1 < 0) i1 = 0;
      int i2 = i + halfwidth;
      if(i2 > vecsize) i2 = vecsize;
      int npts = 0;
      double sum = 0.0;
      for(int j = i1; j <= i2; j++) {
        if(invec[j] != 0.0) {
          sum = sum + invec[j];
          npts++;
        }
      }
      if(npts > 0) {
        outvec.push_back(sum / npts);
      }
      else {
        outvec.push_back(0.0);
      }
    }
    return outvec;
  }

  /**
   * Perform a highpass filter by subtracting one vector (the lowpass
   * filtered vector) from the original vector.
   *
   * @param invec1 The vector that contains the original data before
   *    the lowpass was applied.
   *
   * @param invec2 The vector which has gone through a lowpass filter.
   *
   */
  vector<double> VecFilter::HighPass(vector<double> invec1, vector<double> invec2) {
    vector<double> outvec;

    // Both vectors must be the same size
    if(invec1.size() != invec2.size()) {
      string m = "Both vectors must be the same size in [VecFilter::HighPass]";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    int vecsize = (int)invec1.size() - 1;
    for(int i = 0; i <= vecsize; i++) {
      if(invec1[i] != 0.0 && invec2[i] != 0.0) {
        outvec.push_back(invec1[i] - invec2[i]);
      }
      else {
        outvec.push_back(0.0);
      }
    }
    return outvec;
  }

  /**
   * Highpass specifically used in hicubenorm where the cubenorm stats are
   * manipulated. Highpass filtering with Subtract/Divide mode is done if the
   * original and resultant lowpass vectors are non-zero and valid points vector
   * has max valid points else that index is Marked as having isufficient valid
   * points for later processing
   *
   * @author Sharmila Prasad (1/28/2011)
   *
   * @param pdInVector1       - Original data vector
   * @param pdInVector2       - Vector after the lowpass filter
   * @param piValidPntsVector - Valid pixels vector
   * @param piMaxPoints       - Max Valid pixels
   * @param psMode            - Mode Subtract/Divide
   *
   * @return vector<double>   - Resulting vector after highpass
   */
  vector<double> VecFilter::HighPass(vector<double> pdInVector1, vector<double> pdInVector2,
                 vector<int> piValidPntsVector, int piMaxPoints, const iString & psMode)
  {
    vector<double> dOutVector;

    // Both vectors must be the same size
    if(pdInVector1.size() != pdInVector2.size()) {
      string m = "Both vectors must be the same size in [VecFilter::HighPass]";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    int iSize = (int)pdInVector1.size();
    for(int i = 0; i < iSize; i++) {
      if(pdInVector1[i] != 0.0 && pdInVector2[i] != 0.0 && piValidPntsVector[i]==piMaxPoints) {
        if(psMode == "SUBTRACT") {
          dOutVector.push_back(pdInVector1[i] - pdInVector2[i]);
        }
        else {
          dOutVector.push_back(pdInVector1[i] / pdInVector2[i]);
        }
      }
      else {
        dOutVector.push_back(MARKER);
      }
    }
    return dOutVector;
  }

} // end namespace isis
