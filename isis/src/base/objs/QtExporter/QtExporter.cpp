#include "QtExporter.h"

#include <QImage>
#include <QImageWriter>

#include "Buffer.h"
#include "ExportDescription.h"
#include "FileName.h"
#include "IException.h"

using namespace Isis;


namespace Isis {
  /**
   * Construct the Qt exporter.
   *
   * @param format The format to export to
   */
  QtExporter::QtExporter(iString format) : ImageExporter() {
    m_qimage = NULL;
    m_format = format;

    // Setup the required extension and world file
    if (format == "png")
      setExtension("png");
    else if (format == "jpeg")
      setExtension("jpg");
    else if (format == "tiff")
      setExtension("tif");
    else if (format == "gif")
      setExtension("gif");
    else if (format == "bmp")
      setExtension("bmp");
  }


  /**
   * Destruct the exporter.
   */
  QtExporter::~QtExporter() {
    delete m_qimage;
    m_qimage = NULL;
  }


  /**
   * Set the input with the description generically, check the data size for a
   * single-band image with the established dimensions, initialize the image
   * with the Indexed8 format, and setup the color table from 0 to 256.
   *
   * @param desc Export description containing necessary channel information
   */
  void QtExporter::setGrayscale(ExportDescription &desc) {
    Cube *cube = setInput(desc);
    checkDataSize(cube->getSampleCount(), cube->getLineCount(), 1);
    m_qimage = new QImage(
        cube->getSampleCount(), cube->getLineCount(), QImage::Format_Indexed8);
    m_qimage->setNumColors(256);

    // Create the color table (black = 0 to white = 255)
    QVector<QRgb> colors;
    for (int i = 0; i < 256; i++) colors.push_back(qRgb(i, i, i));
    m_qimage->setColorTable(colors);
  }


  /**
   * Set the input with the description generically, check the data size for a
   * three-band image with the established dimensions, and initialize the image
   * with the RGB32 format.
   *
   * @param desc Export description containing necessary channel information
   */
  void QtExporter::setRgb(ExportDescription &desc) {
    Cube *cube = setInput(desc);
    checkDataSize(cube->getSampleCount(), cube->getLineCount(), 3);
    m_qimage = new QImage(
        cube->getSampleCount(), cube->getLineCount(), QImage::Format_RGB32);
  }


  /**
   * Set the input with the description generically, check the data size for a
   * four-band image with the established dimensions, and initialize the image
   * with the ARGB32 format.
   *
   * @param desc Export description containing necessary channel information
   */
  void QtExporter::setRgba(ExportDescription &desc) {
    Cube *cube = setInput(desc);
    checkDataSize(cube->getSampleCount(), cube->getLineCount(), 4);
    m_qimage = new QImage(
        cube->getSampleCount(), cube->getLineCount(), QImage::Format_ARGB32);
  }


  /**
   * Write a line of grayscale data to the output image.
   *
   * @param in Vector containing a single grayscale input line
   */
  void QtExporter::writeGrayscale(vector<Buffer *> &in) const {
    Buffer &grayLine = *in[0];

    // Loop for each column and load the pixel, which will
    // be in the range of [0,255]
    int l = grayLine.Line() - 1;
    for (int s = 0; s < grayLine.SampleDimension(); s++) {
      int dn = getPixel(grayLine[s]);

      // Since the plausable "exception" thrown by setPixel cannot be caught,
      //  the following if statement does it informally.
      m_qimage->setPixel(s, l, dn);
      if (!m_qimage->valid(s, l)) {
        iString msg = "QT has detected your file size as exceeding 2GB.";
        msg += " While your image might be under 2GB, your image labels are more";
        msg += " than likely pushing the file size over 2GB.";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
  }


  /**
   * Write a line of RGB data to the output image.
   *
   * @param in Vector containing three input lines (red, green, blue)
   */
  void QtExporter::writeRgb(vector<Buffer *> &in) const {
    Buffer &redLine = *in[0];
    Buffer &greenLine = *in[1];
    Buffer &blueLine = *in[2];

    QRgb *line = (QRgb *) m_qimage->scanLine(redLine.Line() - 1);
    for (int s = 0; s < redLine.SampleDimension(); s++) {
      int red = getPixel(redLine[s]);
      int green = getPixel(greenLine[s]);
      int blue = getPixel(blueLine[s]);

      line[s] = qRgb(red, green, blue);
    }
  }


  /**
   * Write a line of RGBA data to the output image.
   *
   * @param in Vector containing four input lines (red, green, blue, alpha)
   */
  void QtExporter::writeRgba(vector<Buffer *> &in) const {
    Buffer &redLine = *in[0];
    Buffer &greenLine = *in[1];
    Buffer &blueLine = *in[2];
    Buffer &alphaLine = *in[3];

    QRgb *line = (QRgb *) m_qimage->scanLine(redLine.Line() - 1);
    for (int s = 0; s < redLine.SampleDimension(); s++) {
      int red = getPixel(redLine[s]);
      int green = getPixel(greenLine[s]);
      int blue = getPixel(blueLine[s]);
      int alpha = getPixel(alphaLine[s]);

      line[s] = qRgba(red, green, blue, alpha);
    }
  }


  /**
   * Let the base ImageExporter handle the generic black-box writing routine,
   * then save the image to disk.
   *
   * @param outputName The filename of the output cube
   * @param quality The quality of the output, not used for TIFF
   */
  void QtExporter::write(FileName outputName, int quality) {
    ImageExporter::write(outputName, quality);

    // The return status is wrong for JPEG images, so the code will always
    // continue
    if (!m_qimage->save(outputName.expanded().c_str(), m_format.c_str(),
          quality)) {

      iString err = "Unable to save [" + outputName.expanded() +
        "] to the disk";
      throw IException(IException::Programmer, err, _FILEINFO_);
    }
  }


  /**
   * Checks that the data size for an image of the desired dimensions will be
   * less than 2GB.
   *
   * @param samples Number of samples in the output
   * @param lines Number of lines in the output
   * @param bands Number of bands in the output
   */
  void QtExporter::checkDataSize(BigInt samples, BigInt lines, int bands) {
    // Qt has a 2GB limit on file sizes it can handle
    BigInt maxSize = (BigInt) 2 * 1024 * 1024 * 1024;

    BigInt size = samples * lines * bands;
    if (size >= maxSize) {
      iString gigaBytes = size / (1024.0 * 1024.0 * 1024.0);
      iString msg = "Cube exceeds max size of 2GB. Qimage cannot support ";
      msg += "that much raw data. Your cube is " + gigaBytes + " GB.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }


  /**
   * Returns true if the format is supported by QImageWriter.
   *
   * @param format Lowercase format abbreviation
   *
   * @return True if supported in Qt, false otherwise
   */
  bool QtExporter::canWriteFormat(iString format) {
    bool supported = false;
    QList<QByteArray> list = QImageWriter::supportedImageFormats();
    QList<QByteArray>::Iterator it = list.begin();
    while (it != list.end() && !supported) {
      if (*it == QString(format.c_str())) supported = true;
      it++;
    }
    return supported;
  }
};

