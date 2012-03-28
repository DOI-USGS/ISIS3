#ifndef QtImporter_h
#define QtImporter_h

/**
 * @file
 * $Revision: 1.17 $
 * $Date: 2010/03/22 19:44:53 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
      QtImporter(Filename inputName);
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
