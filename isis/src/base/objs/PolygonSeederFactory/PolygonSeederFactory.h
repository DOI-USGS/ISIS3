#if !defined(PolygonSeederFactory_h)
#define PolygonSeederFactory_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/12/17 21:43:54 $                                                                 
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

namespace Isis {
  class Pvl;
  class PolygonSeeder;

  /**
   * This class is used to create PolygonSeeder objects. Applications which
   * use autoseeding of points in polygons can use different techniques such as
   * Grid or ????.  If this factory is given a Pvl 
   * object which contains a PolygonSeeder definition it will create that specific
   * instance of the class.  For example,
   * 
   * @code
   * Object = PolygonSeeder
   *   Group = Algorithm
   *     Name = Grid
   *     ...
   *   EndGroup
   *   ...
   * EndObject
   * End
   * @endcode
   * 
   * Will create a GridPolygonSeeder object (which is derived from PolygonSeeder).
   * The simplest way to create an PolygonSeeder class is to use the static Create
   * method
   * 
   * @code
   * Pvl p("myPolygonSeeder.pvl");
   * PolygonSeeder *ps = PolygonSeederFactory::Create(p);
   * @endcode
   * 
   * @ingroup PatternMatching
   *
   * @author 2006-01-20 Stuart Sides 
   *  
   * @internal 
   *   @history 2008-12-17 Christopher Austin - Fixed memory leak 
   */
  class PolygonSeederFactory {
    public:
      static PolygonSeeder *Create(Pvl &pvl);

    private:
      /** 
       * Constructor (its private so you can't use it).  Use the Create Method
       * instead.
       */
      PolygonSeederFactory() {};

      //! Destroys the PolygonSeederFactory
      ~PolygonSeederFactory() {};
  };
};

#endif
