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
#include "IException.h"
#include "CameraFactory.h"
#include "Preference.h"
#include "FileName.h"
#include "IString.h"
#include "KernelDb.h"
#include "PvlObject.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"

using namespace std;
using namespace Isis;

// Create a new empty KernelDb
KernelDb::KernelDb(const unsigned int kernelTypes) {
  p_filename = "None";
  p_kernelTypes = kernelTypes;
}


// Create a new KernelDb object using a filename
KernelDb::KernelDb(const std::string &dbName, const unsigned int kernelTypes) :
  p_kernelData(dbName) {
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
Kernel KernelDb::LeapSecond(Pvl &lab) {
  return FindLast("LeapSecond", lab);
}


// Return the highest version of all TargetAttitudeShape kernels identified
// by the DB
Kernel KernelDb::TargetAttitudeShape(Pvl &lab) {
  return FindLast("TargetAttitudeShape", lab);
}


// Return the highest version of all TargetPosition kernels identified
// by the DB
Kernel KernelDb::TargetPosition(Pvl &lab) {
  return FindLast("TargetPosition", lab);
}


// Return the last matching time of all SpacecraftPointing kernels identified
// by the DB
std::priority_queue< Kernel > KernelDb::SpacecraftPointing(Pvl &lab) {
  return FindAll("SpacecraftPointing", lab);
}


// Return the highest version of all SpaceCraftClock kernels identified
// by the DB
Kernel KernelDb::SpacecraftClock(Pvl &lab) {
  return FindLast("SpacecraftClock", lab);
}


// Return the last matching time of all SpacecraftPosition kernels identified
// by the DB
Kernel KernelDb::SpacecraftPosition(Pvl &lab) {
  return FindLast("SpacecraftPosition", lab);
}


// Return the last Instrument kernal found which matches the criteria in the DB
Kernel KernelDb::Instrument(Pvl &lab) {
  return FindLast("Instrument", lab);
}


// Return the highest version of all Frame kernels identified
// by the DB
Kernel KernelDb::Frame(Pvl &lab) {
  return FindLast("Frame", lab);
}


// Return the last InstrumentAddendum kernal found which matches the
// criteria in the DB
Kernel KernelDb::InstrumentAddendum(Pvl &lab) {
  return FindLast("InstrumentAddendum", lab);
}


// Return the highest version DEM found which matches the criteria in the DB
Kernel KernelDb::Dem(Pvl &lab) {
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

std::priority_queue< Kernel > KernelDb::FindAll(const std::string &entry,
    Isis::Pvl &lab) {
  std::priority_queue< Kernel > filesFound;
  Isis::PvlObject &cube = lab.FindObject("IsisCube");
  int cameraVersion = CameraFactory::CameraVersion(lab);

  // Make sure the entry has been loaded into memory
  if(!p_kernelData.HasObject(entry)) return filesFound;

  // Get the start and end time for the cube
  iTime start(((string) cube.FindGroup("Instrument")["StartTime"]));
  iTime end;
  if(cube.FindGroup("Instrument").HasKeyword("StopTime")) {
    end = ((string) cube.FindGroup("Instrument")["StopTime"]);
  }
  else {
    end = ((string) cube.FindGroup("Instrument")["StartTime"]);
  }

  // Get the object and search through it's groups
  PvlObject &obj = p_kernelData.FindObject(entry);
  for(int groupIndex = obj.Groups() - 1; groupIndex >= 0; groupIndex --) {
    // Get the group and start testing the cases in the keywords
    // to see if they all match this cube
    PvlGroup &grp = obj.Group(groupIndex);

    // If the group name isn't selection, skip it.
    if(!grp.IsNamed("Selection")) continue;

    IString type = "";

    // Make sure the type is better
    if(grp.HasKeyword("Type")) {
      type = (string) grp["Type"];
      if(!(spiceInit::kernelTypeEnum(type) & p_kernelTypes)) {
        continue;
      }
    }

    bool startMatches = Matches(lab, grp, start, cameraVersion);
    bool endMatches = Matches(lab, grp, end, cameraVersion);

    if(startMatches && endMatches) {
      // Simple case - the selection simply matches
      filesFound.push(Kernel(spiceInit::kernelTypeEnum(type), GetFile(grp)));
    }
    else if(startMatches) {
      // Well, the selection start matched but not the end.
      // Let's look for a second selection to handle overlap areas.
      for(int endTimeIndex = obj.Groups() - 1;
          endTimeIndex >= 0;
          endTimeIndex--) {
        PvlGroup &endTimeGrp = obj.Group(endTimeIndex);

        // The second selection must:
        //   Not be the current selection
        //   Be a selection
        //   Be of the same quality
        //   Match the end time
        //
        // *If start time is also matched, do not merge and simply take the
        // secondary match
        if(endTimeIndex == groupIndex) continue;
        if(!endTimeGrp.IsNamed("Selection")) continue;
        if(grp.HasKeyword("Type") != endTimeGrp.HasKeyword("Type")) continue;
        if(grp.HasKeyword("Type") &&
            grp["Type"] != endTimeGrp["Type"]) continue;
        if(!Matches(lab, endTimeGrp, end, cameraVersion)) continue;

        // Better match is true if we find a full overlap
        bool betterMatch = false;

        // True if we have matching time ranges = we want to merge
        bool endTimesMatch = true;

        // Check for matching time ranges
        for(int keyIndex = 0;
            !betterMatch && keyIndex < grp.Keywords();
            keyIndex++) {
          PvlKeyword &key = grp[keyIndex];

          if(!key.IsNamed("Time")) continue;

          iTime timeRangeStart((string)key[0]);
          iTime timeRangeEnd((string)key[1]);

          bool thisEndMatches = Matches(lab, endTimeGrp, timeRangeEnd, cameraVersion);
          endTimesMatch = endTimesMatch && thisEndMatches;

          if(Matches(lab, endTimeGrp, start, cameraVersion) && Matches(lab, endTimeGrp, end, cameraVersion)) {
            // If we run into a continuous kernel, we want to take that in all
            //   cases.
            betterMatch = true;
          }
        }

        // No exact match but time ranges overlap, merge the selections
        if(!betterMatch && endTimesMatch) {
          std::vector< std::string > startMatchFiles = GetFile(grp);
          std::vector< std::string > endMatchFiles = GetFile(endTimeGrp);

          while(endMatchFiles.size()) {
            startMatchFiles.push_back(
              endMatchFiles[endMatchFiles.size() - 1]
            );
            endMatchFiles.pop_back();
          }

          filesFound.push(
            Kernel(spiceInit::kernelTypeEnum(type), startMatchFiles)
          );
        }
        // Found an exact match, use it
        else if(betterMatch) {
          filesFound.push(
            Kernel(spiceInit::kernelTypeEnum(type), GetFile(endTimeGrp))
          );
        }
      }
    }
  }

  return filesFound;
}

const bool KernelDb::Matches(Pvl &lab, PvlGroup &kernelDbGrp,
                             iTime timeToMatch, int cameraVersion) {
  // These are the conditions that make this test pass:
  //   1) No time OR At least one matching time
  //   2) All keyword matches are true OR No keyword matches present
  //
  // In order to accomplish this, matchTime is initialized to true and remains
  // as such if and only if there are no time conditionals. If Time keywords
  // exist, one of them has to set matchTime to true. The matchKeywords is
  // true until a mismatch is found.
  Isis::PvlObject &cube = lab.FindObject("IsisCube");
  bool matchTime = !kernelDbGrp.HasKeyword("Time");
  bool matchKeywords = true;

  // First, the time search. Loop through the keywords, if the name isn't
  //  Time then skip it. If it is, then get the start/end times and keep
  //  looking until one is found.
  for(int keyword = 0; keyword < kernelDbGrp.Keywords(); keyword ++) {
    PvlKeyword key = kernelDbGrp[keyword];

    if(key.IsNamed("Time")) {
      // Pull the selections start and end time out
      iTime kernelStart = (string) key[0];
      iTime kernelEnd   = (string) key[1];

      // If the kernel times inside of the requested times we
      // set the matchTime to be true.
      if((kernelStart <= timeToMatch) && (kernelEnd >= timeToMatch)) {
        matchTime = true;
      }
    }
    else if(key.IsNamed("Match")) {
      try {
        IString matchGroup = key[0];
        IString matchKey   = key[1];
        IString matchValue = key[2];

        IString cubeValue = (string)cube.FindGroup(matchGroup)[matchKey];
        cubeValue.ConvertWhiteSpace();
        cubeValue.Compress();
        cubeValue.Trim(" ");
        cubeValue.UpCase();

        matchValue.ConvertWhiteSpace();
        matchValue.Compress();
        matchValue.Trim(" ");
        matchValue.UpCase();

        // If strings are not the same, match automatically fails
        if(cubeValue.compare(matchValue) != 0) {
          matchKeywords = false;
        }
      }
      catch(IException &e) {
        // This error is thrown if the group or keyword do not exist in 'lab'
        matchKeywords = false;
      }
    }
    else if(key.IsNamed("CameraVersion")) {
      try {
        for(int camVersionKeyIndex = 0;
            camVersionKeyIndex < key.Size();
            camVersionKeyIndex++) {

          bool versionMatch = false;
          IString val = (std::string)key[camVersionKeyIndex];
          IString commaTok;

          while((commaTok = val.Token(",")).length() > 0) {
            if(commaTok.find('-') != std::string::npos) {
              IString dashTok;
              int start = commaTok.Token("-").ToInteger();
              int end = commaTok.Token("-").ToInteger();
              int direction;
              direction = (start <= end) ? 1 : -1;
              // Save the entire range of bands
              for(int version = start;
                  version != end + direction;
                  version += direction) {
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

          if(!versionMatch) {
            matchKeywords = false;
          }

        }
      }
      catch(IException &) {
        matchKeywords = false;
      }
    }
  }

  return matchKeywords && matchTime;
}

// Load the DB with the defined BASE and MISSION info for each type of kernel
void KernelDb::LoadSystemDb(const std::string &mission) {

  // Get the base DataDirectory
  PvlGroup &datadir = Preference::Preferences().FindGroup("DataDirectory");
  string baseDir = datadir["Base"];

  // Get the mission DataDirectory
  string missionDir = datadir[mission];

  // Load the leapsecond DB
  Isis::FileName lsDb(baseDir + "/" + "kernels/lsk/kernels.????.db");
  lsDb = lsDb.highestVersion();
  p_kernelData.Read(lsDb.expanded());

  // Load the target attitude shape DB
  Isis::FileName tasDbPath(missionDir + "/" + "kernels/pck");
  if(tasDbPath.fileExists()) {
    Isis::FileName tasDb(missionDir + "/" + "kernels/pck/kernels.????.db");
    tasDb = tasDb.highestVersion();
    p_kernelData.Read(tasDb.expanded());
  }
  else {
    Isis::FileName baseTasDb(baseDir + "/" + "kernels/pck/kernels.????.db");
    baseTasDb = baseTasDb.highestVersion();
    p_kernelData.Read(baseTasDb.expanded());
  }

  // Load the target position DB
  Isis::FileName tpDbPath(missionDir + "/" + "kernels/tspk");
  if(tpDbPath.fileExists()) {
    Isis::FileName tpDb(missionDir + "/" + "kernels/tspk/kernels.????.db");
    tpDb = tpDb.highestVersion();
    p_kernelData.Read(tpDb.expanded());
  }
  else {
    Isis::FileName baseTpDb(baseDir + "/" + "kernels/spk/kernels.????.db");
    baseTpDb = baseTpDb.highestVersion();
    p_kernelData.Read(baseTpDb.expanded());
  }

  // Load the DEM DB
  Isis::FileName demDb(baseDir + "/" + "dems/kernels.????.db");
  demDb = demDb.highestVersion();
  p_kernelData.Read(demDb.expanded());

  // Load the mission specific spacecraft pointing DB
  Isis::FileName spDb(missionDir + "/" + "kernels/ck/kernels.????.db");
  spDb = spDb.highestVersion();
  p_kernelData.Read(spDb.expanded());

  // Load the mission specific frame DB
  Isis::FileName fDb(missionDir + "/" + "kernels/fk/kernels.????.db");
  fDb = fDb.highestVersion();
  p_kernelData.Read(fDb.expanded());

  // Load the mission specific instrument DB
  Isis::FileName iDb(missionDir + "/" + "kernels/ik/kernels.????.db");
  iDb = iDb.highestVersion();
  p_kernelData.Read(iDb.expanded());

  // Load the mission specific spacecraft clock DB
  Isis::FileName scDb(missionDir + "/" + "kernels/sclk/kernels.????.db");
  scDb = scDb.highestVersion();
  p_kernelData.Read(scDb.expanded());

  // Load the mission specific spacecraft position DB
  Isis::FileName sposDb(missionDir + "/" + "kernels/spk/kernels.????.db");
  sposDb = sposDb.highestVersion();
  p_kernelData.Read(sposDb.expanded());

  // Load the mission specific instrument addendum DB
  Isis::FileName iakDb(missionDir + "/" + "kernels/iak/kernels.????.db");
  iakDb = iakDb.highestVersion();
  p_kernelData.Read(iakDb.expanded());
}

std::vector<std::string> KernelDb::GetFile(PvlGroup &grp) {
  std::vector<std::string> files;
  //PvlKeyword kfile = grp["File"];

  for(int i = 0; i < grp.Keywords(); i++) {
    PvlKeyword kfile = grp[i];
    if(kfile.Name() != "File") continue;

    // Two values in the "File" keyword from the DB,
    // indicates an ISIS preference in the DataDirectory section
    // and a filename
    if(kfile.Size() == 2) {
      string pref = kfile[0];
      string version = kfile[1];
      Isis::FileName filename("$" + pref + "/" + version);
      if (filename.isVersioned())
        filename = filename.highestVersion();
      files.push_back(filename.originalPath() + "/" + filename.name());
    }
    // One value in "File" indicates a full file spec
    else if(kfile.Size() == 1) {
      Isis::FileName filename(kfile[0]);
      if (filename.isVersioned())
        filename = filename.highestVersion();
      files.push_back(filename.originalPath() + "/" + filename.name());
    }
    else {
      string msg = "Invalid File keyword value in [Group = ";
      msg += grp.Name() + "] in database file [";
      msg += p_filename + "]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
  }

  return files;
}

const bool KernelDb::Better(const std::string newType,
                            const std::string oldType) {
  return Better(spiceInit::kernelTypeEnum(newType),
                spiceInit::kernelTypeEnum(oldType));
}

const bool KernelDb::Better(const spiceInit::kernelTypes nType,
                            const spiceInit::kernelTypes oType) {
  if((nType & p_kernelTypes) && (nType >= oType)) return true;

  return false;
}


