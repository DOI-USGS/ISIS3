#include "TiffImporter.h"

#include "FileName.h"
#include "IException.h"
#include "iString.h"

namespace Isis {
  /**
   * Construct the importer.
   *
   * @param inputName The name of the input image
   */
  TiffImporter::TiffImporter(FileName inputName) : ImageImporter(inputName) {
    // Open the TIFF image
    m_image = NULL;
    if ((m_image = TIFFOpen(inputName.expanded().c_str(), "r")) == NULL) {
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


  /**
   * Destruct the importer.
   */
  TiffImporter::~TiffImporter() {
    _TIFFfree(m_raster);
    m_raster = NULL;

    TIFFClose(m_image);
    m_image = NULL;
  }


  /**
   * The number of "samples" (bands in Isis terms) per pixel in the input image.
   * Combined with the photometric interpretation, this can be used to determine
   * the color mode of the input image.  We need both pieces of information
   * because grayscale images are not guaranteed to have only one sample per
   * pixel.
   *
   * @return The samples per pixel
   */
  int TiffImporter::samplesPerPixel() const {
    return m_samplesPerPixel;
  }


  /**
   * Tests to see if the input image has a "min is white" or "min is black"
   * photometric interpretation, implying grayscale (no RGB/A).
   *
   * @return True if the image is grayscale, false otherwise
   */
  bool TiffImporter::isGrayscale() const {
    return
      m_photo == PHOTOMETRIC_MINISWHITE ||
      m_photo == PHOTOMETRIC_MINISBLACK;
  }


  /**
   * Tests to see if the input image is neither grayscale nor has more than
   * three samples per pixel, implying RGB (no alpha).
   *
   * @return True if the image is RGB, false otherwise
   */
  bool TiffImporter::isRgb() const {
    return !isGrayscale() && samplesPerPixel() <= 3;
  }


  /**
   * Tests to see if the input image is not grayscale and has more than three
   * samples per pixel, implying RGBA.
   *
   * @return True if the image is RGBA, false otherwise
   */
  bool TiffImporter::isArgb() const {
    return !isGrayscale() && samplesPerPixel() > 3;
  }


  /**
   * Does nothing as LibTIFF reads the entire input image into memory, and
   * therefore does not need to be updated throughout the import process.
   *
   * @param line Current line of the output buffer
   * @param band Current band of the output buffer
   */
  void TiffImporter::updateRawBuffer(int line, int band) const {
  }


  /**
   * Returns a representation of a pixel for the input format that can then be
   * broken down into specific gray or RGB/A components.
   *
   * @param s The sample of the desired pixel
   * @param l The line of the desired pixel
   *
   * @return The pixel at the given sample and line of the input with all
   *         channel info
   */
  int TiffImporter::getPixel(int s, int l) const {
    l = lines() - l - 1;
    int index = l * samples() + s;
    return m_raster[index];
  }


  /**
   * Retrieves the gray component of the given pixel.  In LibTIFF, even
   * grayscale images are given RGB channels, so converts the RGB components
   * into grayscale regardless of input photometric interpretation.
   *
   * @param pixel Representation of a LibTIFF pixel value
   *
   * @return The gray component
   */
  int TiffImporter::getGray(int pixel) const {
    return convertRgbToGray(pixel);
  }


  /**
   * Retrieves the red component of the given pixel.
   *
   * @param pixel Representation of a LibTIFF pixel value
   *
   * @return The red component
   */
  int TiffImporter::getRed(int pixel) const {
    return TIFFGetR(pixel);
  }


  /**
   * Retrieves the green component of the given pixel.
   *
   * @param pixel Representation of a LibTIFF pixel value
   *
   * @return The green component
   */
  int TiffImporter::getGreen(int pixel) const {
    return TIFFGetG(pixel);
  }


  /**
   * Retrieves the blue component of the given pixel.
   *
   * @param pixel Representation of a LibTIFF pixel value
   *
   * @return The blue component
   */
  int TiffImporter::getBlue(int pixel) const {
    return TIFFGetB(pixel);
  }


  /**
   * Retrieves the alpha component of the given pixel.
   *
   * @param pixel Representation of a LibTIFF pixel value
   *
   * @return The alpha component
   */
  int TiffImporter::getAlpha(int pixel) const {
    return TIFFGetA(pixel);
  }
};

