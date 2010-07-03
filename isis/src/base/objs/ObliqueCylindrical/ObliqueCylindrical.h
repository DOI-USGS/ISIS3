#ifndef ObliqueCylindrical_h
#define ObliqueCylindrical_h
/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2010/02/08 19:02:07 $
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

namespace Isis {
/**                                                                       
 * @brief Oblique Cylindrical Map Projection                
 *                                                                        
 * This class provides methods for the forward and inverse equations of a Oblique 
 * Cylindrical map projection (for a sphere). The code was converted to C++ from
 * the Fortran version of the USGS General Cartographic Transformation Package 
 * (GCTP). In particular it was modified from the Simple
 * Cylindrical code. This class inherits Projection and provides
 * the four virtual methods Name(), SetGround (forward) and
 * SetCoordinate (inverse), XYRange (for obtaining projection
 * coordinate coverage for a latitude/longitude window) and the
 * == operator. Please see the Projection class for a full
 * accounting of all the methods available.
 * 
 * This projection works by moving the north pole of the simple cylindrical projection.
 * The pole latitude and longitude are the location of the new north pole, and the rotation
 * is the equivalent to the center longitude in simple cylindrical. 
 *                                                                        
 * @ingroup MapProjection                                                  
 *                                                                        
 * @author 2000-02-09 Jeff Anderson
 *                                                                        
 * @internal    
 *   @history 2007-06-19  Steven Lambright, Converted to ISIS3 and created XY Range search implementation
 *   @history 2007-06-29 Steven Lambright - Added Mapping, MappingLatitudes and
 *                                          MappingLongitudes methods.
 *   @history 2008-05-09 Steven Lambright - Added Name, Version methods
 *   @history 2010-02-08 Sharmila Prasad  - Removed testing p_latitude and p_longitude  in operator "=="
 */                                                                      
  class ObliqueCylindrical : public Isis::Projection {
    public:
      ObliqueCylindrical (Isis::Pvl &label, bool allowDefaults=false);
      ~ObliqueCylindrical ();
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
      std::string Name() const { return "ObliqueCylindrical"; }

      /** 
       * Returns the version of the map projection
       * 
       * 
       * @return std::string Version number
       */
      std::string Version () const { return "1.0"; }

      bool operator==(const Isis::Projection &proj);

      double GetPoleLatitude() const { return p_poleLatitude; };
      double GetPoleLongitude() const { return p_poleLongitude; };
      double GetPoleRotation() const { return p_poleRotation; };
  
    private:
      void init();

      void doSearch(double minBorder, double maxBorder, double &extremeVal, const double constBorder,
                    bool searchX, bool searchLongitude, bool findMin);

      void findExtreme(double &newMinBorder, double &newMaxBorder, double &minVal, double &maxVal, const double constBorder, 
                      bool searchX, bool searchLongitude, bool findMin);
      inline void SetSearchGround(const double variableBorder, const double constBorder, bool variableIsLat);

      // These are the oblique projection pole values in degrees.
      double p_poleLatitude;   //!< The Oblique Pole Latitude
      double p_poleLongitude;  //!< The Oblique Pole Longitude
      double p_poleRotation;   //!< The Oblique Pole Rotation

      // These are necessary for calculating X/Y range with discontinuity
      std::vector<double> p_specialLatCases; //!< Constant Latitudes that intersect a discontinuitiy
      std::vector<double> p_specialLonCases; //!< Constant Longitudes that intersect a discontinuitiy

      // These vectors are not used by the projection
      std::vector<double> p_xAxisVector;
      std::vector<double> p_yAxisVector;
      std::vector<double> p_zAxisVector;
  };
};

#endif

