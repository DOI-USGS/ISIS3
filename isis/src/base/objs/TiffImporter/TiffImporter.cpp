#include "TiffImporter.h"

#include <sstream>

#include "Filename.h"
#include "IException.h"
#include "ProcessByLine.h"

using namespace std;
using namespace Isis;


namespace Isis {
  TiffImporter::TiffImporter(Filename inputName) : ImageImporter(inputName) {
    // Open the TIFF image
    m_image = NULL;
    if ((m_image = TIFFOpen(inputName.Expanded().c_str(), "r")) == NULL) {
      throw IException(IException::Programmer,
          "Could not open incoming image", _FILEINFO_);
    }

    // Get its constant dimensions.  Note, height seems to get reset to 0 if
    // called before setting width.
    uint32 height;
    TIFFGetField(m_image, TIFFTAG_IMAGELENGTH, &height);
    setLines(height);

    uint32 width;
    TIFFGetField(m_image, TIFFTAG_IMAGEWIDTH, &width);
    setSamples(width);

    TIFFGetField(m_image, TIFFTAG_SAMPLESPERPIXEL, &m_samplesPerPixel);

    // Setup the width and height of the image
    unsigned long imagesize = lines() * samples();
    m_raster = NULL;
    if ((m_raster = (uint32 *) malloc(sizeof(uint32) * imagesize)) == NULL) {
      throw IException(IException::Programmer,
          "Could not allocate enough memory", _FILEINFO_);
    }

    // Read the image into the memory buffer
    if (TIFFReadRGBAImage(m_image, samples(), lines(), m_raster, 0) == 0) {
      throw IException(IException::Programmer,
          "Could not read image", _FILEINFO_);
    }

    // Deal with photometric interpretations
    if (TIFFGetField(m_image, TIFFTAG_PHOTOMETRIC, &m_photo) == 0) {
      throw IException(IException::Programmer,
          "Image has an undefined photometric interpretation", _FILEINFO_);
    }

    setDefaultBands();
  }


  TiffImporter::~TiffImporter() {
    _TIFFfree(m_raster);
    m_raster = NULL;

    TIFFClose(m_image);
    m_image = NULL;
  }


  int TiffImporter::samplesPerPixel() const {
    return m_samplesPerPixel;
  }


  bool TiffImporter::isGrayscale() const {
    return
      m_photo == PHOTOMETRIC_MINISWHITE ||
      m_photo == PHOTOMETRIC_MINISBLACK;
  }


  bool TiffImporter::isRgb() const {
    return !isGrayscale() && samplesPerPixel() <= 3;
  }


  bool TiffImporter::isArgb() const {
    return !isGrayscale() && samplesPerPixel() > 3;
  }


  int TiffImporter::getPixel(int s, int l) const {
    l = lines() - l - 1;
    int index = l * samples() + s;
    return m_raster[index];
  }


  int TiffImporter::getGray(int pixel) const {
    // Weighted formula taken from Qt documentation on converting an RGB
    // value to grayscale:
    // http://qt-project.org/doc/qt-4.8/qcolor.html#qGray-2
    uint32 red = TIFFGetR(pixel);
    uint32 green = TIFFGetG(pixel);
    uint32 blue = TIFFGetB(pixel);
    return (red * 11 + green * 16 + blue * 5) / 32;
  }


  int TiffImporter::getRed(int pixel) const {
    return TIFFGetR(pixel);
  }


  int TiffImporter::getGreen(int pixel) const {
    return TIFFGetG(pixel);
  }


  int TiffImporter::getBlue(int pixel) const {
    return TIFFGetB(pixel);
  }


  int TiffImporter::getAlpha(int pixel) const {
    return TIFFGetA(pixel);
  }
};

