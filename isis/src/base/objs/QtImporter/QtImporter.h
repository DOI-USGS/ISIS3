#ifndef QtImporter_h
#define QtImporter_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ImageImporter.h"

class QImage;

namespace Isis {
  /**
   * @brief Imports a series of standard image formats with Qt facilities.
   *
   * Takes a standard input image format and imports it into Isis in the cube
   * format using Qt's QImage structure to handle reading the data into memory.
   * See Qt's documentation on QImageReader for a complete list of supported
   * formats:
   *
   *   http://qt-project.org/doc/qt-4.8/qimagereader.html#supportedImageFormats
   *
   * While Qt can be used to import TIFF images into Isis, it is generally
   * recommended to use the TiffImporter class instead, which uses LibTIFF
   * version 4 to import big TIFFs.  Qt, as of version 4.8, can only import
   * images <2GB in size.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2012-03-16 Travis Addair
   *
   * @internal
   *   @history 2012-03-28 Travis Addair - Added documentation.
   *
   */
  class QtImporter : public ImageImporter {
    public:
      QtImporter(FileName inputName);
      virtual ~QtImporter();

      virtual bool isGrayscale() const;
      virtual bool isRgb() const;
      virtual bool isArgb() const;

    protected:
      virtual void updateRawBuffer(int line, int band) const;
      virtual int getPixel(int s, int l) const;

      virtual int getGray(int pixel) const;
      virtual int getRed(int pixel) const;
      virtual int getGreen(int pixel) const;
      virtual int getBlue(int pixel) const;
      virtual int getAlpha(int pixel) const;

    private:
      //! Structure holding all input image data in memory.
      QImage *m_qimage;
  };
};


#endif
