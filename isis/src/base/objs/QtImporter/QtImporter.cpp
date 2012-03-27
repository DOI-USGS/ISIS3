#include "QtImporter.h"

#include <QImage>

#include "Filename.h"
#include "IException.h"

using namespace Isis;


namespace Isis {
  QtImporter::QtImporter(Filename inputName) : ImageImporter(inputName) {
    m_qimage = NULL;

    m_qimage = new QImage(inputName.Expanded());
    if (m_qimage->isNull()) {
      throw IException(IException::User,
          "The file [" + inputName.Expanded() +
          "] does not contain a recognized image format",
          _FILEINFO_);
    }

    setSamples(m_qimage->width());
    setLines(m_qimage->height());
    setDefaultBands();
  }


  QtImporter::~QtImporter() {
    delete m_qimage;
    m_qimage = NULL;
  }


  bool QtImporter::isGrayscale() const {
    return m_qimage->isGrayscale();
  }


  bool QtImporter::isRgb() const {
    return !isGrayscale() && !isArgb();
  }


  bool QtImporter::isArgb() const {
    return m_qimage->hasAlphaChannel();
  }


  void QtImporter::updateRawBuffer(int line, int band) const {
  }


  int QtImporter::getPixel(int s, int l) const {
    return m_qimage->pixel(s, l);
  }


  int QtImporter::getGray(int pixel) const {
    return qGray(pixel);
  }


  int QtImporter::getRed(int pixel) const {
    return qRed(pixel);
  }


  int QtImporter::getGreen(int pixel) const {
    return qGreen(pixel);
  }


  int QtImporter::getBlue(int pixel) const {
    return qBlue(pixel);
  }


  int QtImporter::getAlpha(int pixel) const {
    return qAlpha(pixel);
  }
};

