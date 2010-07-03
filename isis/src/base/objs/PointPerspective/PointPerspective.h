#ifndef PointPerspective_h
#define PointPerspective_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/06/13 20:02:20 $                                                                 
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
 * @brief PointPerspective Map Projection                
 *                                                                        
 * This class provides methods for the forward and inverse equations of an 
 * PointPerspective map projection (for a sphere). The code was converted 
 * to C++ from the C version of the USGS General Cartographic Transformation 
 * Package (GCTP). This class inherits Projection and provides the two virtual 
 * methods SetGround (forward) and SetCoordinate (inverse) and a third virtual 
 * method, XYRange, for obtaining projection coordinate coverage for a 
 * latitude/longitude window. Please see the Projection class for a full 
 * accounting of all the methods available.                                                
 *                                                                                                          
 * @ingroup MapProjection                                                  
 *                                                                        
 * @author 2008-06-13 Tracie Sucharski                                          
 *                                                                        
 * @internal
 */                                                                       
  class PointPerspective : public Isis::Projection {
    public:
      PointPerspective(Isis::Pvl &label, bool allowDefaults=false);
      ~PointPerspective();
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
      std::string Name() const { return "PointPerspective"; }

      /** 
       * Returns the version of the map projection
       * 
       * 
       * @return std::string Version number
       */
      std::string Version () const { return "1.0"; }

      bool operator== (const Isis::Projection &proj);

     /** 
      * Returns the latitude of true scale (in the case of PointPerspective
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
      double p_distance;         //!<Distance fromp perspective point to planet center                         
      double sinph0;             //!<Sin of the center latitude
      double cosph0;             //!<Cos of the center latitude
      double p_P;                //!<Perspective Point
                                 
  };
};
#endif
