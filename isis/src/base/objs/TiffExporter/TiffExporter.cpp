#include "TiffExporter.h"

#include "Buffer.h"
#include "Filename.h"
#include "IException.h"

using namespace Isis;


namespace Isis {
  /**
   * Construct the importer.
   *
   * @param inputName The name of the input image
   */
  TiffExporter::TiffExporter() : StreamExporter() {
    m_image = NULL;
    m_raster = NULL;

    setExtension("tif");
  }


  /**
   * Destruct the importer.
   */
  TiffExporter::~TiffExporter() {
    _TIFFfree(m_raster);
    m_raster = NULL;

    TIFFClose(m_image);
    m_image = NULL;
  }


  void TiffExporter::createBuffer() {
    PixelType type = getPixelType();
    int mult = (type == Isis::UnsignedByte) ? 1 : 2;
    int size = samples() * bands() * mult;
    if ((m_raster = (char *) malloc(sizeof(char) * size)) == NULL) {
      throw IException(IException::Programmer,
          "Could not allocate enough memory", _FILEINFO_);
    }
  }


  void TiffExporter::write(Filename outputName, int quality) {
    // Open the output image
    if ((m_image = TIFFOpen(outputName.Expanded().c_str(), "w")) == NULL) {
      throw IException(IException::Programmer,
          "Could not open outgoing image", _FILEINFO_);
    }

    TIFFSetField(m_image, TIFFTAG_IMAGEWIDTH, samples());
    TIFFSetField(m_image, TIFFTAG_IMAGELENGTH, lines());
    TIFFSetField(m_image, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
    TIFFSetField(m_image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

    TIFFSetField(m_image, TIFFTAG_PHOTOMETRIC,
        bands() == 1 ? PHOTOMETRIC_MINISBLACK : PHOTOMETRIC_RGB);

    PixelType type = getPixelType();
    int bps = (type == Isis::UnsignedByte) ? 8 : 16;
    TIFFSetField(m_image, TIFFTAG_BITSPERSAMPLE, bps);

    TIFFSetField(m_image, TIFFTAG_SAMPLESPERPIXEL, bands());

    ImageExporter::write(outputName, quality);
  }


  void TiffExporter::setBuffer(int s, int b, int dn) const {
    PixelType type = getPixelType();
    int index = s * bands() + b;
    switch (type) {
      case UnsignedByte:
        ((unsigned char *) m_raster)[index] = (unsigned char) dn;
        break;
      case SignedWord:
        ((short int *) m_raster)[index] = (short int) dn;
        break;
      case UnsignedWord:
        ((short unsigned int *) m_raster)[index] = (short unsigned int) dn;
        break;
      default:
        throw IException(IException::Programmer,
            "Invalid pixel type for data [" + iString(type) + "]",
            _FILEINFO_);
    }
  }


  void TiffExporter::writeLine(int l) const {
    if (!TIFFWriteScanline(m_image, m_raster, l)) {
      throw IException(IException::Programmer,
          "Could not write image", _FILEINFO_);
    }
  }


  bool TiffExporter::canWriteFormat(iString format) {
    return format == "tiff";
  }
};

