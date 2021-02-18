/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "HiLab.h"
#include "IException.h"

using namespace std;

namespace Isis {
  /**
   * Constructs a HiLab Object
   * @param cube The cube containing the HiRise labels to be processed.
   */
  HiLab::HiLab(Cube *cube) {
    PvlGroup group = cube->group("Instrument");
    p_cpmmNumber = group["CpmmNumber"];
    p_channel = group["ChannelNumber"];

    if(group.hasKeyword("Summing")) {
      p_bin = group["Summing"];
    }
    else {
      std::string msg = "Cannot find required Summing keyword in label";
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    if(group.hasKeyword("Tdi")) {
      p_tdi = group["Tdi"];
    }
    else {
      std::string msg = "Cannot find required Tdi keyword in label";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  }

  int HiLab::getCcd() {
    const int cpmm2ccd[] = {0, 1, 2, 3, 12, 4, 10, 11, 5, 13, 6, 7, 8, 9};
    return cpmm2ccd[p_cpmmNumber];
  }
}
