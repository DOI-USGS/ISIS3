#if !defined(Kernels_h)
#define Kernels_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $
 * $Date: 2009/06/26 23:39:21 $
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
   * @brief Determine SPICE kernels defined in an ISIS file
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
class Kernels {
    public:
      /** Default Constructor */
      Kernels() { }
      Kernels(const std::string &filename);
      Kernels(Cube &cube);
      Kernels(Pvl &pvl);
      /** Destructor always unloads the kernels from the pool */
      virtual ~Kernels() { UnLoad(); }

      /** Returns the number of kernels found and/or loaded */
      int size() const { return (_kernels.size());  }
      int Missing() const;

      void Init(Pvl &pvl);

      int Load(const std::string &ktype);
      int Load();

      int UnLoad(const std::string &ktype);
      int UnLoad();

      std::vector<std::string> getList(bool removePath = false) const;
      std::vector<std::string> getList(const std::string &ktype, 
                                       bool removePath = false) const;
      
      int CameraVersion() const { return (_camVersion); }

  private:
    struct KernelFile {          
      std::string  pathname;
      std::string  name;
      std::string  fullpath;
      bool         exists; 
      std::string  ktype; 
      mutable bool loaded; 
    };

    typedef std::vector<KernelFile> KernelList;
    KernelList _kernels;
    int        _camVersion;

    bool Load(KernelFile &kfile);
    bool UnLoad(KernelFile &kfile);

    std::vector<std::string> getTypes(const std::string &ktypes) const;
    std::string resolveType(const std::string &kfile) const;
    bool IsNaifType(const std::string &ktype) const;
    KernelFile examine(const std::string &fname) const;
    std::vector<KernelFile> findKernels(Pvl &pvl, const std::string &kname); 
    void addKernels(const KernelList &klist);
    std::string getKernelType(const std::string &kname) const; 
    void loadKernel(const std::string &ktype = "");
    int getCameraVersion(Pvl &pvl) const;

};

}     // namespace Isis
#endif


