#ifndef ImageImporter_h
#define ImageImporter_h

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

namespace Isis {
  class Buffer;
  class Cube;
  class CubeAttributeOutput;
  class FileName;
  class ImageImporter;

  /**
   * @brief Imports images with standard formats into Isis as cubes.
   *
   * Abstract base class for a series of image importers.  Each importer handles
   * a specialized suite of standard image formats, and can be used as a
   * black-box with the import() method, or for finer control, the importer can
   * act as a functor in a ProcessByLine routine.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2012-03-16 Travis Addair
   *
   * @internal
   *   @history 2012-03-28 Travis Addair - Added documentation.
   *
   */
  class ImageImporter {
    public:
      ImageImporter(FileName inputName);
      virtual ~ImageImporter();

      void operator()(Buffer &out) const;

      Cube * import(FileName outputName);
      Cube * import(FileName outputName, CubeAttributeOutput &att);

      void setNullRange(double min, double max);
      void setLrsRange(double min, double max);
      void setHrsRange(double min, double max);

      void setSamples(int s);
      void setLines(int l);
      void setBands(int b);

      int samples() const;
      int lines() const;
      int bands() const;

      FileName filename() const;

      /**
       * Pure virtual method for returning true if the image is grayscale.
       *
       * @return True if the image is grayscale, false otherwise
       */
      virtual bool isGrayscale() const = 0;

      /**
       * Pure virtual method for returning true if the image is RGB (no alpha).
       *
       * @return True if the image is RGB, false otherwise
       */
      virtual bool isRgb() const = 0;

      /**
       * Pure virtual method for returning true if the image is RGBA.
       *
       * @return True if the image is RGBA, false otherwise
       */
      virtual bool isArgb() const = 0;

      static ImageImporter * fromFileName(FileName inputName);

    protected:
      //! Friendly alias for a method used to get a particular color channel.
      typedef int (ImageImporter::*GetChannelMethod)(int pixel) const;

      Cube * createOutput(FileName outputName, CubeAttributeOutput &att);

      void setDefaultBands();
      double testSpecial(double pixel) const;

      virtual GetChannelMethod getBandChannel(int band) const;
      virtual int convertRgbToGray(int pixel) const;

      /**
       * Pure virtual method that updates the buffer used to store chunks of the
       * input data at a time.  Does nothing for classes that read the entire
       * input image into memory.
       *
       * @param line Current line of the output buffer
       * @param band Current band of the output buffer
       */
      virtual void updateRawBuffer(int line, int band) const = 0;

      /**
       * Pure virtual method that returns a representation of a pixel for the
       * input format that can then be broken down into specific gray or RGB/A
       * components.
       *
       * @param s The sample of the desired pixel
       * @param l The line of the desired pixel
       *
       * @return The format-specific pixel representation
       */
      virtual int getPixel(int s, int l) const = 0;

      /**
       * Pure virtual method for retrieving the gray component of the given
       * pixel.
       *
       * @param pixel Representation of a pixel for the input format
       *
       * @return The gray component
       */
      virtual int getGray(int pixel) const = 0;

      /**
       * Pure virtual method for retrieving the red component of the given
       * pixel.
       *
       * @param pixel Representation of a pixel for the input format
       *
       * @return The red component
       */
      virtual int getRed(int pixel) const = 0;

      /**
       * Pure virtual method for retrieving the green component of the given
       * pixel.
       *
       * @param pixel Representation of a pixel for the input format
       *
       * @return The green component
       */
      virtual int getGreen(int pixel) const = 0;

      /**
       * Pure virtual method for retrieving the blue component of the given
       * pixel.
       *
       * @param pixel Representation of a pixel for the input format
       *
       * @return The blue component
       */
      virtual int getBlue(int pixel) const = 0;

      /**
       * Pure virtual method for retrieving the alpha component of the given
       * pixel.
       *
       * @param pixel Representation of a pixel for the input format
       *
       * @return The alpha component
       */
      virtual int getAlpha(int pixel) const = 0;

    private:
      //! The filename of the input image.
      FileName *m_inputName;

      //! The owned handle on the output cube to be imported to.
      Cube *m_outCube;

      //! The number of samples to be written to the output.
      int m_samples;

      //! The number of lines to be written to the output.
      int m_lines;

      //! The number of bands to be written to the output.
      int m_bands;

      //! The lower bound of the range within which input DNs will be made Null.
      double m_nullMin;

      //! The upper bound of the range within which input DNs will be made Null.
      double m_nullMax;

      //! The lower bound of the range within which input DNs will be made LRS.
      double m_lrsMin;

      //! The upper bound of the range within which input DNs will be made LRS.
      double m_lrsMax;

      //! The lower bound of the range within which input DNs will be made HRS.
      double m_hrsMin;

      //! The upper bound of the range within which input DNs will be made HRS.
      double m_hrsMax;
  };
};


#endif
