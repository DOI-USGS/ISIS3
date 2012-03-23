#include "ImageImporter.h"

#include <QImageReader>

#include "Buffer.h"
#include "CubeAttribute.h"
#include "Filename.h"
#include "JP2Importer.h"
#include "ProcessByLine.h"
#include "QtImporter.h"
#include "SpecialPixel.h"
#include "TiffImporter.h"

using namespace Isis;

namespace Isis {
  ImageImporter::ImageImporter(Filename inputName) {
    m_inputName = NULL;
    m_outCube = NULL;

    m_inputName = new Filename(inputName);
    m_outCube = new Cube;

    m_nullMin = DBL_MAX;
    m_nullMax = DBL_MIN;
    m_lrsMin = DBL_MAX;
    m_lrsMax = DBL_MIN;
    m_hrsMin = DBL_MAX;
    m_hrsMax = DBL_MIN;
  }


  ImageImporter::~ImageImporter() {
    delete m_inputName;
    m_inputName = NULL;

    delete m_outCube;
    m_outCube = NULL;
  }


  void ImageImporter::operator()(Buffer &out) const {
    GetChannelMethod getChannel = getBandChannel(out.Band());

    int l = out.Line() - 1;
    for (int s = 0; s < out.SampleDimension(); s++) {
      out[s] = testSpecial((this->*getChannel)(getPixel(s, l)));
    }
  }


  Cube * ImageImporter::import(Filename outputName) {
    CubeAttributeOutput att;
    return import(outputName, att);
  }


  Cube * ImageImporter::import(Filename outputName, CubeAttributeOutput &att) {
    ProcessByLine p;
    Cube *cube = createOutput(outputName, att);

    Pvl *label = cube->getLabel();
    PvlGroup bandBin("BandBin");

    PvlKeyword name("Name");
    if (bands() == 1) {
      name += "Gray";
    }
    else if (bands() == 3 || bands() == 4) {
      name += "Red";
      name += "Green";
      name += "Blue";
      if (bands() == 4) name += "Alpha";
    }
    else {
      throw IException(IException::Programmer,
          "Cannot interpret BandBin for [" + iString(bands()) + "] band image",
          _FILEINFO_);
    }
    bandBin += name;
    label->AddGroup(bandBin);

    p.SetInputCube(cube);
    p.ProcessCubeInPlace(*this, false);
    p.EndProcess();

    return cube;
  }


  Cube * ImageImporter::createOutput(
      Filename outputName, CubeAttributeOutput &att) {

    m_outCube->setDimensions(samples(), lines(), bands());
    m_outCube->create(outputName.Expanded(), att);
    return m_outCube;
  }


  void ImageImporter::setDefaultBands() {
    setBands((isGrayscale()) ? 1 : (isArgb()) ? 4 : 3);
  }


  void ImageImporter::setNullRange(double min, double max) {
    m_nullMin = min;
    m_nullMax = max;
  }


  void ImageImporter::setLrsRange(double min, double max) {
    m_lrsMin = min;
    m_lrsMax = max;
  }


  void ImageImporter::setHrsRange(double min, double max) {
    m_hrsMin = min;
    m_hrsMax = max;
  }


  void ImageImporter::setSamples(int s) {
    m_samples = s;
  }


  void ImageImporter::setLines(int l) {
    m_lines = l;
  }


  void ImageImporter::setBands(int b) {
    if (b == 2 || b > 4)
      throw IException(IException::Programmer,
          "Cannot create an image with [" + iString(b) + "] bands",
          _FILEINFO_);

    m_bands = b;
  }


  int ImageImporter::samples() const {
    return m_samples;
  }


  int ImageImporter::lines() const {
    return m_lines;
  }


  int ImageImporter::bands() const {
    return m_bands;
  }


  Filename ImageImporter::filename() const {
    return *m_inputName;
  }


  /**
   * Tests the pixel. If it is valid it will return the dn value,
   * otherwise it will return the Isis special pixel value that
   * corresponds to it
   *
   * @param pixel The double precision value that represents a
   *              pixel.
   * @return double  The double precision value representing the
   *         pixel will return as a valid dn or changed to an isis
   *         special pixel.
   */
  double ImageImporter::testSpecial(double pixel) const {
    if (pixel <= m_nullMax && pixel >= m_nullMin) {
      return Isis::NULL8;
    }
    else if (pixel <= m_hrsMax && pixel >= m_hrsMin) {
      return Isis::HIGH_REPR_SAT8;
    }
    else if (pixel <= m_lrsMax && pixel >= m_lrsMin) {
      return Isis::LOW_REPR_SAT8;
    }
    else {
      return pixel;
    }
  }


  ImageImporter::GetChannelMethod
    ImageImporter::getBandChannel(int band) const {

    GetChannelMethod getChannel;

    if (bands() == 1) {
      getChannel = &ImageImporter::getGray;
    }
    else {
      switch (band) {
        case 1:
          getChannel = &ImageImporter::getRed;
          break;
        case 2:
          getChannel = &ImageImporter::getGreen;
          break;
        case 3:
          getChannel = &ImageImporter::getBlue;
          break;
        case 4:
          getChannel = &ImageImporter::getAlpha;
          break;
        default:
          throw IException(IException::Programmer,
              "Cannot determine channel for band [" + iString(band) + "]",
              _FILEINFO_);
      }
    }

    return getChannel;
  }


  ImageImporter * ImageImporter::fromFilename(Filename inputName) {
    ImageImporter *importer = NULL;

    QString format = QImageReader::imageFormat(inputName.Expanded());
    if (format == "tiff") {
      importer = new TiffImporter(inputName);
    }
    else if (format != "") {
      importer = new QtImporter(inputName);
    }
    else {
      importer = new JP2Importer(inputName);
    }

    return importer;
  }
};

