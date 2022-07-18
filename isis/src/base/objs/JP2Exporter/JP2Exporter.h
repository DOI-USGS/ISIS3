#ifndef JP2Exporter_h
#define JP2Exporter_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "StreamExporter.h"

namespace Isis {
  class JP2Encoder;

  /**
   * @brief Exports cubes into JPEG 2000 images
   *
   * A streamed exporter for JPEG 2000 images.  Can write an arbitrarily large
   * set of single-band Isis cubes to an arbitrarily large JPEG 2000 image with
   * the given pixel type.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2012-04-03 Travis Addair
   *
   * @internal
   *   @history 2012-04-04 Travis Addair - Added documentation.
   *   @history 2013-06-05 Jeannie Backer - Removed "get" prefix from ImageExporter method calls.
   *                           References #1380.
   *   @history 2015-02-12 Jeffrey Covington - Added compression parameter to write() method.
   *                           Fixes #1745.
   */
  class JP2Exporter : public StreamExporter {
    public:
      JP2Exporter();
      virtual ~JP2Exporter();

      virtual void write(FileName outputName, int quality=100,
                         QString compression="none", UserInterface *ui = nullptr);

      static bool canWriteFormat(QString format);

    protected:
      virtual void createBuffer();

      virtual void setBuffer(int s, int b, int dn) const;
      virtual void writeLine(int l) const;

    private:
      //! Object responsible for writing data to the output image
      JP2Encoder *m_encoder;

      //! Two dimensional array containing all color channels for a line
      char **m_buffer;
  };
};


#endif
