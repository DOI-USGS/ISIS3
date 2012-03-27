#ifndef JP2Importer_h
#define JP2Importer_h

#include "ImageImporter.h"
#include "PixelType.h"

namespace Isis {
  class JP2Decoder;

  /**
   * @author 2012-03-16 Travis Addair
   *
   * @internal
   */
  class JP2Importer : public ImageImporter {
    public:
      JP2Importer(Filename inputName);
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
      JP2Decoder *m_decoder;
      mutable char **m_buffer; // TODO hack, we need non-const process method
      Isis::PixelType m_pixelType;
  };
};


#endif
