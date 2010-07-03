#ifndef LimitPolygonSeeder_h
#define LimitPolygonSeeder_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.8 $                                                             
 * $Date: 2010/05/05 21:24:30 $                                                                 
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

#include "geos/geom/Point.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/Polygon.h"
#include "PolygonSeeder.h"

namespace Isis {
  class Pvl;

  /**                                                                       
   * @brief Seed points using a grid
   *                                                                        
   * This class seeds the polygons with Control Points by creating a grid 
   *  centered on the polygon. For each grid square, if it contains any overlap, a
   *  box is then created within the grid square, surrounding the valid data. The
   *  point checked is the center of this box, and if this point is within the
   *  overlap polygon then this point is returned, otherwise the grid square does
   *  not have a point.
   *  
   * @ingroup PatternMatching
   *                                                                        
   * @author  2008-04-21 Steven Lambright 
   *  
   * @internal 
   *   @history 2008-08-18 Christopher Austin - Upgraded to geos3.0.0 
   *   @history 2008-11-12 Steven Lambright - Fixed documentation
   *   @history 2008-11-25 Steven Lambright - Added error checking
   *   @history 2008-12-23 Steven Lambright - Fixed problem with finding points in
   *            polygons that caused this algorithm to miss some.
   *   @history 2009-08-05 Travis Addair - Encapsulated group 
   *            creation for seed definition group
   *   @history 2010-04-15 Eric Hyer - Now updates parent's invalidInput
   *                                   variable
   *   @history 2010-04-20 Christopher Austin - adapted for generic/unitless
   * 	                                        seeding
   */                                                                       
  class LimitPolygonSeeder : public PolygonSeeder {
    public:
      LimitPolygonSeeder (Pvl &pvl);

      //! Destructor
      virtual ~LimitPolygonSeeder() {};

      std::vector<geos::geom::Point*> Seed(const geos::geom::MultiPolygon *mp); 

      virtual PvlGroup PluginParameters(std::string grpName);

    protected:
      virtual void Parse(Pvl &pvl);

    private:
      geos::geom::Geometry *GetMultiPolygon(double dMinX, double dMinY, 
                                            double dMaxX, double dMaxY, 
                                            const geos::geom::MultiPolygon &orig);
      int p_majorAxisPts; //!< Number of points to place on major axis
      int p_minorAxisPts; //!< Number of points to place on minor axis
  };
};

#endif
