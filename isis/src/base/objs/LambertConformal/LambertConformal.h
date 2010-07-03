#ifndef LambertConformal_h
#define LambertConformal_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.5 $                                                             
 * $Date: 2009/03/20 22:30:23 $                                                                 
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
 * @brief Lambert Conformal Map Projection                
 *                                                                        
 * This class provides methods for the forward and inverse equations of a 
 * Lambert Conformal map projection (for an ellipse). The code was converted 
 * to C++ from the C version of the USGS General Cartographic Transformation 
 * Package (GCTP). This class inherits Projection and provides the two virtual 
 * methods SetGround (forward) and SetCoordinate (inverse) and a third virtual 
 * method, XYRange, for obtaining projection coordinate coverage for a 
 * latitude/longitude window. Please see the Projection class for a full 
 * accounting of all the methods available.
 *                                                                                                                                                 
 * @ingroup MapProjection                                                  
 *                                                                        
 * @author 2005-03-29 Elizabeth Ribelin                                                                           
 *                                                                        
 * @internal 
 *   @history 2007-06-29 Steven Lambright - Added Mapping, MappingLatitudes and
 *                                          MappingLongitudes methods.
 *   @history 2008-05-09 Steven Lambright - Added Name, Version methods
 *   @history 2008-08-15 Stuart Sides - Modified to allow standard parallels to
 *            be in any order. Modified to not accept center latitudes too close
 *            to either pole.
 *   @history 2009-03-20 Stuart Sides - Modified to not accept center latitudes
 *                       near the pole opposite the apex of the cone
 */                                                                       
  class LambertConformal : public Isis::Projection {
    public:
      LambertConformal(Isis::Pvl &label, bool allowDefaults=false);
      ~LambertConformal();
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
      std::string Name() const { return "LambertConformal"; }

      /** 
       * Returns the version of the map projection
       * 
       * 
       * @return std::string Version number
       */
      std::string Version () const { return "1.0"; }

      bool operator== (const Isis::Projection &proj);

      /** 
       * Returns the latitude of true scale (in the case of LambertConformal
       * it is the smaller of the two standard parallels)
       * 
       * @return double
       */
      double TrueScaleLatitude () const {
        if (p_par1 > p_par2) return p_par2 * 180.0 / Isis::PI;
        else return p_par1 * 180.0 / Isis::PI; 
      };

  
    private:
      double p_centerLongitude; //!<The center longitude for the map projection
      double p_centerLatitude;  //!<The center latitude for the map projection
      double p_par1;            //!<The first standard parallel
      double p_par2;            //!<The second standard parallel
      double p_n, p_f, p_rho;   //!<Snyder's n, f, and rho

  };
};

#endif                            
