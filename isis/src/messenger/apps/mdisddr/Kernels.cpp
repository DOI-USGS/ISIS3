/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $
 * $Date: 2009/12/29 23:03:50 $
 * $Id: Kernels.cpp,v 1.2 2009/12/29 23:03:50 ehyer Exp $
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

#include "Kernels.h"
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
   */
  Kernels::Kernels(const std::string &filename) {
    Pvl pvl(filename);
    Load(pvl);
  } 

  /**
   * @brief Construct using an ISIS Cube object
   * 
   * @param cube    Cube object of ISIS file
   */
  Kernels::Kernels(Cube &cube) { 
    Load(*cube.Label());
  } 

  /**
   * @brief Construct from an ISIS label
   * 
   * @param pvl  ISIS label to get kernel information from
   */
  Kernels::Kernels(Pvl &pvl) { 
    Load(pvl);
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
   */
  void Kernels::Load(Pvl &pvl) {
    _kernels.clear();
    addKernels(findKernels("InstrumentPosition", KernelSet::Spk, pvl, "InstrumentPosition"));
    addKernels(findKernels("Frame", KernelSet::Fk, pvl));
    addKernels(findKernels("InstrumentPointing", KernelSet::Ck, pvl, "InstrumentPointing"));
    addKernels(findKernels("TargetPosition", KernelSet::Pck, pvl, "SunPosition"));
    addKernels(findKernels("TargetAttitudeShape", KernelSet::Pck, pvl, "BodyRotation"));
    addKernels(findKernels("Instrument", KernelSet::Ik, pvl));
    addKernels(findKernels("InstrumentAddendum", KernelSet::Spk, pvl));
    addKernels(findKernels("LeapSecond", KernelSet::Lsk, pvl));
    addKernels(findKernels("SpacecraftClock", KernelSet::Sclk, pvl));
    addKernels(findKernels("ShapeModel", KernelSet::Dem, pvl));
    addKernels(findKernels("Extras", KernelSet::Extras, pvl));
    return;
  }

  void Kernels::addKernels(const KernelSet &kset) {
    KernelSet kerns = kset;
    if (size() > 0) {
      kerns.setDependancy(_kernels[size()-1].type());
    }
    _kernels.push_back(kerns);
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
  Kernels::KernelFiles Kernels::getList(bool removePath) const {
      KernelFiles flist;
      for (unsigned int i = 0 ; i < _kernels.size() ; i++) {
        KernelFiles kfiles = _kernels[i].getNames();
        for (unsigned int k = 0 ; k < kfiles.size() ; k++) {
          if (removePath) {
              Filename kfile(kfiles[k]);
              flist.push_back(kfile.Name());
          }
          else {
              flist.push_back(kfiles[k]);
          }
        }
      }
      return (flist);
  }

  /**
   * @brief Unloads all kernels if they were loaded when found
   */
  void Kernels::Unload() {
      if (_furnish) {
         for (unsigned int i = 0 ; i < _kernels.size() ; i++) {
           KernelFiles kfiles = _kernels[i].getNames();
           for (unsigned int k = 0 ; k < kfiles.size() ; k++) {
             Filename f(kfiles[k]); 
             string kernName(f.Expanded());
             unload_c(kernName.c_str());
           }
         }
      }
      return;
  }

  KernelSet Kernels::findKernels(const std::string &kname, 
                                 KernelSet::KernelType ktype, Pvl &pvl,
                                 const std::string &blobname) const {
    // Get the kernel group and load main kernels
    PvlGroup &kernels = pvl.FindGroup("Kernels",Pvl::Traverse);
    KernelSet kkey(PvlKeyword(kname), ktype);

    // Check for the keyword
    if (kernels.HasKeyword(kname)) {
      kkey = KernelSet(kernels[kname], ktype);
      // Check for keyword design < 3.1.19 and update it to current state
      if (kkey.inTable() && (kkey.size() == 0)) {
        string bname(blobname);
        if (bname.empty()) bname = kname;
        const PvlObject &blob = findTable(bname, pvl);
        if (blob.HasKeyword("Kernels")) {
          PvlKeyword bkey = blob["Kernels"];
          PvlKeyword newkey = kernels[kname];
          // Found the Kernels keyword in BLOB.  Get the filenames
          for (int i = 0 ; i < bkey.Size() ; i++) {
            newkey.AddValue(bkey[i]);
          }
          kkey = KernelSet(newkey, ktype);
        }
        else {
          kkey.Missing("Image has been jigsawed and/or kernels are gone");
        }
      }
    }

    return (kkey);
  }

  const PvlObject &Kernels::findTable(const std::string &name, PvlObject &pvl) 
                                      const {
    PvlObject::ConstPvlObjectIterator tobj = pvl.FindObject("Table", 
                                                            pvl.BeginObject(),
                                                            pvl.EndObject());
    while (tobj != pvl.EndObject()) {
      if (iString::Equal(tobj->Name(), "Table")) {
        if (tobj->HasKeyword("Name")) {
          if (iString::Equal((*tobj)["Name"], name)) {
            return (*tobj);
          }
        }
      }
      ++tobj;
    }

    // If it reaches here, it is a fatal error
    string mess = "Failed to find Table object named " + name;
    throw iException::Message(iException::Programmer, mess, _FILEINFO_);
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
  void Kernels::loadKernel(PvlKeyword &key) {
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
  void Kernels::loadKernelFromTable(PvlKeyword &key, 
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
}  // namespace Isis


