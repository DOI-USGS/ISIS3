#ifndef processexportminirflropds_h
#define processexportminirflropds_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
   *   @history 2012-11-21 Jeannie Backer - Changed m_ to p_ to fit parent class
   *                           member variables. Moved constructor and destructor
   *                           implementation to cpp file. References #678
   */

  class ProcessExportMiniRFLroPds : public Isis::ProcessExportPds {
    public:
      ProcessExportMiniRFLroPds();
      ~ProcessExportMiniRFLroPds();

      virtual Pvl &StandardPdsLabel(const ProcessExportPds::PdsFileType type = ProcessExportPds::Image);
      virtual void CreateImageLabel(void);
  };
}

#endif
