#ifndef QtImporter_h
#define QtImporter_h

#include "ImageImporter.h"

class QImage;

namespace Isis {
  /**
   * @author 2012-03-16 Travis Addair
   *
   * @internal
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
      QImage *m_qimage;
  };
};


#endif
