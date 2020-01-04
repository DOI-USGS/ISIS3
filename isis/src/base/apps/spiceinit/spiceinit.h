#ifndef spiceinit_h
#define spiceinit_h

#include <vector>

#include <QString>

#include "Cube.h"
#include "Pvl.h"
#include "UserInterface.h"

namespace Isis {

  /**
   * Struct specifying how to spiceinit a Cube.
   */
  struct spiceinitOptions {
    // Enumeration describing where to get the target shape model
    enum shapeSource {
      ELLIPSOID, //!< The shapemodel is a triaxial ellipsoid
      RINGPLANE, //!< The shapemodel is the targets ring plane
      SYSTEM,    //!< The shapemodel is the system default
      USER       //!< The shapemodel is entered by the user
    };

    bool web = false; //!< If the spice web service should be used
    bool attach = true; // If the spice data should be attached to the Cube file
    bool cksmithed = false; // If system controlled CKs should be used
    bool ckrecon = true; // If system reconstructed CKs should be used
    bool ckpredicted = false; // If system predicted CKs should be used
    bool cknadir = false; // If pointing should be computed from the spacecraft state
    bool spksmithed = false; // If system controlled CKs should be used
    bool spkrecon = true; // If system reconstructed CKs should be used
    bool spkpredicted = false; // If system predicted CKs should be used
    std::vector<QString> lsk = std::vector<QString>(); //!< override leap second kernels
    std::vector<QString> pck = std::vector<QString>(); //!< override planetary constant kernels
    std::vector<QString> tspk = std::vector<QString>(); //!< override target state kernels
    std::vector<QString> ik = std::vector<QString>(); //!< override instrument kernels
    std::vector<QString> sclk = std::vector<QString>(); //!< override spacecraft clock kernels
    std::vector<QString> ck = std::vector<QString>(); //!< override camera kernels
    std::vector<QString> fk = std::vector<QString>(); //!< override frame kernels
    std::vector<QString> spk = std::vector<QString>(); //!< override spacecraft state kernels
    std::vector<QString> iak = std::vector<QString>(); //!< override instrument addendum kernels
    std::vector<QString> extra = std::vector<QString>(); //!< override extra kernels
    std::vector<QString> model = std::vector<QString>(); //!< override DEMs
    shapeSource shape = SYSTEM; //!< Where to get the target shape model from
    double startpad = 0.0; //!< Image start time padding in seconds
    double endpad = 0.0; //!< Image stop time padding in seconds
    // URL for the spice web service
    QString url = "https://services.isis.astrogeology.usgs.gov/cgi-bin/spiceinit.cgi";
    int port = 443; //!< port for the spice web service
  };

  extern void spiceinit(UserInterface &ui,
                        Pvl *log = nullptr);
  extern void spiceinit(Cube *icube,
                        const spiceinitOptions &options = spiceinitOptions(),
                        Pvl *log = nullptr);
}

#endif
