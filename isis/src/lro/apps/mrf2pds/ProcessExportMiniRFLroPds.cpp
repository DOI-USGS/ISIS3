/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
