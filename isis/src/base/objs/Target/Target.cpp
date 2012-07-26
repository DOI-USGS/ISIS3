/**
 * @file
 * $Revision: 1.24 $
 * $Date: 2010/04/09 22:31:16 $
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
#include "Target.h"
#include "Distance.h"
#include "IException.h"
#include "NaifStatus.h"
#include "Pvl.h"
#include "Spice.h"


using namespace std;

namespace Isis {

  /**
   * Constructs a Target object and loads target information.
   *
   * @param lab Label containing Instrument and Kernels groups.
   *
   * @internal
   * @history 2005-11-29 Debbie A. Cook - Original version
   */

  // TODO: what is needed to initialize?
  Target::Target(Pvl &lab) {

    // Initialize members
    m_radii = new Distance[3];

    PvlGroup kernels = lab.FindGroup("Kernels", Pvl::Traverse);
    m_bodyCode = new SpiceInt;

    PvlGroup &inst = lab.FindGroup("Instrument", Pvl::Traverse);
    *m_name = inst["TargetName"][0];

    if (iString(*m_name).UpCase() == "SKY") {
      m_radii[0] = m_radii[1] = m_radii[2] = Distance(1000.0, Distance::Meters);
      m_sky = true;
    }
    else {
      *m_bodyCode = naifBodyCode();
      m_sky = false;
    }
    // Override it if it exists in the labels
    if (kernels.HasKeyword("NaifBodyCode"))
      *m_bodyCode = (int) kernels["NaifBodyCode"];
  }

  /**
   * Destroys the Target object
   */
  Target::~Target() {
    NaifStatus::CheckErrors();

    if (m_radii != NULL) {
      delete [] m_radii;
      m_radii = NULL;
    }

    if (m_name != NULL) {
      delete m_name;
      m_name = NULL;
    }
  }

  //! Return if our target is the sky
  bool Target::isSky() const {
        return m_sky;
  }

  //! Return target name
  iString Target::name() const {
        return *m_name;
      }

  /**
   * Sets the radii of the body.
   *
   * @param r[] Radii of the target in kilometers
   */
  void Target::setRadii(Distance r[3]) {
    m_radii[0] = r[0];
    m_radii[1] = r[1];
    m_radii[2] = r[2];
  }

  /**
   * Returns the radii of the body in km. The radii are obtained from the
   * appropriate SPICE kernel for the body specified by TargetName in the
   * Instrument group of the labels.
   *
   * @param r[] Radii of the target in kilometers
   */
  void Target::radii(Distance r[3]) const {
    r[0] = m_radii[0];
    r[1] = m_radii[1];
    r[2] = m_radii[2];
  }

  /**
   * This returns the NAIF body code of the target indicated in the labels.
   *
   * @return @b SpiceInt NAIF body code
   *
   */
  SpiceInt Target::naifBodyCode() const {
    SpiceInt code;
    SpiceBoolean found;
    bodn2c_c(m_name->c_str(), &code, &found);
    if(!found) {
      string msg = "Could not convert Target [" + *m_name +
                   "] to NAIF code";
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    return (int) code;
  }


  /**
   * This sets the NAIF body code of a "SKY" target to the observing spacecraft.
   *
   */
  void Target::setSky(SpiceInt bodyCode) const {
      *m_bodyCode = bodyCode;  // TODO Is this the best way for now???
  }
}
