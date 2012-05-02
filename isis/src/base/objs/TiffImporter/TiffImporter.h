#ifndef TiffImporter_h
#define TiffImporter_h

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
#include "tiffio.h"

namespace Isis {
  /**
   * @brief Imports TIFF images as Isis cubes.
   *
   * Takes a TIFF input image and imports it into Isis in the cube format.
   * Unlike Qt's facilities for importing TIFFs, this class is capable of
   * importing images >2GB, provided the user's machine has sufficient RAM to
   * hold such an image in memory.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2012-03-16 Travis Addair
   *
   * @internal
   *   @todo Read chunks of the image into memory at a time, not the entire
   *         image.
   *   @history 2012-03-28 Travis Addair - Added documentation.
   *
   */
  class TiffImporter : public ImageImporter {
    public:
      TiffImporter(FileName inputName);
      virtual ~TiffImporter();

      virtual bool isGrayscale() const;
      virtual bool isRgb() const;
      virtual bool isArgb() const;

    protected:
      int samplesPerPixel() const;

      virtual void updateRawBuffer(int line, int band) const;
      virtual int getPixel(int s, int l) const;

      virtual int getGray(int pixel) const;
      virtual int getRed(int pixel) const;
      virtual int getGreen(int pixel) const;
      virtual int getBlue(int pixel) const;
      virtual int getAlpha(int pixel) const;

    private:
      //! LibTIFF representation of the input image.
      TIFF *m_image;

      //! Buffer holding the raw TIFF image in memory.
      uint32 *m_raster;

      //! The enumerated photometric interpretation of the input image.
      uint16 m_photo;

      //! The number of "samples" (bands in Isis terms) in the input image.
      uint16 m_samplesPerPixel;
  };
};


#endif
