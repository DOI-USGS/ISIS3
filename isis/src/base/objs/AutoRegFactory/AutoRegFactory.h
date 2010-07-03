#ifndef AutoRegFactory_h
#define AutoRegFactory_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $                                                             
 * $Date: 2008/06/19 23:35:38 $                                                                 
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
  class AutoReg;

  /**
   * This class is used to create AutoReg objects.  Typically applications which
   * need use autoregistration would like to use different techniques such as
   * MaximumCorrelation or MinimumDifference.  If this factory is given a Pvl 
   * object which contains a AutoReg definition it will create that specific
   * instance of the class.  For example,
   * 
   * @code
   * Object = AutoReg
   *   Group = Algorithm
   *     Name = MinimumDifference
   *     ...
   *   EndGroup
   *   ...
   * EndObject
   * End
   * @endcode
   * 
   * Will create a MinimumDifference object (which is derived from AutoReg).
   * The simplest way to create an AutoReg class is to use the static Create
   * method
   * 
   * @code
   * Pvl p("myautoreg.pvl");
   * AutoReg *ar = AutoRegFactory::Create(p);
   * @endcode
   * 
   * @ingroup PatternMatching
   *
   * @author 2005-05-04 Jeff Anderson
   * 
   * @internal
   *   @history 2006-03-27 Jacob Danton Added unitTest
   *   @history 2008-06-18 Christopher Austin Fixed documentation errors
   *   @history 2008-06-19 Steven Lambright Fixed memory leak
   */
  class AutoRegFactory {
    public:
      static AutoReg *Create(Pvl &pvl);

    private:
      /** 
       * Constructor (its private so you can't use it).  Use the Create Method
       * instead.
       */
      AutoRegFactory() {};

      //! Destroys the AutoRegFactory
      ~AutoRegFactory() {};
  };
};

#endif
