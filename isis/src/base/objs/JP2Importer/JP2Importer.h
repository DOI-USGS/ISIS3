#ifndef JP2Importer_h
#define JP2Importer_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
