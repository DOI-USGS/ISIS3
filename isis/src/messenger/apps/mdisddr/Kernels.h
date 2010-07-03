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

class KernelSet {
  public:
    typedef enum { None, Lsk, Pck, Ik, Fk, Sclk, Ck, Spk, Iak, Dem, Extras } KernelType; 
    typedef std::vector<std::string> KernelFiles;
    KernelSet() : _kernkey(PvlKeyword("Kernels")), _ktype(None), 
                  _dependancy(None),_missing("") {}
    KernelSet(const PvlKeyword &key, KernelType &ktype, 
              const KernelType &depends = None) : _kernkey(key), _ktype(ktype),
                                                 _dependancy(depends),
                                                 _missing("") { }
    ~KernelSet() {}

    /**
     * @brief Determines if the kernel keyword is valid
     * @return bool True if the keyword is non-NULL
     */
    bool isValid() const { return (!_kernkey.IsNull()); }
    /**
     * @brief Get number of kernels files in list excluding special Tabel/Nadir
     * @return int  Number of real kernel files
     */
    int size() const { 
      if (!isValid()) return (0);
      return (_kernkey.Size() - ((inTable()) ? 1 : 0));
    }

    KernelType type() const { return (_ktype); }
    bool inTable() const {
      if ((_kernkey.Size() > 0) && (isTableBound(_kernkey[0]))) return (true);
      return (false);
    }

    KernelType dependant() const { return (_dependancy); }
    void setDependancy(const KernelType &depends) { _dependancy = depends; }

    KernelFiles getNames() const { 
      KernelFiles k;
      if (isValid()) {
        for (int i = 0 ; i < _kernkey.Size() ; i++) {
          if (!isTableBound(_kernkey[i])) k.push_back(_kernkey[i]);
        }
      }
      return (k);
    }

    void Missing(const std::string &mess = "Kernel names not found") {
      _missing = mess;
    }

    bool isMissing() const { return (_missing.size() > 0); }
    const std::string getMissing() const { return (_missing); }

  private:
    PvlKeyword _kernkey;
    KernelType  _ktype;
    KernelType  _dependancy;
    std::string _missing;

    bool isTableBound(const std::string &kfile) const {
      if (iString::Equal(kfile, "Table")) return (true);      
      if (iString::Equal(kfile, "Nadir")) return (true);      
      return (false);
    }

};


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
      typedef KernelSet::KernelFiles KernelFiles;
      /** Default Constructor */
      Kernels() : _kernels(), _furnish(false) { }
      Kernels(const std::string &filename);
      Kernels(Cube &cube);
      Kernels(Pvl &pvl);
      /** Destructor always unloads the kernels from the pool */
      virtual ~Kernels() { Unload(); }

      /**
       * @brief Returns the number of kernel keywords found
       * @return int Number keywords
       */
      int nKeys() const { return (_kernels.size()); }

      /** Returns the number of kernels found and/or loaded */
      int size() const { 
        int nk(0);
        ConstKernelListIter k;
        for (k = _kernels.begin() ; k != _kernels.end() ; ++k) {
          nk += k->size();
        }
        return (nk);
      }

      void Load(Pvl &pvl);
      KernelFiles getList(bool removePath = false) const;
      void Unload();

  private:
    typedef std::vector<KernelSet> KernelList;
    typedef KernelList::iterator KernelListIter;
    typedef KernelList::const_iterator ConstKernelListIter;
    typedef KernelSet::KernelType KernelType;
    KernelList _kernels;
    bool _furnish;          //!< Load the kernels found?

    KernelSet findKernels(const std::string &kname, 
                          KernelSet::KernelType ktype, Pvl &Pvl,
                          const std::string &blobname = "") const; 
    const PvlObject &findTable(const std::string &name, PvlObject &pvl) const;
    void addKernels(const KernelSet &kset);
    void loadKernel(PvlKeyword &key);
    void loadKernelFromTable(PvlKeyword &key, const std::string &tblname, 
                             Pvl &pvl);

};

}     // namespace Isis
#endif


