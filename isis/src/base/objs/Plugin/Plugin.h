#ifndef Plugin_h
#define Plugin_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2007/07/19 22:49:55 $                                                                 
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

#include "Pvl.h"

namespace Isis {
/**                                                                       
 * @brief Loads plugins from a shared library
 * 
 * This class is used to handle dynamic loading of module/classes.  It  is 
 * rarely used directly but instead is inherited for a particular purpose such 
 * as managing class specific map projections or camera models.
 * The class is derived from a PVL which aides in the selection of the
 * shared library and plugin routine to load.  For example, assume the file
 * my.plugin contained:
 *  @code
 *    OBJECT=SINUSOIDAL
 *      LIBRARY=libisis3.so 
 *      ROUTINE=SinusoidalPlugin 
 *    END_OBJECT 
 *    OBJECT=SIMPLECYLINDRICAL 
 *      LIBRARY=libisis3.so 
 *      ROUTINE=SimpleCylindricalPlugin 
 *    END_OBJECT 
 *  @endcode 
 * The desired routine can be selected in code as follows:
 * @code
 * Plugin p;
 * p.Read("my.plugin");
 * string proj;
 * cin >> proj;  // Enter either SINUSOIDAL or SIMPLECYLINDRICAL
 * p.Find(proj);
 * void *ptr = p.GetPlugin();
 * @endcode
 * Obtaining plugins can be difficult to understand.  It is suggested you
 * look at ProjectionFactory to get a better understanding of how they are used.
 * 
 * @see ProjectionFactory
 * @see CameraFactory
 *                                                                        
 * @ingroup System                                                  
 *                                                                        
 * @author 2004-02-07 Jeff Anderson                                                                                  
 *                                                                        
 * @internal                                                                                                                             
 *   @history 2005-02-15 Jeff Anderson refactored to use Qt Qlibrary class. 
 *   @history 2007-07-19 Steven Lambright Fixed memory leak
 */                                                                       
  class Plugin : public Isis::Pvl {
    public:
      Plugin ();

      //! Destroys the Plugin object.
      virtual ~Plugin () {};

      void *GetPlugin( const std::string &group);
  };
};

#endif
