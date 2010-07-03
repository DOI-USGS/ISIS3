#ifndef WorldMapper_h
#define WorldMapper_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1.1.1 $                                                             
 * $Date: 2006/10/31 23:18:10 $                                                                 
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

namespace Isis {
/**                                                                       
 * @brief Create a mapping between a projection and other coordinate system               
 *                                                                        
 * This class can be used to create a mapping between a projection coordinate 
 * system and a different coordinate system. Simple examples of this would be a
 * mapping between projection x/y's to cube line/samples or a mapping between 
 * projection x/y's and paper x/y's in inches. The class is pure virtual and
 * therefore should never be directly instantiated. Remember the basic premise  
 * is to use this class to create a new class which converts coordinates from  
 * one system to another (inches to meters and back, meters to pixels and back, 
 * etc).
 * 
 * If you would like to see WorldMapper being used in implementation, 
 * see the ProjectionFactory class                                     
 *                                                                        
 * @ingroup MapProjection                                                 
 *                                                                        
 * @author 2003-01-28 Jeff Anderson                                                                           
 *                                                                        
 * @internal                                                              
 *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology... 
 *                                     isis.astrogeology...
 *  @history 2003-09-25 Jeff Anderson - Added Resolution method
 *  @history 2005-02-22 Elizabeth Ribelin - Modified file to support Doxygen 
 *                                          documentation
 *                                                                        
 *  @todo 2005-02-22 Jeff Anderson - add coded example to class documentation                                                     
 */                                                                       
  class WorldMapper {
    public:
      //! Constructs an empty WorldMapper object
      WorldMapper () {};
  
      //! Destroys the WorldMapper object
      virtual ~WorldMapper () {};
  
     /** 
      * This pure virtual method will return a projection X coordinate in meters
      * given a world X coordinate. For example, the worldX value could be an X 
      * in millimeters on a piece of paper. The value returned would be an X in 
      * map projection coordinates.
      *
      * @param worldX An X value in world coordinates.
      * 
      * @return double
      */
      virtual double ProjectionX (const double worldX) const = 0;
      
     /** 
      * This pure virtual method will return a projection Y coordinate in meters
      * given a world Y coordinate.  For example, the worldY value could be an
      * Y in millimeters on a piece of paper.  The value returned would be an
      * Y in map projection coordinates.
      *
      * @param worldY A Y value in world coordinates.
      * 
      * @return double
      */
      virtual double ProjectionY (const double worldY) const = 0;
      
     /** 
      * This pure virtual method will return a world X coordinate given a 
      * projection Y coordinate in meters. For example, the worldY value could 
      * be an Y in millimeters on a piece of paper.
      *
      * @param projectionX  A X value in projection coordinates.
      * 
      * @return double
      */
      virtual double WorldX (const double projectionX) const = 0;
  
     /** 
      * This pure virtual method will return a world Y coordinate given a 
      * projection Y coordinate in meters. For example, the worldY value could
      * be an Y in millimeters on a piece of paper.
      * 
      * @param projectionY A Y value in projection coordinates.
      * 
      * @return double
      */
      virtual double WorldY (const double projectionY) const = 0;

     /** 
      * This virtual method will the resolution of the world system relative to 
      * one unit in the projection system. For example, one meter in the 
      * projection system could be .001 inches on a piece of paper. Unless this 
      * method is overridden it will return 1
      * 
      * @return double
      */
      virtual double Resolution () const { return 1.0; };
  };
};

#endif
