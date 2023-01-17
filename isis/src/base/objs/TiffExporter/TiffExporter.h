#ifndef TiffExporter_h
#define TiffExporter_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "StreamExporter.h"

#include <tiffio.h>

namespace Isis {
  /**
   * @brief Exports cubes into TIFF images
   *
   * A streamed exporter for TIFF images.  Can write an arbitrarily large set of
   * single-band Isis cubes to an arbitrarily large TIFF image with the given
   * pixel type.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2012-04-03 Travis Addair
   *
   * @internal
   *   @history 2012-04-04 Travis Addair - Added documentation.
   *   @history 2012-08-28 Steven Lambright - Fixed some problems with memory
   *                         allocations/deallocations and a flag on the tiff
   *                         files was not being set, but it was necessary to
   *                         successfully write out tiff files (TIFFTAG_ROWSPERSTRIP),
   *                         References #579.
   *   @history 2013-06-05 Jeannie Backer - Removed "get" prefix from ImageExporter
   *                           method calls. References #1380.
   *   @history 2015-02-10 Jeffrey Covington - Changed default compression to no
   *                         compression. Added compression parameter to write()
   *                         method. Fixes #1745.
   *
   */
  class TiffExporter : public StreamExporter {
    public:
      TiffExporter();
      virtual ~TiffExporter();

      virtual void write(FileName outputName, int quality=100,
                         QString compression="none", UserInterface *ui = nullptr);

      static bool canWriteFormat(QString format);

    protected:
      virtual void createBuffer();

      virtual void setBuffer(int s, int b, int dn) const;
      virtual void writeLine(int l) const;

    private:
      //! Object responsible for writing data to the output image
      TIFF *m_image;

      //! Array containing all color channels for a line
      unsigned char *m_raster;
  };
};


#endif
