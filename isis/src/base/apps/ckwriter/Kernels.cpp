/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $
 * $Date: 2009/06/26 23:39:21 $
 * $Id: Kernels.cpp,v 1.1 2009/06/26 23:39:21 kbecker Exp $
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
#include <numeric>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "Kernels.h"
#include "Filename.h"
#include "PvlKeyword.h"
#include "Pvl.h"
#include "iException.h"
#include "naif/SpiceUsr.h"
#include "NaifStatus.h"

using namespace std;

namespace Isis {

  /**
   * @brief Construct using an ISIS file name
   * 
   * @param filename Name of ISIS cube file
   */
  Kernels::Kernels(const std::string &filename) {
    Pvl pvl(filename);
    Init(pvl);
  } 

  /**
   * @brief Construct using an ISIS Cube object
   * 
   * @param cube    Cube object of ISIS file
   */
  Kernels::Kernels(Cube &cube) { 
    Init(*cube.Label());
  } 

  /**
   * @brief Construct from an ISIS label
   * 
   * @param pvl  ISIS label to get kernel information from
   */
  Kernels::Kernels(Pvl &pvl) { 
    Init(pvl);
  } 


  /**
   * @brief Determine Spice kernels in an ISIS label
   * 
   * This method reads keywords in the Kernel group in the ISIS label hierarchy
   * to find all the pertinent kernel files.  They are found in the Kernel 
   * Group.
   * 
   * @param pvl  ISIS label
   */
  void Kernels::Init(Pvl &pvl) {
    UnLoad();
    _kernels.clear();
    addKernels(findKernels(pvl, "InstrumentPosition"));
    addKernels(findKernels(pvl, "Frame"));
    addKernels(findKernels(pvl, "InstrumentPointing"));
    addKernels(findKernels(pvl, "TargetPosition"));
    addKernels(findKernels(pvl, "TargetAttitudeShape"));
    addKernels(findKernels(pvl, "Instrument"));
    addKernels(findKernels(pvl, "InstrumentAddendum"));
    addKernels(findKernels(pvl, "LeapSecond"));
    addKernels(findKernels(pvl, "SpacecraftClock"));
    addKernels(findKernels(pvl, "ShapeModel"));
    addKernels(findKernels(pvl, "Extras"));
    _camVersion = getCameraVersion(pvl);
    return;
  }

 
  int Kernels::Load(const std::string &ktypes) {
    //  If not types specified, return them all
    int nLoaded(0);
    if (ktypes.empty()) { 
      for (unsigned int k = 0 ; k < _kernels.size() ; k++) {
        if (Load(_kernels[k])) { nLoaded++; }
      }
    }
    else {
      // Find types and return requested types
      vector<string> tlist = getTypes(ktypes);
      for (unsigned int t = 0 ; t < tlist.size() ; t++) {
        for (unsigned int k = 0; k < _kernels.size() ; k++) {
          if (_kernels[k].ktype == tlist[t]) {
            if (Load(_kernels[k])) { nLoaded++; }
          }
        }
      }
    }
    return (nLoaded);
  }

  int Kernels::Load() {
    //  If not types specified, return them all
    int nLoaded(0);
    for (unsigned int k = 0 ; k < _kernels.size() ; k++) {
      if (Load(_kernels[k])) { nLoaded++; }
    }
    return (nLoaded);
  }



  bool Kernels::Load(Kernels::KernelFile &kfile) {
    if (IsNaifType(kfile.ktype)) {
      if (!kfile.loaded) {
        NaifStatus::CheckErrors();
        try {
          furnsh_c(kfile.fullpath.c_str());
          NaifStatus::CheckErrors();
          kfile.loaded = true;
        } catch (iException &ie) {
          ie.Clear();
          return (false);
        }
      }
    }
    return (kfile.loaded);
  }

  /**
   * @brief Unloads all kernels if they were loaded when found
   */
  int Kernels::UnLoad() {
    int nUnloaded(0);
    for (unsigned int i = 0 ; i < _kernels.size() ; i++) {
      if (UnLoad(_kernels[i])) nUnloaded++;
    }
    return (nUnloaded);
  }

  /**
   * @brief Unloads all kernels if they were loaded when found
   */
  int Kernels::UnLoad(const std::string &ktypes) {
    //  If not types specified, return them all
    int nUnloaded(0);
    if (ktypes.empty()) { 
      nUnloaded = UnLoad();     
    }
    else {
      // Find types and return requested types
      vector<string> tlist = getTypes(ktypes);
      for (unsigned int t = 0 ; t < tlist.size() ; t++) {
        for (unsigned int k = 0; k < _kernels.size() ; k++) {
          if (_kernels[k].ktype == tlist[t]) {
            if (UnLoad(_kernels[k])) nUnloaded++;
          }
        }
      }
    }
    return (nUnloaded);
  }

