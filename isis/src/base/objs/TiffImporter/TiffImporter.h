#ifndef TiffImporter_h
#define TiffImporter_h

#include "ImageImporter.h"
#include "tiffio.h"

namespace Isis {
  /**
   * @author 2012-03-16 Travis Addair
   *
   * @internal
   */
  class TiffImporter : public ImageImporter {
    public:
      TiffImporter(Filename inputName);
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
      TIFF *m_image;
      uint32 *m_raster;

      uint16 m_photo;
      uint16 m_samplesPerPixel;
  };
};


#endif
