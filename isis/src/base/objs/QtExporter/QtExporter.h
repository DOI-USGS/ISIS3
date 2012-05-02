#ifndef QtExporter_h
#define QtExporter_h

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

#include "ImageExporter.h"

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
   *
   */
  class QtExporter : public ImageExporter {
    public:
      QtExporter(iString format);
      virtual ~QtExporter();

      virtual void setGrayscale(ExportDescription &desc);
      virtual void setRgb(ExportDescription &desc);
      virtual void setRgba(ExportDescription &desc);

      virtual void write(FileName outputName, int quality=100);

      static bool canWriteFormat(iString format);

    protected:
      virtual void writeGrayscale(vector<Buffer *> &in) const;
      virtual void writeRgb(vector<Buffer *> &in) const;
      virtual void writeRgba(vector<Buffer *> &in) const;

      void checkDataSize(BigInt samples, BigInt lines, int bands);

    private:
      //! Structure holding all output image data in memory.
      QImage *m_qimage;

      //! The lowercase abbreviated format of the output image.
      iString m_format;
  };
};


#endif
