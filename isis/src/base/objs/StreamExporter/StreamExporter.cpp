/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "StreamExporter.h"

#include "Buffer.h"
#include "ExportDescription.h"
#include "ProcessExport.h"

using namespace Isis;
using namespace std;


namespace Isis {
  /**
   * Construct the stream exporter.
   */
  StreamExporter::StreamExporter() : ImageExporter() {
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
   * Generic initialization with the export description.  Set the input, set
   * the pixel type, and create the buffer.
   *
   * @param desc Export description containing necessary channel information
   */
  void StreamExporter::initialize(ExportDescription &desc) {
    ImageExporter::initialize(desc);
    createBuffer();
  }


  /**
   * Write a line of grayscale data to the output image.
   *
   * @param in Vector containing a single grayscale input line
   */
  void StreamExporter::writeGrayscale(vector<Buffer *> &in) const {
    Buffer &grayLine = *in[0];

    int lineIndex = grayLine.Line() - 1;
    for (int sampleIndex = 0; sampleIndex < grayLine.SampleDimension(); sampleIndex++) {
      int pixelValue = outputPixelValue(grayLine[sampleIndex]);
      setBuffer(sampleIndex, 0, pixelValue);
    }

    writeLine(lineIndex);
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
      int red = outputPixelValue(redLine[i]);
      int green = outputPixelValue(greenLine[i]);
      int blue = outputPixelValue(blueLine[i]);

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
      int red = outputPixelValue(redLine[i]);
      int green = outputPixelValue(greenLine[i]);
      int blue = outputPixelValue(blueLine[i]);
      int alpha = outputPixelValue(alphaLine[i]);

      setBuffer(i, 0, red);
      setBuffer(i, 1, green);
      setBuffer(i, 2, blue);
      setBuffer(i, 3, alpha);
    }

    writeLine(redLine.Line() - 1);
  }
};

