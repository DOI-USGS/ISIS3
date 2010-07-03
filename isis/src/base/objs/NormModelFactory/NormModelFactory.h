#ifndef NormModelFactory_h
#define NormModelFactory_h

/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/06/18 17:35:09 $                                                                 
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
  class NormModel;

  /**
   * This class is used to create NormModel objects. Typically, applications which
   * perform normalization corrections need to use different methods such as
   * Shade, ShadeAtm, Albedo, etc. If this factory is given a Pvl object which
   * contains a NormModel definition, it will create that specific instance of the
   * class. For example,
   * 
   * @code
   * Object = NormalizationModel
   *   Group = Algorithm
   *     Name = Shade
   *     ...
   *   EndGroup
   *   ...
   * EndObject
   * End
   * @endcode
   * 
   * Will create a Shade normalization object (which is derived from 
   * NormModel). The simplest way to create a NormModel class is to use 
   * the static Create method.
   * 
   * @code
   * Pvl p("mynormmodel.pvl");
   * NormModel *ar = NormModelFactory::Create(p);
   * @endcode
   * 
   * @ingroup PatternMatching
   *
   * @author 2006-01-23 Janet Barrett 
   *  
   * @internal
   *   @history 2006-01-23 Janet Barrett - Original version
   *   @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
   */
  class NormModelFactory {
    public:
      static NormModel *Create(Pvl &pvl, PhotoModel &pmodel);
      static NormModel *Create(Pvl &pvl, PhotoModel &pmodel, AtmosModel &amodel);

    private:
      /** 
       * Constructor (its private so you can't use it).  Use the Create Method
       * instead.
       */
      NormModelFactory() {};

      //! Destroys the NormModelFactory
      ~NormModelFactory() {};
  };
};

#endif
