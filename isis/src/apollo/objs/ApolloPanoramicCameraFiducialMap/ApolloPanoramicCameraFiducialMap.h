#ifndef ApolloPanoramicCameraFiducialMap_h
#define ApolloPanoramicCameraFiducialMap_h
/**
 * @file 
 *  
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

#include "PvlGroup.h"

using namespace std;

namespace Isis {
  class Affine;
  /**
   * @brief Computes map from image coordinates to focal plane based on fiducials
   *
   * The ApolloPanoramicCameraFiducialMap class allows for the computation of a transformation
   * from image coordinates (sample,line) to focal cylinder coordinates (x,y) for
   * the Apollo 15, 16, 17 Panoramic Cameras The
   * transformation map is an affine transformation defined by values written in
   * the Isis 3 Instrument group labels.
   *
   * This class will load the fiducial sample/line and x/y values from the labels,
   * compute the coefficients of the affine transformation, and place the
   * coefficients in to the data pool.  Typically these values are read from an
   * iak, but for Lunar Orbiter they are frame dependent.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup ApolloPanoramic
   *
   * @author 2016-09-07 Ken Edmundson
   *
   * @internal
   *   @history 2016-09-07 Ken Edmundson - Original Version
   */
  class ApolloPanoramicCameraFiducialMap  {
    public:
      ApolloPanoramicCameraFiducialMap(PvlGroup &inst, const int naifIkCode);
      //! Destroys ApolloPanoramicCameraFiducialMap object.
      ~ApolloPanoramicCameraFiducialMap() {};

      Affine* CreateTrans();

    private:
      void ReadFiducials(PvlGroup &inst);
      std::vector<double> m_fidMeasuredSamples;   //!< Image sample positions of fiducial map
      std::vector<double> m_fidMeasuredLines;     //!< Image line positions of fiducial map
      std::vector<double> m_fidCalibratedSamples; //!< Focal plane X positions of fiducial map
      std::vector<double> m_fidCalibratedLines;   //!< Focal plane Y positions of fiducial map

      int p_naifIkCode;            //!< Naif instrument code

  };
};
#endif
