#ifndef Kernels_h
#define Kernels_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */                                                                 

#include <iostream>
#include <string>
#include <vector>

#include "Camera.h"
#include "Cube.h"
#include "CollectorMap.h"
#include "IException.h"
#include "Pvl.h"

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
   *   @history 2011-03-27 Kris Becker - Added copy constructor and assignment
   *                                     operator
   *   @history 2015-03-05 Kris Becker - Added support for DSK and meta kernels
   *                                     types. References #2035
   *   @history 2015-07-21 Kristin Berry - Added NaifStatus::CheckErrors() to see if any NAIF errors
   *                                     were signaled. References #2248.
   */
class Kernels {
    public:
      /** Default Constructor */
      Kernels();
      Kernels(const Kernels &kernels);
      Kernels(const QString &filename);
      Kernels(Cube &cube);
      Kernels(Pvl &pvl);
      /** Destructor always unloads the kernels from the pool */
      virtual ~Kernels() { UnLoad(); }

      Kernels &operator=(const Kernels &kernels);

      /** Returns the number of kernels found and/or loaded */
      int size() const { return (_kernels.size());  }
      int Missing() const;

      void Init(Pvl &pvl);
      bool Add(const QString &kfile);

      void Clear();
      int Discover();

      void Manage();
      void UnManage();
      bool IsManaged() const;

      void InitializeNaifKernelPool();

      int Load(const QString &ktype);
      int Load();

      int UnLoad(const QString &ktype);
      int UnLoad();

      int UpdateLoadStatus();

      int Merge(const Kernels &other);

      QStringList getKernelTypes() const;
      QStringList getKernelList(const QString &ktype = "") 
                                             const;
      QStringList getLoadedList(const QString &ktypes = "") 
                                             const;
      QStringList getMissingList() const;

      
      /** Returns the ISIS camera model version number */
      int CameraVersion() const { return (_camVersion); }

  private:
    struct KernelFile {          
      QString  pathname;
      QString  name;
      QString  fullpath;
      bool         exists; 
      QString  ktype; 
      mutable bool loaded; 
      bool         managed;
    };

    typedef std::vector<KernelFile> KernelList;
    KernelList _kernels;
    int        _camVersion;

    typedef std::vector<KernelFile *> KernelFileList;
    typedef CollectorMap<QString, KernelFileList> TypeList;

    bool Load(KernelFile &kfile);
    bool UnLoad(KernelFile &kfile);

    QStringList getTypes(const QString &ktypes) const;
    QString resolveType(const QString &kfile) const;
    QString resolveTypeByExt(const QString &kfile, 
                                 const QString &iktype = "UNKNOWN") const;

    bool IsNaifType(const QString &ktype) const;
    KernelFile examine(const QString &fname, const bool &manage = true) 
                       const;
    int UpdateManagedStatus();
    std::vector<KernelFile> findKernels(Pvl &pvl, const QString &kname,
                                       const bool &manage = true);
    KernelFile *findByName(const QString &kfile);
    TypeList categorizeByType() const;
      
    void addKernels(const KernelList &klist);
    QString getKernelType(const QString &kname) const; 
    void loadKernel(const QString &ktype = "");
    int getCameraVersion(Pvl &pvl) const;

};

}     // namespace Isis
#endif


