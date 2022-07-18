#ifndef QtExporter_h
#define QtExporter_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ImageExporter.h"

#include "Constants.h"

class QImage;

namespace Isis {
  /**
   * @brief Exports cubes into one of several formats with Qt facilities
   *
   * Takes a series of single-banded Isis cubes and exports them into one of
   * several possible standard image formats using Qt's QImage structure to
   * handle reading the data into memory and setting individual pixel values.
   * Set Qt's documentation on QImageWriter for a complete list of supported
   * formats:
   *
   *   http://qt-project.org/doc/qt-4.8/qimagewriter.html#supportedImageFormats
   *
   * While Qt can be used to export Isis cubes to TIFF images, it is generally
   * recommended to use the TiffExporter class instead, which uses LibTIFF
   * version 4 to import big TIFFs.  Qt, as of version 4.8, can only import
   * images <2GB in size.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2012-04-03 Travis Addair
   *
   * @internal
   *   @history 2012-04-04 Travis Addair - Added documentation.
   *   @history 2013-06-05 Jeannie Backer - Added initialize() method and replaced call to
   *                           setInput(). Changed ImageExporter calls to new method names, where
   *                           needed. References #1380.
   *   @history 2015-02-12 Jeffrey Covington - Added compression parameter to write() method.
   *                           Fixes #1745.
   *
   */
  class QtExporter : public ImageExporter {
    public:
      QtExporter(QString format);
      virtual ~QtExporter();

      virtual void setGrayscale(ExportDescription &desc);
      virtual void setRgb(ExportDescription &desc);
      virtual void setRgba(ExportDescription &desc);

      virtual void write(FileName outputName, int quality=100,
                         QString compression="none", UserInterface *ui = nullptr);

      static bool canWriteFormat(QString format);

    protected:
      virtual void writeGrayscale(vector<Buffer *> &in) const;
      virtual void writeRgb(vector<Buffer *> &in) const;
      virtual void writeRgba(vector<Buffer *> &in) const;

      void checkDataSize(BigInt samples, BigInt lines, int bands);

    private:
      void initialize(ExportDescription &desc);

      //! Structure holding all output image data in memory.
      QImage *m_qimage;

      //! The lowercase abbreviated format of the output image.
      QString m_format;
  };
};


#endif
