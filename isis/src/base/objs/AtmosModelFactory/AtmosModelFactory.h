#ifndef AtmosModelFactory_h
#define AtmosModelFactory_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/06/18 18:53:56 $                                                                 
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
  class AtmosModel;


  /**
   *  This class is used to create AtmosModel objects. Typically, applications
   *which perform atmospheric corrections need to use different types of 
   *atmospheric function such as Isotropic1, Anisotropic1, HapkeAtm1, etc. If this
   *factory is given a Pvl object which contains an AtmosModel definition, it will
   *create that specific instance of the class. For example, 
   * 
   * @code
   *  Object = AtmosphericModel Group = Algorithm
   *    Name = Isotropic1 ...
   *  EndGroup ...
   *EndObject End 
   * @endcode
   * 
   * Will create an Isotropic 1st order object (which is derived from AtmosModel).
   * The simplest way to create an AtmosModel class is to use the static Create 
   * method
   * 
   * @code
   * Pvl p("myatmosmodel.pvl");
   * AtmosModel *ar = AtmosModelFactory::Create(p);
   * @endcode
   * 
   * @ingroup PatternMatching
   *
   * @author 2006-01-23 Janet Barrett
   * 
   * @internal
   *  @history 2008-06-18 Christopher Austin - Fixed documentation errors
   */
  class AtmosModelFactory {
    public:
      static AtmosModel *Create(Pvl &pvl, PhotoModel &pmodel);

    private:
      /** 
       * Constructor (its private so you can't use it).  Use the Create Method
       * instead.
       */
      AtmosModelFactory() {};

      //! Destroys the AtmosModelFactory
      ~AtmosModelFactory() {};
  };
};

#endif
