#ifndef JP2Importer_h
#define JP2Importer_h

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
#include "PixelType.h"

namespace Isis {
  class JP2Decoder;

  /**
   * @brief Imports JPEG 2000 images as Isis cubes.
   *
   * Takes a JPEG 2000 input image and imports it into Isis in the cube format.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2012-03-16 Travis Addair
   *
   * @internal
   *   @history 2012-03-28 Travis Addair - Added documentation.
   *
   */
  class JP2Importer : public ImageImporter {
    public:
      JP2Importer(FileName inputName);
      virtual ~JP2Importer();

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

      int getFromBuffer(int s, int b) const;

    private:
      //! Takes a raw stream of JPEG 2000 data and reads it into a buffer.
      JP2Decoder *m_decoder;

      //! Buffer that stores a line of JPEG 2000 data and all its color bands.
      char **m_buffer;

      //! Pixel type of the input image needed for reading data into the buffer.
      Isis::PixelType m_pixelType;
  };
};


#endif
