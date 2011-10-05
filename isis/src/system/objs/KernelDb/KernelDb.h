#ifndef KernelDb_h
#define KernelDb_h

/**
 * @file
 * $Revision: 1.7 $
 * $Date: 2009/07/28 21:01:18 $
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

#include <iostream>
#include <string>
#include <queue>

#include "Pvl.h"
#include "iTime.h"
#include "iString.h"

namespace spiceInit {

  enum kernelTypes {
    Predicted = 1,
    Nadir = 2,
    Reconstructed = 4,
    Smithed = 8
  };

  inline kernelTypes kernelTypeEnum(const std::string &type) {
    Isis::iString strng = type;
    strng.ConvertWhiteSpace();
    strng.Compress();
    strng.Trim(" ");
    strng.UpCase();
    if(strng == "PREDICTED") return Predicted;
    if(strng == "NADIR") return Nadir;
    if(strng == "RECONSTRUCTED") return Reconstructed;
    if(strng == "SMITHED") return Smithed;

    return (kernelTypes)0;
  }

  inline const char *kernelTypeEnum(const kernelTypes &type) {
    if(type == Predicted) return "Predicted";
    if(type == Nadir) return "Nadir";
    if(type == Reconstructed) return "Reconstructed";
    if(type == Smithed) return "Smithed";

    return "Unknown";
  }
};

class Kernel;

/**
 * @brief KernelDb class
 *
 * If you would like to see KernelDb being used
 *         in implementation, see spiceinit.
 *
 * @ingroup System
 *
 * @internal
 * @history 2005-12-27 Jacob Danton - Added support for multiple
 *                         files and fixed a bug in SearchMatch.
 * @history 2007-07-09 Steven Lambright - Removed inheritance from PVL
 * @history 2007-10-25 Steven Koechle - Corrected Smithed Enum
 *                         fixed search methods.
 * @history 2007-07-09 Steven Lambright - Added Kernel class and multiple-ck
 *                         return
 * @history 2009-05-12 Steven Lambright - Added Camera Version Checking
 * @history 2010-07-19 Steven Lambright - Added kernel selection merging
 * @history 2011-10-05 Jacob Danton - The placement of int cameraVersion =
 *                         CameraFactory::CameraVersion(lab); was the reason
 *                         that spiceinit ran so slowly. Its placement in the
 *                         "Matches" function resulted in CameraFactory reading
 *                         in the "Camera.plugin" file for every single kernel
 *                         in both the CK and SPK kernels.????.db files. That
 *                         means, for LRO, it was read in around 2100 times.
 *                         By moving this line of code out of the "Matches"
 *                         function and into the "FindAll" function (the only
 *                         one that calls "Matches") and then passing it as a
 *                         parameter, I was able to reduce the time to about 5%
 *                         (from almost 4 minutes to under 15 seconds).
 */
class KernelDb {

  public:
    // constructor
    KernelDb(const unsigned int kernelTypes);
    KernelDb(const std::string &dbName, const unsigned int kernelTypes);
    KernelDb(std::istream &dbStream, const unsigned int kernelTypes);

    // destructor
    ~KernelDb() {};

    // Members for getting kernels
    Kernel LeapSecond(Isis::Pvl &lab);
    Kernel TargetAttitudeShape(Isis::Pvl &lab);
    Kernel TargetPosition(Isis::Pvl &lab);
    std::priority_queue< Kernel > SpacecraftPointing(Isis::Pvl &lab);
    Kernel SpacecraftClock(Isis::Pvl &lab);
    Kernel SpacecraftPosition(Isis::Pvl &lab);
    Kernel Instrument(Isis::Pvl &lab);
    Kernel Frame(Isis::Pvl &lab);
    Kernel InstrumentAddendum(Isis::Pvl &lab);
    Kernel Dem(Isis::Pvl &lab);

    Kernel FindLast(const std::string &entry, Isis::Pvl &lab);
    std::priority_queue< Kernel > FindAll(const std::string &entry,
                                          Isis::Pvl &lab);

    void LoadSystemDb(const std::string &mission);

    // Returns true if the newType is allowed and better than
    // the oldType
    const bool Better(const std::string newType, const std::string oldType);
    const bool Better(const spiceInit::kernelTypes newType,
                      const spiceInit::kernelTypes oldType);
    const bool Matches(Isis::Pvl &lab, Isis::PvlGroup &kernelDbGrp,
                       Isis::iTime timeToMatch, int cameraVersion);

  protected:
    std::string p_filename;

  private:
    std::vector<std::string> GetFile(Isis::PvlGroup &grp);
    unsigned int p_kernelTypes;
    Isis::Pvl p_kernelData;
};

class Kernel {
  public:
    Kernel() {
      kernelType = (spiceInit::kernelTypes)0;
    }

    Kernel(spiceInit::kernelTypes type, const std::vector<std::string> &data) {
      kernelType = type;
      kernels = data;
    }

    bool operator<(const Kernel &other) const {
      return (this->kernelType < other.kernelType);
    }

    const int size() {
      return kernels.size();
    }
    void push_back(const std::string &str) {
      kernels.push_back(str);
    }

    std::string &operator[](const int index) {
      return kernels[index];
    }
    std::string operator[](const int index) const {
      return kernels[index];
    }

    spiceInit::kernelTypes kernelType;
    std::vector<std::string> kernels;
};

#endif
