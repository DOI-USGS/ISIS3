#ifndef Equirectangular_h
#define Equirectangular_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.5 $                                                             
 * $Date: 2008/11/13 15:56:28 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       

#include "Projection.h"

namespace Isis {
/**                                                                       
 * @brief Equirectangular Map Projection                
 *                                                                        
 * This class provides methods for the forward and inverse equations of a 
 * Equirectangular map projection (for a sphere).  The code was converted to 
 * C++ from the Fortran version of the USGS General Cartographic Transformation 
 * Package (GCTP).  In particular it was modified from the Equidistant 
 * Cylindrical code. This class inherits IsisProjection and provides the two 
 * virtual methods SetGround (forward) and SetCoordinate (inverse) and a third 
 * virtual method, XYRange, for obtaining projection coordinate coverage for a 
 * latitude/longitude window.  Please see the IsisProjection class for a full 
 * accounting of all the methods available.
 *             
 *                                                                        
 * @ingroup MapProjection                                                  
 *                                                                        
 * @author 2003-11-13 Jeff Anderson 
 *                                                                        
 * @internal                                                              
 *   @history 2004-02-07 Jeff Anderson - added plug-in capability.
 *   @history 2004-02-24 Jeff Anderson - Modified forward and inverse methods to 
 *                                       use the local radius at the center
 *                                       latitude instead of the equitorial
 *                                       radius.
 *   @history 2005-03-11 Elizabeth Ribelin - added TrueScaleLatitude method test
 *                                           to the unitTest
 *   @history 2007-06-29 Steven Lambright - Added Mapping, MappingLatitudes and
 *                                          MappingLongitudes methods.
 *   @history 2008-05-09 Steven Lambright - Added Name, Version, IsEquatorialCylindrical methods
 *   @history 2008-05-09 Steven Lambright - Fixed test for being too close to a
 *                                           pole
 *   @history 2008-11-12 Steven Lambright - Commented some unclear code
 *                                          (CenterLatitudeRadius keyword)
 */                                                                       
  class Equirectangular : public Projection {
    public:
      Equirectangular (Isis::Pvl &label, bool allowDefaults=false);
      ~Equirectangular ();
      bool SetGround (const double lat, const double lon);
      bool SetCoordinate (const double x, const double y);
      bool XYRange (double &minX, double &maxX, double &minY, double &maxY);
      virtual PvlGroup Mapping();
      virtual PvlGroup MappingLatitudes();
      virtual PvlGroup MappingLongitudes();

      /**
       * Returns the name of the map projection
       *
       * @return string Name of projection
       */
      std::string Name() const { return "Equirectangular"; }

      /** 
       * Returns the version of the map projection
       * 
       * 
       * @return std::string Version number
       */
      std::string Version () const { return "1.0"; }

      bool IsEquatorialCylindrical() { return true; }
      bool operator==(const Isis::Projection &proj);
      double TrueScaleLatitude() const;
  
    private:
      double p_centerLongitude;  //!<The center longitude for the map projection
      double p_centerLatitude;   //!<The center latitude for the map projection
      double p_cosCenterLatitude;//!<Cosine of the center latitude
      double p_clatRadius;
  };
};
  
#endif

