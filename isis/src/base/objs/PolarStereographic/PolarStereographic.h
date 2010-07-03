#ifndef PolarStereographic_h
#define PolarStereographic_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/05/09 18:49:25 $
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

#include "Projection.h"
#include "Constants.h"

namespace Isis {
/**                                                                       
 * @brief Stereographic Map Projection                
 *                                                                        
 * This class provides methods for the forward and inverse equations of a Polar 
 * Stereographic map projection (for an ellipsoid). The code was converted to 
 * C++ from the Fortran version of the USGS General Cartographic Transformation 
 * Package (GCTP). This class inherits Projection and provides the two virtual 
 * methods SetGround (forward) and SetCoordinate (inverse) and a third virtual 
 * method, XYRange, for obtaining projection coordinate coverage for a 
 * latitude/longitude window. Please see the Projection class for a full 
 * accounting of all the methods available.                                     
 *                                                                        
 * @ingroup MapProjection                                                  
 *                                                                        
 * @author 2004-02-24 Jeff Anderson                                                                           
 *                                                                        
 * @internal                                                              
 *  @history 2004-02-24 Jeff Anderson - Fixed a bug in TrueScaleLatitude and 
 *                                      changed default computation for
 *                                      CenterLatitude
 *  @history 2005-03-01 Elizabeth Ribelin - Modified file to support Doxygen 
 *                                          documentation
 *  @history 2005-03-11 Elizabeth Ribelin - added TrueScaleLatitude method test 
 *                                          to unitTest
 *  @history 2006-06-14 Elizabeth Miller - Added error check to make sure the
 *                                         center latitude is not zero
 *  @history 2007-06-29 Steven Lambright - Added Mapping, MappingLatitudes and
 *                                          MappingLongitudes methods.
 *  @history 2008-05-09 Steven Lambright - Added Name, Version methods
 */                                                                       
  class PolarStereographic : public Isis::Projection {
    public:
      PolarStereographic (Isis::Pvl &label, bool allowDefaults=false);
      ~PolarStereographic ();
      bool SetGround (const double lat, const double lon);
      bool SetCoordinate (const double x, const double y);
      bool XYRange (double &minX, double &maxX, double &minY, double &maxY);

      PvlGroup Mapping();
      PvlGroup MappingLatitudes();
      PvlGroup MappingLongitudes();

      /**
       * Returns the name of the map projection
       *
       * @return string Name of projection
       */
      std::string Name() const { return "PolarStereographic"; }

      /** 
       * Returns the version of the map projection
       * 
       * 
       * @return std::string Version number
       */
      std::string Version () const { return "1.0"; }

      bool operator== (const Isis::Projection &proj);

     /** 
      * Returns the latitude of true scale (in the case of Polar Stereographic
      * it is the center latitude)
      * 
      * @return double
      */
      double TrueScaleLatitude () const { 
        return p_centerLatitude * 180.0 / Isis::PI; 
      };
  
    private:
      double p_centerLongitude;  //!<The center longitude for the map projection
      double p_centerLatitude;   //!<The center latitude for the map projection
  
      double p_t;
      double p_m;
      double p_e4;
      double p_signFactor;
      double p_poleFlag;
  };
};

#endif

