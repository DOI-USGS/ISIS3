/**                                                                       
 * @file                                                                  
 * $Revision: 1.5 $                                                             
 * $Date: 2009/05/12 20:11:20 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.                                                           
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website,              
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */
 
#include <iomanip>
#include "iException.h"
#include "CameraFactory.h"
#include "Preference.h"
#include "Filename.h"
#include "iString.h"
#include "KernelDb.h"

using namespace std; 
using namespace Isis;

// Create a new empty KernelDb
KernelDb::KernelDb(const unsigned int kernelTypes) {
  p_filename = "None";
  p_kernelTypes = kernelTypes;
}


// Create a new KernelDb object using a filename
KernelDb::KernelDb(const std::string &dbName, const unsigned int kernelTypes) :
            p_kernelData (dbName) {
  p_filename = dbName;
  p_kernelTypes = kernelTypes;
}


// Create a new KernelDb object using a stream
KernelDb::KernelDb(std::istream &dbStream, const unsigned int kernelTypes) {
  dbStream >> p_kernelData;
  p_filename = "internal stream";
  p_kernelTypes = kernelTypes;
}


// Return the highest version of all LeapSecond kernels identified by the DB
Kernel KernelDb::LeapSecond (Pvl &lab) {
  return FindLast("LeapSecond", lab);
}


// Return the highest version of all TargetAttitudeShape kernels identified
// by the DB
Kernel KernelDb::TargetAttitudeShape (Pvl &lab) {
  return FindLast("TargetAttitudeShape", lab);
}


// Return the highest version of all TargetPosition kernels identified
// by the DB
Kernel KernelDb::TargetPosition (Pvl &lab) {
  return FindLast("TargetPosition", lab);
}


// Return the last matching time of all SpacecraftPointing kernels identified
// by the DB
std::priority_queue< Kernel > KernelDb::SpacecraftPointing (Pvl &lab) {
  return FindAll("SpacecraftPointing", lab);
}


// Return the highest version of all SpaceCraftClock kernels identified
// by the DB
Kernel KernelDb::SpacecraftClock (Pvl &lab) {
  return FindLast("SpacecraftClock", lab);
}


// Return the last matching time of all SpacecraftPosition kernels identified
// by the DB
Kernel KernelDb::SpacecraftPosition (Pvl &lab) {
  return FindLast("SpacecraftPosition", lab);
}


// Return the last Instrument kernal found which matches the criteria in the DB
Kernel KernelDb::Instrument (Pvl &lab) {
  return FindLast("Instrument", lab);
}


// Return the highest version of all Frame kernels identified
// by the DB
Kernel KernelDb::Frame (Pvl &lab) {
  return FindLast("Frame", lab);
}


// Return the last InstrumentAddendum kernal found which matches the
// criteria in the DB
Kernel KernelDb::InstrumentAddendum (Pvl &lab) {
  return FindLast("InstrumentAddendum", lab);
}


// Return the highest version DEM found which matches the criteria in the DB
Kernel KernelDb::Dem (Pvl &lab) {
  return FindLast("Dem", lab);
}

Kernel KernelDb::FindLast(const std::string &entry, Isis::Pvl &lab) {
  std::priority_queue< Kernel > all = FindAll(entry, lab);
  Kernel last;

  if(all.size() > 0) {
    last = all.top();
  }

  return last;
}