  /**
   * @brief Unloads all kernels if they were loaded when found
   */
  bool Kernels::UnLoad(KernelFile &kfile) {
    //  If its loaded assume its a loaded NAIF kernel and unload it
    bool wasLoaded(false);
    if (kfile.loaded) {
     NaifStatus::CheckErrors();
     try {
       unload_c(kfile.fullpath.c_str());
       NaifStatus::CheckErrors();
     } catch (iException &ie) {
       // Errors are trapped and ignored.  It may be unloaded by other source
       ie.Clear();
     }
     kfile.loaded = false;
     wasLoaded = true;
    }
    return (wasLoaded);
  }

 /**
   * @brief Provide a list of all the kernels found
   * 
   * This method will return all the kernel file references as found in the ISIS
   * label.  It will optionally remove the file paths and return only the kernel
   * file name if requested.  If removePath is false, it returns the complete
   * path to the SPICE kernels.
   * 
   * @param removePath Do we remove the file paths and return only the file
   *                   names?
   * 
   * @return std::vector<std::string> A vector of filenames of SPICE kernels
   */
  std::vector<std::string> Kernels::getList(bool removePath) const {
    vector<string> flist;
    for (unsigned int i = 0 ; i < _kernels.size() ; i++) {
      if (removePath) {
        flist.push_back(_kernels[i].name);
      }
      else {
        flist.push_back(_kernels[i].pathname);
      }
    }
    return (flist);
  }

  /**
   * @brief Provide a list of all the kernels found
   * 
   * This method will return all the kernel file references as found in the ISIS
   * label.  It will optionally remove the file paths and return only the kernel
   * file name if requested.  If removePath is false, it returns the complete
   * path to the SPICE kernels.
   * 
   * @param removePath Do we remove the file paths and return only the file
   *                   names?
   * 
   * @return std::vector<std::string> A vector of filenames of SPICE kernels
   */
  std::vector<std::string> Kernels::getList(const std::string &ktypes, 
                                            bool removePath) const { 

    //  If not types specified, return them all
    if (ktypes.empty()) { return (getList(removePath));     }

    // Find types and return requested types
    vector<string> tlist;
    iString::Split(',', ktypes, tlist);
    vector<string> flist;
    for (unsigned int i = 0 ; i < tlist.size() ; i++) {
      string ktype = iString(tlist[i]).Trim(" \n\r\t\b\v\f").UpCase();
      for (unsigned int k = 0; k < _kernels.size() ; k++) {
        if (_kernels[k].ktype == ktype) {
          if (removePath) {
            flist.push_back(_kernels[i].name);
          }
          else {
            flist.push_back(_kernels[i].pathname);
          }
        }
      }
    }
    return (flist);
  }

  std::vector<std::string> Kernels::getTypes(const std::string &ktypes) const {
   // Find types and return requested types
    vector<string> tlist;
    iString::Split(',', ktypes, tlist);
    for (unsigned int k = 0 ; k < tlist.size() ; k++) {
      tlist[k] = iString(tlist[k]).Trim(" \r\n\t\v\b\f").UpCase();
    }
    return (tlist);
  }

  /**
   * @brief Add a list of kernel files to internal storage 
   * 
   * @author Kris Becker - 12/6/2010
   * 
   * @param klist 
   */
  void Kernels::addKernels(const KernelList &klist) {
    copy(klist.begin(), klist.end(), back_inserter(_kernels));
    return; 
  }


  Kernels::KernelList Kernels::findKernels(Pvl &pvl, 
                                           const std::string &kname) {
    KernelList klist;
    // Get the kernel group and load main kernels
    PvlGroup &kernels = pvl.FindGroup("Kernels",Pvl::Traverse);
    // Check for the keyword
    if (kernels.HasKeyword(kname)) {
      PvlKeyword &kkey = kernels[kname];
      for (int i = 0 ; i < kkey.Size() ; i++) {
        if (!iString::Equal(kkey[i], "Table")) {
          klist.push_back(examine(kkey[i]));
        }
      }
    }

    return (klist);
  }


