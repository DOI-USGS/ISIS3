#ifndef VecFilter_h
#define VecFilter_h
/**
 * @file
 * $Date: 2009/03/17 16:58:23 $
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

#include <vector>

namespace Isis {
    /**
    * @brief This class is used to perform filter operations on vectors.
    *
    * This class performs boxcar filter operations on vectors. The boxcar
    * will be a one dimensional Nx1 boxcar where N is a positive odd 
    * integer.
    *
    * For an example of how the VecFilter object is used in %Isis, see the
    * hicubenorm.cpp application.
    *
    * @ingroup Statistics
    *
    * @author Janet Barrett - 2009-03-13
    *
    */
  class VecFilter {
    public:
      VecFilter ();
      ~VecFilter ();

      std::vector<double> LowPass(std::vector<double> invec, int boxsize);
      std::vector<double> HighPass(std::vector<double> invec1, std::vector<double> invec2);

    private:
  };
} // end namespace isis

#endif

