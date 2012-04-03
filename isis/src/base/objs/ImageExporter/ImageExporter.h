#ifndef ImageExporter_h
#define ImageExporter_h

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

#include <vector>

#include "iString.h"

using std::vector;

namespace Isis {
  class Buffer;
  class Cube;
  class CubeAttributeOutput;
  class ExportDescription;
  class Filename;
  class ImageExporter;
  class ProcessExport;

  /**
   * @brief Export Isis cubes into standard formats
   *
   * Abstract base class for a series of image exporters.  Each exporter handles
   * a specialized suite of standard image formats, and can be used as a
   * black-box with the write() method, or for finer control, the importer can
   * act as a functor in a ProcessExport routine.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2012-04-03 Travis Addair
   *
   * @internal
   *
   */
  class ImageExporter {
    public:
      ImageExporter();
      virtual ~ImageExporter();

      void operator()(vector<Buffer *> &in) const;

      virtual void write(Filename outputName, int quality=100);

      int samples() const;
      int lines() const;
      int bands() const;

      double getInputMinimum(int channel) const;
      double getInputMaximum(int channel) const;

      virtual void setGrayscale(ExportDescription &desc) = 0;
      virtual void setRgb(ExportDescription &desc) = 0;
      virtual void setRgba(ExportDescription &desc) = 0; 
      void setOutputRange(double min, double max);

      static ImageExporter * fromFormat(iString format);

    protected:
      //! Friendly alias for a method used to write a particular color channel.
      typedef void (ImageExporter::*WriteChannels)(vector<Buffer *> &in) const;

      void setExtension(iString extension);
      Cube * setInput(ExportDescription &desc);

      ProcessExport & getProcess() const;
      virtual int getPixel(double dn) const;

      virtual void writeGrayscale(vector<Buffer *> &in) const = 0;
      virtual void writeRgb(vector<Buffer *> &in) const = 0;
      virtual void writeRgba(vector<Buffer *> &in) const = 0;

    private:
      Cube * addChannel(ExportDescription &desc, int i);
      void createWorldFile(Filename outputName);

    private:
      ProcessExport *m_process;

      WriteChannels m_writeMethod;

      iString m_extension;

      iString m_world;

      int m_samples;

      int m_lines;

      int m_bands;

      double m_dataMin;

      double m_dataMax;
  };
};


#endif
