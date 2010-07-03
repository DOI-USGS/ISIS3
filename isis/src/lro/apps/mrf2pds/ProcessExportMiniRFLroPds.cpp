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
#include "iException.h"
#include "iString.h"
#include "Filename.h"
#include "PvlFormatPds.h"
#include "ProcessExportMiniRFLroPds.h"

using namespace std;

namespace Isis {

  /**
    * Create a standard PDS label for type IMAGE using the mrf
    * formatting
    *
    * @author sprasad (2/1/2010)
    */
  Pvl &ProcessExportMiniRFLroPds::StandardPdsLabel(const ProcessExportPds::PdsFileType type) {
    p_label = new Pvl;

    p_formatter = new PvlFormatPds("$lro/translations/mrfExportRoot.typ");
    p_label->SetFormat(p_formatter);
    p_label->SetTerminator("END");

    if(type == ProcessExportPds::Image) {
      CreateImageLabel();
    }
    else {
      string msg = "Unsupported PDS output type";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }
    return *p_label;
  }


  /**
   * Create Image label with mrf formatting
   *
   * @author sprasad (2/1/2010)
   */
  void ProcessExportMiniRFLroPds::CreateImageLabel(void) {

    Pvl &mainPvl = *p_label;

    if(p_exportType == ProcessExportPds::Stream) {
      StreamImageRoot(mainPvl);
    }
    else if(p_exportType == ProcessExportPds::Fixed) {
      FixedImageRoot(mainPvl);
    }
    else {
      string msg = "Invalid PDS export type";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    StandardImageImage(mainPvl);

    // The IMAGE_MAP_PROJECTION group is located in the ROOT for PDS IMAGEs. The
    // standard routines will add the IMAGE_MAP_PROJECTION correctly
    StandardAllMapping(mainPvl);
    mainPvl.GetFormat()->Add("$lro/translations/mrfExportAllMapping.typ");
  }

}
