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
  class FileName;
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
   *   @history 2012-04-04 Travis Addair - Added documentation.
   *   @history 2012-08-28 Steven Lambright - The world file should no longer
   *                           overwrite the output image. This is related to #832/#970.
   *                           References #579.
   */
  class ImageExporter {
    public:
      ImageExporter();
      virtual ~ImageExporter();

      void operator()(vector<Buffer *> &in) const;

      virtual void write(FileName outputName, int quality=100);

      int samples() const;
      int lines() const;
      int bands() const;

      double getInputMinimum(int channel) const;
      double getInputMaximum(int channel) const;

      void setOutputRange(double min, double max);

      /**
       * Pure virtual method for setting up an export to a grayscale image.
       *
       * @param desc The description describing the export parameters
       */
      virtual void setGrayscale(ExportDescription &desc) = 0;

      /**
       * Pure virtual method for setting up an export to an RGB image.
       *
       * @param desc The description describing the export parameters
       */
      virtual void setRgb(ExportDescription &desc) = 0;

      /**
       * Pure virtual method for setting up an export to an RGBA image.
       *
       * @param desc The description describing the export parameters
       */
      virtual void setRgba(ExportDescription &desc) = 0; 

      static ImageExporter * fromFormat(iString format);

    protected:
      //! Friendly alias for a method used to write a particular color channel.
      typedef void (ImageExporter::*WriteChannels)(vector<Buffer *> &in) const;

      void setExtension(iString extension);
      Cube * setInput(ExportDescription &desc);

      ProcessExport & getProcess() const;
      virtual int getPixel(double dn) const;

      /**
       * Pure virtual method for writing a line of grayscale data to the output
       * image.
       *
       * @param in Vector containing a single grayscale input line
       */
      virtual void writeGrayscale(vector<Buffer *> &in) const = 0;

      /**
       * Pure virtual method for writing a line of RGB data to the output image.
       *
       * @param in Vector containing three input lines (red, green, blue)
       */
      virtual void writeRgb(vector<Buffer *> &in) const = 0;

      /**
       * Pure virtual method for writing a line of RGBA data to the output
       * image.
       *
       * @param in Vector containing four input lines (red, green, blue, alpha)
       */
      virtual void writeRgba(vector<Buffer *> &in) const = 0;

    private:
      Cube * addChannel(ExportDescription &desc, int i);
      void createWorldFile(FileName outputName);

    private:
      //! The object that feeds lines to this instance and handles stretching.
      ProcessExport *m_process;

      //! Method pointer to one of the pure virtual write methods.
      WriteChannels m_writeMethod;

      //! Extension to append to the output image if not already provided.
      iString m_extension;

      //! Extension to append to the output world file.
      iString m_world;

      //! Number of samples (columns) in the output image.
      int m_samples;

      //! Number of lines (rows) in the output image.
      int m_lines;

      //! Number of bands (channels) in the output image.
      int m_bands;

      //! Input DN floor, all DNs less than this will be mapped to this.
      double m_dataMin;

      //! Input DN ceiling, all DNs greater than this will be mapped to this.
      double m_dataMax;
  };
};


#endif