std::priority_queue< Kernel > KernelDb::FindAll(const std::string &entry, Isis::Pvl &lab) {
  std::priority_queue< Kernel > filesFound;
  int cameraVersion = CameraFactory::CameraVersion(lab);
  Isis::PvlObject &cube = lab.FindObject("IsisCube");

  // Make sure the entry has been loaded into memory
  if (!p_kernelData.HasObject(entry)) {
    return filesFound;
  }

  // Get the start and end time for the cube
  iTime start(((string) cube.FindGroup("Instrument")["StartTime"]));
  iTime end;
  if (cube.FindGroup("Instrument").HasKeyword("StopTime")) {
    end = ((string) cube.FindGroup("Instrument")["StopTime"]);
  }
  else {
    end = ((string) cube.FindGroup("Instrument")["StartTime"]);
  }

  // lastType will be used to check if the new group's type is better/worse than the last match
  string lastType;
  lastType.clear();

  // Get the object and search through it's groups
  PvlObject &obj = p_kernelData.FindObject(entry);
  for (int group = obj.Groups()-1; group >= 0; group--) {
    // Get the group and start testing the cases in the keywords to see if they all match this cube
    PvlGroup &grp = obj.Group(group);

    // These are the conditions that make this test pass:
    //   1) No time OR At least one matching time
    //   2) All keyword matches are true OR No keyword matches present
    // 
    // In order to accomplish this, matchTime is initialized to true and remains as such if and only if
    // there are no time conditionals. If Time keywords exist, one of them has to set matchTime to true.
    // The matchKeywords is true until a mismatch is found.
    bool matchTime = !grp.HasKeyword("Time");
    bool matchKeywords = true;

    // If the group name isn't selection, skip it.
    if (!grp.IsNamed("Selection")) {
      continue;
    }

    iString type = "";

    // Make sure the type is better
    if(grp.HasKeyword("Type")) {
      type = (string) grp["Type"];
      if (!(spiceInit::kernelTypeEnum(type) & p_kernelTypes)) {
        continue;
      }
    }

    // First, the time search. Loop through the keywords, if the name isn't Time then
    // skip it. If it is, then get the start/end times and keep looking until one is found.
    for (int keyword = 0; keyword < grp.Keywords(); keyword ++) {
      PvlKeyword key = grp[keyword];

      if (key.IsNamed("Time")) {
        // Pull the selections start and end time out
        iTime kernelStart = (string) key[0];
        iTime kernelEnd   = (string) key[1];

        // If the kernel times inside of the requested times we 
        // set the matchTime to be true.
        if ((kernelStart <= start) && (kernelEnd >= end)) {
          matchTime = true;
        }
      }
      else if (key.IsNamed("Match")) {
        try {
          iString matchGroup = key[0];
          iString matchKey   = key[1];
          iString matchValue = key[2];

          iString cubeValue = (string)cube.FindGroup(matchGroup)[matchKey];
          cubeValue.ConvertWhiteSpace();
          cubeValue.Compress();
          cubeValue.Trim(" ");
          cubeValue.UpCase();
  
          matchValue.ConvertWhiteSpace();
          matchValue.Compress();
          matchValue.Trim(" ");
          matchValue.UpCase();

          // If strings are not the same, match automatically fails
          if (cubeValue.compare(matchValue) != 0) {
            matchKeywords = false;
          }
        } catch (Isis::iException &e) {
          // This error is thrown if the group or keyword do not exist in 'lab'
          matchKeywords = false;
        }
      }
      else if(key.IsNamed("CameraVersion")) {
        try {
          for(int camVersionKeyIndex = 0; camVersionKeyIndex < key.Size(); camVersionKeyIndex++) {
            bool versionMatch = false;
            iString val = (std::string)key[camVersionKeyIndex];
            iString commaTok;
  
            while ((commaTok = val.Token(",")).length() > 0) {
              if (commaTok.find('-') != std::string::npos) {
                iString dashTok;
                int start = commaTok.Token("-").ToInteger();
                int end = commaTok.Token("-").ToInteger();
                int direction;
                direction = (start<=end) ? 1 : -1;
                // Save the entire range of bands
                for (int version = start; version != end+direction; version+=direction) {
                  if(version == cameraVersion) {
                    versionMatch = true;
                  }
                }
              }
              // This token is a single band specification
              else {
                if(commaTok.ToInteger() == cameraVersion) {
                  versionMatch = true;
                }
              }
            }
  
            if(!versionMatch)  {
              matchKeywords = false;
            }

          }
        } catch (Isis::iException &e) {
          matchKeywords = false;
        }
      }
    }

    if(matchKeywords && matchTime) {
      lastType = type;
      filesFound.push(Kernel(spiceInit::kernelTypeEnum(type), GetFile(grp)));
    }
  }

  return filesFound;
}

