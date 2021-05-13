#ifndef CISSCALFILE_H
#define CISSCALFILE_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>

#include <QString>

#include "TextFile.h"

namespace Isis {
  /**
   * @brief Extends <b>TextFile</b>  to handle Cassini ISS
   *        calibration files.
   * This class was created as an extension of <b>TextFile</b> to
   *        be able to read in Cassini ISS calibration files used
   *        by the Isis <B>ciss2isis</B> and <B>cisscal</B>
   *        applications. It is able to read PDS style text files
   *        and skip all header info that exists before the tag
   *        <TT>"\begindata"</TT>.
   *
   * @ingroup Cassini-Huygens
   * @author 2008-03-27 Jeannie Walldren
   * @internal
   *   @history 2008-03-27 Jeannie Walldren - Original Version.
   *   @history 2011-05-03 Jeannie Walldren - Fixed documentation of ingroup
   *                          name and added ISISdisclaimer.
   */
  class CisscalFile : public TextFile {
    public:
      CisscalFile(const QString &filename, const char *openmode = "input",
                  const char *extension = "");
      //!> Destructor closes the text file.
      ~CisscalFile() {
        TextFile::Close();
      };
      bool GetLine(QString &line);
    protected:
      bool p_begindataFound; //!> Flag variable indicates whether the tag <code>"\begindata"</code> has been found.
      bool p_GetLine(QString &line);
  };
};
#endif
