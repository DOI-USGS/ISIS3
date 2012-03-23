#ifndef JP2Importer_h
#define JP2Importer_h

#include "ImageImporter.h"

namespace Isis {
  /**
   * @author 2012-03-16 Travis Addair
   *
   * @internal
   */
  class JP2Importer : public ImageImporter {
    public:
      JP2Importer(Filename inputName);
      virtual ~JP2Importer();

      using ImageImporter::import;
      virtual Cube * import(Filename outputName, CubeAttributeOutput &att);

      virtual bool isGrayscale() const;
      virtual bool isRgb() const;
      virtual bool isArgb() const;

    protected:
      virtual int getPixel(int s, int l) const;

      virtual int getGray(int pixel) const;
      virtual int getRed(int pixel) const;
      virtual int getGreen(int pixel) const;
      virtual int getBlue(int pixel) const;
      virtual int getAlpha(int pixel) const;

    private:
      int m_pixelBytes;
      bool m_signed;
  };
};


#endif
