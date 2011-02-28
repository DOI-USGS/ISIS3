#if !defined(Kernels_h)
#define Kernels_h
/**                                                                       
 * @file                                                                  
 * $Revision$
 * $Date$ 
 * $Id$ 
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
#include "CollectorMap.h"
#include "iException.h"

namespace Isis {

  /**
   * @brief Determine SPICE kernels defined in an ISIS file
   * 
   * This class determines all SPICE kernels associated with an ISIS cube file 
   * and optionally/selectively loads them using the NAIF toolkit.  This creates
   * the kernel pool as it was when spiceinit determined all the kernels and it 
   * initialized the file for geometric operations. 
   *  
   * Note that because NAIF kernel management is global in nature, careful 
   * thought and planning should preceed use of this class.  You can get 
   * yourself into trouble if you take over management of kernels and 
   * inadvertantly unload them without regard for use by other resources. 
   * Methods are provided in this implementation to help minimize problems 
   * related to management of this global resource. 
   * 
   * Note that ISIS caches some of the voluminous NAIF kernels, extracting only
   * what is required from the SPK and CK (generally) kernels for a given 
   * observation.  After this occurs, these kernels are no longer loaded by the
   * ISIS Spice class hierarchy.  This class provides that environment so that
   * further NAIF operations can occur, such as velocity vector and instrument
   * timing manipulations. 
   *  
   * This class can be used instead of instantiating an ISIS Spice object.  The 
   * recommended usage is as follows: 
   *  
   * @code 
   *   Cube cube;
   *   cube.Open("mycube.cub");
   *   Kernels myKernels(cube);
   *   myKernels.UpdateLoadStatus();  //  Ensures loaded kernels stay loaded
   *   myKernels.Load();
   *  
   *    . . .
   *  
   *   // After manipulating the CK Spice data...
   *   myKernels.UnLoad();
   * @endcode 
   *  
   * The implications of this code segment are: 1) All the kernels pertinent to 
   * an individual ISIS file are identified upon Kernels instantiation, 2) their 
   * load status is determined using the UpdateLoadStatus() method, 3) kernels 
   * already loaded are flagged as unmanaged, meaning they cannot be unloaded 
   * unless management is an explicit "hostile takeover" provided via the 
   * Manage() method (don't do it unless you absolutely have to), 4) after 
   * checking load status, and kernels you load are managed by your 
   * instantiation, and 5) when you UnLoad() or the object is destroyed, all 
   * managed kernels are unloaded - unmanaged kernels are not unloaded. 
   *  
   * This class can be used in lieu of the Spice class with liberal use of the 
   * UpdateLoadStatus() and Discover() methods of this class. 
   *  
   * It is possible to double load NAIF kernels, so be sure to maintain load 
   * status via UpdateLoadStatus() and proper usage techniques. 
   * 
   * @ingroup Utility
   * @author 2010-12-28 Kris Becker 
   *  
   * @internal 
   *   @history 2010-12-29 Kris Becker - added resolveTypeByExt() method to find
   *                        non-compliant NAIF file type identifiers kernels
   *   @history 2011-02-07 Kris Becker Corrected typo in Kernels/Extra (was
   *                                   Extras)
   *   @history 2011-02-28 Kris Becker - When using the examine() method, if the
   *                                     kernel is found to be loaded, its
   *                                     management state is set to unmanaged
   */
class Kernels {
    public:
      /** Default Constructor */
      Kernels();
      Kernels(const std::string &filename);
      Kernels(Cube &cube);
      Kernels(Pvl &pvl);
      /** Destructor always unloads the kernels from the pool */
      virtual ~Kernels() { UnLoad(); }

      /** Returns the number of kernels found and/or loaded */
      int size() const { return (_kernels.size());  }
      int Missing() const;

      void Init(Pvl &pvl);
      bool Add(const std::string &kfile);

      void Clear();
      int Discover();

      void Manage();
      void UnManage();
      bool IsManaged() const;

      void InitializeNaifKernelPool();

      int Load(const std::string &ktype);
      int Load();

      int UnLoad(const std::string &ktype);
      int UnLoad();

      int UpdateLoadStatus();

      int Merge(const Kernels &other);

      std::vector<std::string> getKernelTypes() const;
      std::vector<std::string> getKernelList(const std::string &ktype = "") 
                                             const;
      std::vector<std::string> getLoadedList(const std::string &ktypes = "") 
                                             const;
      std::vector<std::string> getMissingList() const;

      
      /** Returns the ISIS camera model version number */
      int CameraVersion() const { return (_camVersion); }

  private:
    struct KernelFile {          
      std::string  pathname;
      std::string  name;
      std::string  fullpath;
      bool         exists; 
      std::string  ktype; 
      mutable bool loaded; 
      bool         managed;
    };

    typedef std::vector<KernelFile> KernelList;
    KernelList _kernels;
    int        _camVersion;

    typedef std::vector<KernelFile *> KernelFileList;
    typedef CollectorMap<std::string, KernelFileList> TypeList;

    bool Load(KernelFile &kfile);
    bool UnLoad(KernelFile &kfile);

    std::vector<std::string> getTypes(const std::string &ktypes) const;
    std::string resolveType(const std::string &kfile) const;
    std::string resolveTypeByExt(const std::string &kfile, 
                                 const std::string &iktype = "UNKNOWN") const;

    bool IsNaifType(const std::string &ktype) const;
    KernelFile examine(const std::string &fname, const bool &manage = true) 
                       const;
    std::vector<KernelFile> findKernels(Pvl &pvl, const std::string &kname,
                                       const bool &manage = true);
    KernelFile *findByName(const std::string &kfile);
    TypeList categorizeByType() const;
     
    void addKernels(const KernelList &klist);
    std::string getKernelType(const std::string &kname) const; 
    void loadKernel(const std::string &ktype = "");
    int getCameraVersion(Pvl &pvl) const;

};

}     // namespace Isis
#endif