  /**
   * @brief Determine if the type is a NAIF supported kernel type
   * 
   * 
   * @param ktype String containing the kernel type
   * 
   * @return bool True if it is a NAIF kernel type
   */
  bool Kernels::IsNaifType(const std::string &ktype) const {
    if (iString::Equal(ktype, "UNKNOWN")) return (false);
    if (iString::Equal(ktype, "DEM")) return (false);
    return (true);
  }
  /**
   * @brief Determine type of NAIF kernel file 
   *  
   *  
   * This method will determine the type of kernel contained in the file 
   * specified by the kfile parameter. 
   *  
   * The file specified by the kfile parameter is assumed to conform to NAIF 
   * kernel file conventions (i.e., binary kernels are created using the NAIF 
   * toolkit, text kernels conform to NAIF standards).  There are, however, two 
   * exceptions that must be considered.  ISIS DEMs are cubes and do not follow 
   * the NAIF convention for obvious reasons.  ISIS IAK kernels also do not 
   * typically follow NAIF identification standards.  These two cases are 
   * handled special. 
   *  
   * To determine a NAIF stardard conforming file type, the first eight 
   * characters of the file given will be inspected to determine the NAIF kernel 
   * type.  If this fails to produce a known type, then it is assumed to be an 
   * ISIS DEM or IAK kernel. 
   *  
   * For valid NAIF kernels, the NAIF routine kinfo_c is used to acquire 
   * additional information such as if it is loaded.
   *  
   * For files where the type cannot be determined, the type is set to 
   * "UNKNOWN".  ISIS DEMs are set to the type "DEM".  ISIS IAKs are set to 
   * "IAK".  Other types are set as follows: 
   *  
   * CK 
   * SPK 
   * PCK 
   * EK 
   * META 
   * IK 
   * FK 
   * SCLK 
   *  
   * 
   * @param kfile Name of kernel file to inspect
   * 
   * @return Kernels::KernelFile An internal Kernels file structure describing 
   *         the file.
   */
  Kernels::KernelFile Kernels::examine(const std::string &kfile) const {

    Filename kernfile(kfile);
    KernelFile kf;
    kf.pathname = kfile;
    kf.name = kernfile.Name();
    kf.fullpath = kernfile.Expanded();
    kf.exists = kernfile.Exists();
    kf.ktype = "UNKNOWN";
    kf.loaded = false;     // Assumes its not loaded

    // Determine type and load info
    if (kf.exists) {
      kf.ktype = resolveType(kf.fullpath);

      // Use NAIF to determine if it is loaded
      if (IsNaifType(kf.ktype)) {
        SpiceChar ktype[32];
        SpiceChar source[128];
        SpiceInt  handle;
        SpiceBoolean found;
        kinfo_c(kf.fullpath.c_str(), sizeof(ktype), sizeof(source), ktype,
                source, &handle, &found);
        if (found == SPICETRUE) {
          kf.loaded = true;
          kf.ktype = iString(ktype).UpCase();
        }
      }
    }

    return (kf);
  }


  std::string Kernels::resolveType(const std::string &kfile) const {
    Filename kernFile(kfile);
    string kpath = kernFile.Expanded();
    ifstream ifile(kpath.c_str(), ios::in | ios::binary);
    string ktype("UNKNOWN");
    if (ifile) {
      char ibuf[10];
      ifile.read(ibuf, 8);
      ibuf[8] = '\0';

      // See if the file is a known NAIF type.  Assume it has been
      // extracted from a NAIF compliant kernel
      string istr = iString(ibuf).Trim(" \n\r\f\t\v\b");
      if (!istr.empty()) {
        vector<string> parts;
        iString::Split('/', istr, parts);
        if (parts.size() > 1) {
          ktype = parts[parts.size()-1];
        }
      }

      // Check for ISIS specific types
      if (ktype == "UNKNOWN") {
        string ext = Filename(kfile).Extension();
        if (ext == "cub") {
          ktype = "DEM";
        }
        else if (ext == "ti") {
          //  Assume its an ISIS IAK with a .ti extension
          ktype = "IAK";
        }
      }
    }
    return (ktype);
  }

  int Kernels::getCameraVersion(Pvl &pvl) const {
    PvlGroup &kernels = pvl.FindGroup("Kernels",Pvl::Traverse);
    int cv(0);
    // Check for the keyword
    if (kernels.HasKeyword("CameraVersion")) {
      PvlKeyword &kkey = kernels["CameraVersion"];
      cv = iString(kkey[0]).ToInteger();
    }
    return (cv);
  }

}  // namespace Isis


