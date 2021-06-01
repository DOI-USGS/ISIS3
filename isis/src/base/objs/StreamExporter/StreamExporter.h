#ifndef StreamExporter_h
#define StreamExporter_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ImageExporter.h"

namespace Isis {
  /**
   * @brief Exports cubes into a standard format in incremental pieces
   *
   * Abstract base class for a series of stream image exporters.  Stream
   * exporters are specialized in that they write out data as a stream of lines
   * as opposed to keeping the export data all in memory.  In this way, they can
   * be run on arbitrarily large images.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2012-04-03 Travis Addair
   *
   * @internal
   *   @history 2012-04-04 Travis Addair - Added documentation.
   *   @history 2013-06-05 Jeannie Backer - Removed setType() and pixelType() methods and pixel
   *                           type member variable since these are now handled by ImageExporter.
   *                           Changed ImageExporter calls to new method names, where needed.
   *                           References #1380.
   *
   */
  class StreamExporter : public ImageExporter {
    public:
      StreamExporter();
      virtual ~StreamExporter();

      virtual void setGrayscale(ExportDescription &desc);
      virtual void setRgb(ExportDescription &desc);
      virtual void setRgba(ExportDescription &desc);

    protected:
      virtual void initialize(ExportDescription &desc);

      virtual void writeGrayscale(vector<Buffer *> &in) const;
      virtual void writeRgb(vector<Buffer *> &in) const;
      virtual void writeRgba(vector<Buffer *> &in) const;

      /**
       * Pure virtual method for creating the buffer to store a chunk of
       * streamed line data with one or more bands.
       */
      virtual void createBuffer() = 0;

      /**
       * Pure virtual method for setting a particular index of the line buffer
       * to the given DN.
       *
       * @param s The sample index into the buffer
       * @param b The band index into the buffer
       * @param dn The value to set at the given index
       */
      virtual void setBuffer(int s, int b, int dn) const = 0; 

      /**
       * Pure virtual method for writing a line of buffered data to the output
       * image on disk.
       *
       * @param l The line of the output image to write to
       */
      virtual void writeLine(int l) const = 0;

  };
};


#endif
