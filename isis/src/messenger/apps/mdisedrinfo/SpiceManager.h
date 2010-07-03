#if !defined(SpiceManager_h)
#define SpiceManager_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $
 * $Date: 2007/08/23 07:15:44 $
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

#include <string>
#include <vector>
#include <iostream>

#include "Pvl.h"
#include "Cube.h"
#include "Camera.h"
#include "iException.h"

namespace Isis {


  /**
   * @brief Load/Unload SPICE kernels defined in an ISIS file
   * 
   * This class determines all SPICE kernels associated to an ISIS cube file and
   * optionally loads them using the NAIF toolkit.  This creates the kernel pool
   * as it was when spiceinit determined all the kernels and it initialized the
   * file for geometric operations.
   * 
   * Note that ISIS caches of some of the voluminous NAIF kernels, extracting
   * only what is required from the SPK and CK (generally) kernels for a given
   * observation.  After this occurs, these kernels are no longer loaded by the
   * ISIS Spice class hierarchy.  This class provides that environment so that
   * further NAIF operations can occur, such as velocity vectors.
   * 
   * @ingroup Utility
   * @author 2007-08-19 Kris Becker
   */
class SpiceManager {
    public:
      /** Default Constructor */
      SpiceManager() : _kernlist(), _furnish(true) { }
      SpiceManager(const std::string &filename, bool furnish = true);
      SpiceManager(Cube &cube, bool furnish = true);
      SpiceManager(Pvl &pvl, bool furnish = true);
      /** Destructor always unloads the kernels from the pool */
      virtual ~SpiceManager() { Unload(); }

      /** Returns the number of kernels found and/or loaded */
      int size() const { return (_kernlist.size()); }

      void Load(Pvl &pvl, bool furnish = true);
      void add(const std::string &kernel);
      std::vector<std::string> getList(bool removePath = false) const;
      void Unload();


  private:
    std::vector<std::string> _kernlist;  //!< The list of kernels
    bool _furnish;                       //!< Load the kernels found?

    void loadKernel(PvlKeyword &key);
    void loadKernelFromTable(PvlKeyword &key, const std::string &tblname, 
                             Pvl &pvl);
    void addKernelName(const std::string &kname);

};

}     // namespace Isis
#endif

