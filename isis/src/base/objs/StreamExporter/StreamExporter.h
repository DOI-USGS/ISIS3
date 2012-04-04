#ifndef StreamExporter_h
#define StreamExporter_h

/**
 * @file
 * $Revision: 1.17 $
 * $Date: 2010/03/22 19:44:53 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "ImageExporter.h"
#include "PixelType.h"

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
   *
   */
  class StreamExporter : public ImageExporter {
    public:
      StreamExporter();
      virtual ~StreamExporter();

      virtual void setGrayscale(ExportDescription &desc);
      virtual void setRgb(ExportDescription &desc);
      virtual void setRgba(ExportDescription &desc);

      void setType(ExportDescription &desc);

    protected:
      virtual void initialize(ExportDescription &desc);

      PixelType getPixelType() const;

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

    private:
      //! Pixel type to export the data to.
      PixelType m_type;
  };
};


#endif