// Load the DB with the defined BASE and MISSION info for each type of kernel
void KernelDb::LoadSystemDb (const std::string &mission) {

  // Get the base DataDirectory
  PvlGroup &datadir = Preference::Preferences().FindGroup("DataDirectory");
  string baseDir = datadir["Base"];

  // Get the mission DataDirectory
  string missionDir = datadir[mission];

  // Load the leapsecond DB
  Isis::Filename lsDb(baseDir + "/" + "kernels/lsk/kernels.????.db");
  lsDb.HighestVersion();
  p_kernelData.Read (lsDb.Expanded());

  // Load the target attitude shape DB
  Isis::Filename tasDbPath(missionDir + "/" + "kernels/pck");
  if( tasDbPath.Exists()) {
    Isis::Filename tasDb(missionDir + "/" + "kernels/pck/kernels.????.db");
    tasDb.HighestVersion();
    p_kernelData.Read (tasDb.Expanded());
  }
  else {
    Isis::Filename baseTasDb(baseDir + "/" + "kernels/pck/kernels.????.db");
    baseTasDb.HighestVersion();
    p_kernelData.Read (baseTasDb.Expanded());
  }

  // Load the target position DB
  Isis::Filename tpDbPath(missionDir + "/" + "kernels/tspk");
  if( tpDbPath.Exists()) {
    Isis::Filename tpDb(missionDir + "/" + "kernels/tspk/kernels.????.db");
    tpDb.HighestVersion();
    p_kernelData.Read (tpDb.Expanded());
  }
  else {
    Isis::Filename baseTpDb(baseDir + "/" + "kernels/spk/kernels.????.db");
    baseTpDb.HighestVersion();
    p_kernelData.Read (baseTpDb.Expanded());
  }

  // Load the DEM DB
  Isis::Filename demDb(baseDir + "/" + "dems/kernels.????.db");
  demDb.HighestVersion();
  p_kernelData.Read (demDb.Expanded());

  // Load the mission specific spacecraft pointing DB
  Isis::Filename spDb(missionDir + "/" + "kernels/ck/kernels.????.db");
  spDb.HighestVersion();
  p_kernelData.Read (spDb.Expanded());

  // Load the mission specific frame DB
  Isis::Filename fDb(missionDir + "/" + "kernels/fk/kernels.????.db");
  fDb.HighestVersion();
  p_kernelData.Read (fDb.Expanded());

  // Load the mission specific instrument DB
  Isis::Filename iDb(missionDir + "/" + "kernels/ik/kernels.????.db");
  iDb.HighestVersion();
  p_kernelData.Read (iDb.Expanded());

  // Load the mission specific spacecraft clock DB
  Isis::Filename scDb(missionDir + "/" + "kernels/sclk/kernels.????.db");
  scDb.HighestVersion();
  p_kernelData.Read (scDb.Expanded());

  // Load the mission specific spacecraft position DB
  Isis::Filename sposDb(missionDir + "/" + "kernels/spk/kernels.????.db");
  sposDb.HighestVersion();
  p_kernelData.Read (sposDb.Expanded());

  // Load the mission specific instrument addendum DB
  Isis::Filename iakDb(missionDir + "/" + "kernels/iak/kernels.????.db");
  iakDb.HighestVersion();
  p_kernelData.Read (iakDb.Expanded());
}

std::vector<std::string> KernelDb::GetFile (PvlGroup &grp) {
  std::vector<std::string> files;
  //PvlKeyword kfile = grp["File"];

  for (int i=0; i<grp.Keywords(); i++) {
    PvlKeyword kfile = grp[i];
    if (kfile.Name()!="File") continue;

    // Two values in the "File" keyword from the DB,
    // indicates an ISIS preference in the DataDirectory section
    // and a filename
    if (kfile.Size() == 2) {
      string pref = kfile[0];
      string version = kfile[1];
      Isis::Filename filename("$" + pref + "/" + version);
      if (filename.Expanded().find('?') != string::npos) filename.HighestVersion();
      files.push_back(filename.OriginalPath() + "/" + filename.Name());
    }
    // One value in "File" indicates a full file spec
    else if (kfile.Size() == 1) {
      Isis::Filename filename(kfile[0]);
      if (filename.Expanded().find('?') != string::npos) filename.HighestVersion();
      files.push_back(filename.OriginalPath() + "/" + filename.Name());
    } else {
      string msg = "Invalid File keyword value in [Group = ";
      msg += grp.Name() + "] in database file [";
      msg += p_filename + "]";
      throw iException::Message(iException::Parse,msg, _FILEINFO_);
    }
  }

  return files;
}

const bool KernelDb::Better (const std::string newType, const std::string oldType) {
  return Better(spiceInit::kernelTypeEnum(newType), spiceInit::kernelTypeEnum(oldType));
}

const bool KernelDb::Better (const spiceInit::kernelTypes nType, const spiceInit::kernelTypes oType) {
  if ((nType & p_kernelTypes) && (nType >= oType)) return true;

  return false;
}

