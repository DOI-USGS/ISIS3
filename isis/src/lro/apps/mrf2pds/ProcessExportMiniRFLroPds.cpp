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
#include <iostream>
#include <sstream>
#include <cmath>

#include "Pvl.h"
#include "PvlFormat.h"
#include "IException.h"
#include "IString.h"
#include "FileName.h"
#include "PvlFormatPds.h"
#include "ProcessExportMiniRFLroPds.h"

using namespace std;

namespace Isis {
  ProcessExportMiniRFLroPds::ProcessExportMiniRFLroPds() {}
  ProcessExportMiniRFLroPds::~ProcessExportMiniRFLroPds() {}

  /**
    * Create a standard PDS label for type IMAGE using the mrf
    * formatting
    *
    * @author sprasad (2/1/2010)
    */
  Pvl &ProcessExportMiniRFLroPds::StandardPdsLabel(const ProcessExportPds::PdsFileType type) {
    m_label = new Pvl;

    m_formatter = new PvlFormatPds("$ISISROOT/appdata/translations/MrfExportRoot.typ");
    m_label->setFormat(m_formatter);
    m_label->setTerminator("END");

    if (type == ProcessExportPds::Image) {
      CreateImageLabel();
    }
    else {
      string msg = "Unsupported PDS output type";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    return *m_label;
  }


  /**
   * Create Image label with mrf formatting
   *
   * @author sprasad (2/1/2010)
   */
  void ProcessExportMiniRFLroPds::CreateImageLabel(void) {

    Pvl &mainPvl = *m_label;

    if (m_exportType == ProcessExportPds::Stream) {
      StreamImageRoot(mainPvl);
    }
    else if (m_exportType == ProcessExportPds::Fixed) {
      FixedImageRoot(mainPvl);
    }
    else {
      string msg = "Invalid PDS export type";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    StandardImageImage(mainPvl);

    // The IMAGE_MAP_PROJECTION group is located in the ROOT for PDS IMAGEs. The
    // standard routines will add the IMAGE_MAP_PROJECTION correctly
    StandardAllMapping(mainPvl);
    mainPvl.format()->add("$ISISROOT/appdata/translations/MrfExportAllMapping.typ");
  }

}
