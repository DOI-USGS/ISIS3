#ifndef NirsImportFits_h
#define NirsImportFits_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QString>

#include <fstream>
#include <vector>
#include <string>

#include "FileName.h"
#include "PvlObject.h"

namespace Isis {

  /**
   * @brief Import a FITS file with a label description
   *
   * This class interrogates a \b simple file formatted with the Flexible Image
   * Transport System (FITS) file and provides tools to converts it to an ISIS
   * image cube.
   *
   * Example of interrogation of a FITS file in an NirsImportFits object.
   * <code>
   *   NirsImportFits fits(fitsfile, fitsLabelName);
   *   Pvl label;
   *   label.addGroup(fits.label());
   * </code>
   *
   * @ingroup Utility
   *
   * @author 2013-11-07 Kris Becker
   *
   * @internal
   */
  class NirsImportFits {
    public:
      NirsImportFits();
      NirsImportFits(const FileName &fitsfile,
                 const QString &fitsLabelName="FitsLabel");
      ~NirsImportFits();

      int samples() const;
      int lines() const;
      int bands() const;

      PvlObject label() const;

      void load(const QString &fitsfile,
                const QString &fitsLabelName = "FitsLabel");

    private:
      void init();
      PvlObject parseLabel(std::ifstream &in, const QString &fitLabelName);

      // Private instance variables
      FileName  m_file;       //!< FITS file name
      int       m_lines;      //!< Number lines in image
      int       m_samples;    //!< Number samples in image
      int       m_bands;      //!< Number bands in image
      PvlObject  m_label;      //!< FITS label converted to ISIS format

  };

}
#endif
