#include "StreamExporter.h"

#include "Buffer.h"
#include "ExportDescription.h"
#include "ProcessExport.h"

using namespace Isis;


namespace Isis {
  /**
   * Construct the stream exporter.
   */
  StreamExporter::StreamExporter() : ImageExporter() {
    m_type = Isis::None;
  }


  /**
   * Destruct the exporter.
   */
  StreamExporter::~StreamExporter() {
  }


  /**
   * Generic initialization with the export description.  Stream exporters do
   * not do anything special to export a grayscale image beyond setting up the
   * appropriate number of color channels.
   *
   * @param desc Export description containing necessary channel information
   */
  void StreamExporter::setGrayscale(ExportDescription &desc) {
    initialize(desc);
  }


  /**
   * Generic initialization with the export description.  Stream exporters do
   * not do anything special to export an RGB image beyond setting up the
   * appropriate number of color channels.
   *
   * @param desc Export description containing necessary channel information
   */
  void StreamExporter::setRgb(ExportDescription &desc) {
    initialize(desc);
  }


  /**
   * Generic initialization with the export description.  Stream exporters do
   * not do anything special to export an RGBA image beyond setting up the
   * appropriate number of color channels.
   *
   * @param desc Export description containing necessary channel information
   */
  void StreamExporter::setRgba(ExportDescription &desc) {
    initialize(desc);
  }


  /**
   * Generic initialization with the export description.  Set the input and set
   * the pixel type.
   *
   * @param desc Export description containing necessary channel information
   */
  void StreamExporter::initialize(ExportDescription &desc) {
    setInput(desc);
    setType(desc);
  }


  /**
   * Set the pixel type from the description and create the buffer.
   *
   * @param desc Export description containing necessary bit type information
   */
  void StreamExporter::setType(ExportDescription &desc) {
    ProcessExport &p = getProcess();
    p.setFormat(ProcessExport::BIL);

    m_type = desc.getPixelType();
    createBuffer();

    p.SetOutputType(desc.getPixelType());
    p.SetOutputNull(desc.getOutputNull());
    p.SetOutputRange(desc.getOutputMinimum(), desc.getOutputMaximum());

    setOutputRange(desc.getOutputMinimum(), desc.getOutputMaximum());
  }


  /**
   * Returns the pixel type.  Defaults to None if not set by the user.
   *
   * @return The pixel type: {None, UnsignedByte, SignedWord, UnsignedWord}
   */
  PixelType StreamExporter::getPixelType() const {
    return m_type;
  }


  /**
   * Write a line of grayscale data to the output image.
   *
   * @param in Vector containing a single grayscale input line
   */
  void StreamExporter::writeGrayscale(vector<Buffer *> &in) const {
    Buffer &grayLine = *in[0];

    for (int i = 0; i < grayLine.SampleDimension(); i++) {
      int dn = getPixel(grayLine[i]);
      setBuffer(i, 0, dn);
    }

    writeLine(grayLine.Line() - 1);
  }


  /**
   * Write a line of RGB data to the output image.
   *
   * @param in Vector containing three input lines (red, green, blue)
   */
  void StreamExporter::writeRgb(vector<Buffer *> &in) const {
    Buffer &redLine = *in[0];
    Buffer &greenLine = *in[1];
    Buffer &blueLine = *in[2];

    for (int i = 0; i < redLine.SampleDimension(); i++) {
      int red = getPixel(redLine[i]);
      int green = getPixel(greenLine[i]);
      int blue = getPixel(blueLine[i]);

      setBuffer(i, 0, red);
      setBuffer(i, 1, green);
      setBuffer(i, 2, blue);
    }

    writeLine(redLine.Line() - 1);
  }


  /**
   * Write a line of RGBA data to the output image.
   *
   * @param in Vector containing four input lines (red, green, blue, alpha)
   */
  void StreamExporter::writeRgba(vector<Buffer *> &in) const {
    Buffer &redLine = *in[0];
    Buffer &greenLine = *in[1];
    Buffer &blueLine = *in[2];
    Buffer &alphaLine = *in[3];

    for (int i = 0; i < redLine.SampleDimension(); i++) {
      int red = getPixel(redLine[i]);
      int green = getPixel(greenLine[i]);
      int blue = getPixel(blueLine[i]);
      int alpha = getPixel(alphaLine[i]);

      setBuffer(i, 0, red);
      setBuffer(i, 1, green);
      setBuffer(i, 2, blue);
      setBuffer(i, 3, alpha);
    }

    writeLine(redLine.Line() - 1);
  }
};

