/**                                                                       
 * @file                                                                  
 * $Revision: 1.9 $
 * $Date: 2009/12/29 23:03:50 $
 * $Id: SpiceManager.cpp,v 1.9 2009/12/29 23:03:50 ehyer Exp $
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
#include <iomanip>
#include <sstream>

#include "SpiceManager.h"
#include "Filename.h"
#include "PvlKeyword.h"
#include "Pvl.h"
#include "iException.h"
#include "naif/SpiceUsr.h"

using namespace std;

namespace Isis {

  /**
   * @brief Construct using an ISIS file name
   * 
   * @param filename Name of ISIS cube file
   * @param furnish Do we load the kernels we find?
   */
  SpiceManager::SpiceManager(const std::string &filename, bool furnish) {
    Pvl pvl(filename);
    Load(pvl, furnish);
  } 

  /**
   * @brief Construct using an ISIS Cube object
   * 
   * @param cube    Cube object of ISIS file
   * @param furnish Do we load the kernels we find?
   */
  SpiceManager::SpiceManager(Cube &cube, bool furnish) { 
    Load(*cube.Label(), furnish);
  } 

  /**
   * @brief Construct from an ISIS label
   * 
   * @param pvl  ISIS label to get kernel information from
   * @param furnish  Do we load the kernels we find?
   */
  SpiceManager::SpiceManager(Pvl &pvl, bool furnish) { 
    Load(pvl, furnish);
  } 


  /**
   * @brief Perform the hunt for Spice kernels in an ISIS label
   * 
   * This method must traverse the ISIS label hierarchy to find all the
   * pertinent kernel files.  Many of them are found in the Kernel Group, but
   * others exist in Table objects that are SPICE blobs.  The actual names are
   * found in the Kernels keyword in the named Table object.
   * 
   * @param pvl  ISIS label
   * @param furnish Do we actually load the kernel files as we find them?
   */
  void SpiceManager::Load(Pvl &pvl, bool furnish) {
    Unload();
    _furnish = furnish;

    std::string kernlist;
    // Get the kernel group and load main kernels
    PvlGroup kernels = pvl.FindGroup("Kernels",Pvl::Traverse);
    //  Changed 2008-02-27 to load planetary ephemeris before spacecraft 
    //  since MESSENGER team may update planet data in the s/c SPK.
    loadKernelFromTable(kernels["TargetPosition"], "SunPosition", pvl);

    //  Now do s/c ephemeris
    if (kernels.HasKeyword("SpacecraftPosition")) {
      loadKernel(kernels["SpacecraftPosition"]);
    }
    else {
      loadKernelFromTable(kernels["InstrumentPosition"], "InstrumentPosition", 
                          pvl);
    }

    if (kernels.HasKeyword("SpacecraftPointing")) {
      loadKernel(kernels["SpacecraftPointing"]);
    }
    else {
      loadKernelFromTable(kernels["InstrumentPointing"], "InstrumentPointing", 
                           pvl);
    }

    if (kernels.HasKeyword("Frame")) {
      loadKernel(kernels["Frame"]);
    }

    if (kernels.HasKeyword("Extra")) {
      loadKernel(kernels["Extra"]);
    }


    loadKernel(kernels["TargetAttitudeShape"]);
    loadKernel(kernels["Instrument"]);
    loadKernel(kernels["InstrumentAddendum"]);  // Always load after instrument
    loadKernel(kernels["LeapSecond"]);
    loadKernel(kernels["SpacecraftClock"]);
    return;
  }

  /**
   * @brief Add a specified kernel file to the pool
   * 
   * This method adds a specified kernel file to the NAIF pool.  The caller can
   * provide a file pattern with "?" in the filename as this method will
   * determine the highest occuring version.
   * 
   * @param kernel  Name of kernel file (or pattern) to add
   */
  void SpiceManager::add(const std::string &kernfile) {

    string kfile(kernfile);

    //  Check for versionized file naming 
    string::size_type start = kfile.find_first_of("?");
    if (start != string::npos) {
      Filename efile(kfile);
      efile.HighestVersion();
      kfile = efile.Expanded();
    }

    //  Add a specific kernel to the list
    PvlKeyword kernel("Kernels", kfile);
    loadKernel(kernel);
    return;
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
  std::vector<std::string> SpiceManager::getList(bool removePath) const {
      std::vector<std::string> flist;
      for (unsigned int i = 0 ; i < _kernlist.size() ; i++) {
          if (removePath) {
              Filename kfile(_kernlist[i]);
              flist.push_back(kfile.Name());
          }
          else {
              flist.push_back(_kernlist[i]);
          }
      }
      return (flist);
  }

  /**
   * @brief Unloads all kernels if they were loaded when found
   */
  void SpiceManager::Unload() {
      if (_furnish) {
          for (unsigned int i = 0 ; i < _kernlist.size() ; i++) {
            /**
            *  Changed to work with hipeaks crappy compiler.
            *  string kernName(Filename(_kernlist[i]).Expanded());
            *  unload_c(kernName.c_str());
            */
              Filename f(_kernlist[i]); 
              string kernName(f.Expanded());
              unload_c(kernName.c_str());
          }
      }
      _kernlist.clear();
      return;
  }

  /**
   * @brief Loops through PvlKeyword containing Kernel file names
   * 
   * This method interogates a PvlKeyword that is determined to
   * contain SPICE kernel names.  It will optionally load the
   * kernels if initially requested and then adds the name to the
   * internally managed list.
   * 
   * Some keywords may contain special keywords.  These are ignored in this
   * method and can be handled explicitly in other methods.
   * 
   * @param key PvlKeyword containing SPICE kernels
   * @see  loadKernelFromTable()
   */
  void SpiceManager::loadKernel(PvlKeyword &key) {
    for (int i=0; i<key.Size(); i++) {
      if (key[i] == "") continue;
      if (iString(key[i]).UpCase() == "NULL") continue;
      if (iString(key[i]).UpCase() == "NADIR") continue;
      if (iString(key[i]).UpCase() == "TABLE") continue;
      Isis::Filename file(key[i]);
      if (!file.exists()) {
        string msg = "Spice file does not exist [" + file.Expanded() + "]";
        throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
      }
      string fileName(file.Expanded());
      if (_furnish) furnsh_c(fileName.c_str());
      addKernelName((string)key[i]);
    }
  }

  /**
   * @brief Interogate a PvlKeyword for location of kernel file names
   * 
   * This method is intended to find keywords that refer to SPICE Table Blobs
   * and look in those Table objects for the actual names of SPICE kernel files.
   * They are then loaded via the loadKernel() method.
   * 
   * @param key PvlKeyword containing SPICE kernel names
   * @param tblname Name of Table where the SPICE blob is located in the label
   * @param pvl Pvl label to search for the SPICE Table Object Blob
   */
  void SpiceManager::loadKernelFromTable(PvlKeyword &key, 
                                         const std::string &tblname, Pvl &pvl) {
    if (iString::UpCase(key[0]) != "TABLE") {
      loadKernel(key);
    }
    else {
      PvlObject::PvlObjectIterator objIter;
      for (objIter = pvl.BeginObject() ; objIter != pvl.EndObject() ; ++objIter) {
        if (iString::UpCase(objIter->Name()) == "TABLE") {
          if (objIter->HasKeyword("Name")) {
            if (iString::Equal(objIter->FindKeyword("Name")[0], tblname)) {
              loadKernel(objIter->FindKeyword("Kernels"));
              return;
            }
          }
        }
      }
    }
    return;
  }
  /**
   * @brief Adds the named kernel file to the internal list
   * 
   * This method will add the name of the kernel file to the list.  It will
   * first determine if it already exists.  If it does, it will not be added a
   * second time.
   * 
   * @param kname Name of SPICE kernel file to add
   */
  void SpiceManager::addKernelName(const std::string &kname) {
    for (unsigned int i = 0 ; i < _kernlist.size() ; i++) {
      if (_kernlist[i] == kname)  return;
    }
    _kernlist.push_back(kname);
    return;
  }

}  // namespace Isis


