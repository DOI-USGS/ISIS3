#include "StreamExporter.h"

#include "Buffer.h"
#include "ExportDescription.h"
#include "ProcessExport.h"

using namespace Isis;


namespace Isis {
  /**
   * Construct the importer.
   *
   * @param inputName The name of the input image
   */
  StreamExporter::StreamExporter() : ImageExporter() {
    m_type = Isis::None;
  }


  /**
   * Destruct the importer.
   */
  StreamExporter::~StreamExporter() {
  }


  void StreamExporter::setGrayscale(ExportDescription &desc) {
    initialize(desc);
  }


  void StreamExporter::setRgb(ExportDescription &desc) {
    initialize(desc);
  }


  void StreamExporter::setRgba(ExportDescription &desc) {
    initialize(desc);
  }


  void StreamExporter::initialize(ExportDescription &desc) {
    setInput(desc);
    setType(desc);
  }


  void StreamExporter::setType(ExportDescription &desc) {
    ProcessExport &p = getProcess();
    p.SetFormat(ProcessExport::BIL);

    m_type = desc.getPixelType();
    createBuffer();

    p.SetOutputType(desc.getPixelType());
    p.SetOutputNull(desc.getOutputNull());
    p.SetOutputRange(desc.getOutputMinimum(), desc.getOutputMaximum());

    setOutputRange(desc.getOutputMinimum(), desc.getOutputMaximum());
  }


  PixelType StreamExporter::getPixelType() const {
    return m_type;
  }


  void StreamExporter::writeGrayscale(vector<Buffer *> &in) const {
    Buffer &grayLine = *in[0];

    for (int i = 0; i < grayLine.SampleDimension(); i++) {
      int dn = getPixel(grayLine[i]);
      setBuffer(i, 0, dn);
    }

    writeLine(grayLine.Line() - 1);
  }


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

