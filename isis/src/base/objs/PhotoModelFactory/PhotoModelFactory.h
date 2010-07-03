#ifndef PhotoModelFactory_h
#define PhotoModelFactory_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/06/18 19:31:34 $                                                                 
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
  class PhotoModel;

  /**
   * This class is used to create PhotoModel objects. Typically, applications which
   * perform photometric corrections need to use different types of photometric
   * function such as Lambert, Minnaert, HapkeLegendre, etc. If this factory is
   * given a Pvl object which contains a PhotoModel definition, it will create that
   * specific instance of the class. For example,
   * 
   * @code
   * Object = PhotometricModel
   *   Group = Algorithm
   *     Name = Minnaert
   *     ...
   *   EndGroup
   *   ...
   * EndObject
   * End
   * @endcode
   * 
   * Will create a Minnaert object (which is derived from PhotoModel). The 
   * simplest way to create a PhotoModel class is to use the static Create 
   * method.
   * 
   * @code
   * Pvl p("myphotmodel.pvl");
   * PhotoModel *ar = PhotoModelFactory::Create(p);
   * @endcode
   * 
   * @ingroup PatternMatching
   *
   * @author 2006-01-23 Janet Barrett 
   *  
   * @internal
   *   @history 2006-01-23 Janet Barrett - Original version
   *   @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
   *    
   */
  class PhotoModelFactory {
    public:
      static PhotoModel *Create(Pvl &pvl);

    private:
      /** 
       * Constructor (its private so you can't use it).  Use the Create Method
       * instead.
       */
      PhotoModelFactory() {};

      //! Destroys the PhotoModelFactory
      ~PhotoModelFactory() {};
  };
};

#endif
