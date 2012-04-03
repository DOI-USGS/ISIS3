#include "QtExporter.h"

#include <QImage>
#include <QImageWriter>

#include "Buffer.h"
#include "ExportDescription.h"
#include "Filename.h"
#include "IException.h"

using namespace Isis;


namespace Isis {
  /**
   * Construct the importer.
   *
   * @param inputName The name of the input image
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
   * Destruct the importer.
   */
  QtExporter::~QtExporter() {
    delete m_qimage;
    m_qimage = NULL;
  }


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


  void QtExporter::setRgb(ExportDescription &desc) {
    Cube *cube = setInput(desc);
    checkDataSize(cube->getSampleCount(), cube->getLineCount(), 3);
    m_qimage = new QImage(
        cube->getSampleCount(), cube->getLineCount(), QImage::Format_RGB32);
  }


  void QtExporter::setRgba(ExportDescription &desc) {
    Cube *cube = setInput(desc);
    checkDataSize(cube->getSampleCount(), cube->getLineCount(), 4);
    m_qimage = new QImage(
        cube->getSampleCount(), cube->getLineCount(), QImage::Format_ARGB32);
  }


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


  void QtExporter::write(Filename outputName, int quality) {
    ImageExporter::write(outputName, quality);

    // The return status is wrong for JPEG images, so the code will always
    // continue
    if (!m_qimage->save(outputName.Expanded().c_str(), m_format.c_str(),
          quality)) {

      iString err = "Unable to save [" + outputName.Expanded() +
        "] to the disk";
      throw IException(IException::Programmer, err, _FILEINFO_);
    }
  }


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

