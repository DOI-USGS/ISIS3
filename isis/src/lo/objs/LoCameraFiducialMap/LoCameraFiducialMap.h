/**
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

#ifndef LoCameraFiducialMap_h
#define LoCameraFiducialMap_h

#include "PvlGroup.h"

using namespace std;

namespace Isis {
  namespace Lo { 
  /**
   * @brief Computes map from image coordinates to focal plane based on fiducials
   *
   * The LoCameraFiducialMap class allows for the computation of a transformation
   * from image coordinates (sample,line) to focal plane coordinates (x,y) for 
   * either the Lunar Orbiter High Resolution Camera or the Lunar Orbiter Medium
   * resolution camera for any of the last three Lunar Orbiter missions.  The
   * transformation map is an affine transformation defined by values written in
   * the Isis 3 Instrument group labels.
   *
   * This class will load the fiducial sample/line and x/y values from the labels,
   * compute the coefficients of the affine transformation, and place the 
   * coefficients in to the data pool.  Typically these values are read from an
   * iak, but for Lunar Orbiter they are frame dependent.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup LunarOrbiter
   *
   * @author 2007-07-17 Debbie A. Cook
   *
   * @internal
   *  @history 2007-07-17 Debbie A. Cook - Original Version
   *  @history 2007-11-01 Debbie A. Cook - Revised to handle medium resolution camera
   *  @history 2008-06-18 Steven Lambright - Fixed documentation
   */
    class LoCameraFiducialMap  {
      public:
        //! Constructor
        LoCameraFiducialMap ( PvlGroup &inst, const int naifIkCode );

        //! Destructor
        ~LoCameraFiducialMap (){}; 

        private:
          void ReadFiducials( PvlGroup &inst );
          void CreateTrans( int xdir );
          vector<double> p_fidSamples; //!< Image sample positions of fiducial map
          vector<double> p_fidLines;   //!< Image line positions of fiducial map
          vector<double> p_fidXCoords; //!< Focal plane X positions of fiducial map
          vector<double> p_fidYCoords; //!< Focal plane Y positions of fiducial map
          int p_naifIkCode;            //!< Naif instrument code

    }; 
  };
};
#endif
