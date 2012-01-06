/**
 *   Unless noted otherwise, the portions of Isis written by the
 *   USGS are public domain. See individual third-party library
 *   and package descriptions for intellectual property
 *   information,user agreements, and related information.
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

#include "ProcessExportPds.h"

namespace Isis {
  class PvlFormatPds;

  /**
   * @brief Process class for LRO specific PDS image
   *
   * This class extends the ProcessExportPds class to add Mini RF
   * LRO specific PVL.
   *
   * @author 2009-11-04 Sharmila Prasad
   *
   * @internal
   *   @history 2009-11-04 Sharmila Prasad - Original version
   *   @history 2010-03-09 Sharmila Prasad - Changed class name to
   *                           ProcessExportMiniRFLroPds to avoid conflicts with
   *                           functionality of LRO objects
   */

  class ProcessExportMiniRFLroPds : public Isis::ProcessExportPds {
    public:
      ProcessExportMiniRFLroPds() {};
      ~ProcessExportMiniRFLroPds() {};

      virtual Pvl &StandardPdsLabel(const ProcessExportPds::PdsFileType type = ProcessExportPds::Image);
      virtual void CreateImageLabel(void);
  };
}
