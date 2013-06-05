#ifndef ExportDescription_h
#define ExportDescription_h

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

#include "CubeAttribute.h"
#include "FileName.h"
#include "PixelType.h"

namespace Isis {
  /**
   * @brief Describes how a series of cubes should be exported
   *
   * This container class encapsulates the parameters required to specify how an
   * Isis cube should be exported to a standard image format.  This
   * encapsulation is useful to applications like "isis2std" for passing its
   * user parameters down to processing classes such as ImageExporters without
   * needing to create new method signatures that take a growing list of
   * parameters.  The ExportDescription contains top-level data about the export
   * as a whole, such as bit type.  It also contains a list of
   * ChannelDescriptions, each describing the input cubes, attributes, and DN
   * ranges for a channel of color information (gray, red, blue, green, or
   * alpha).
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2012-04-03 Travis Addair
   *
   * @internal
   *   @history 2012-04-04 Travis Addair - Added documentation.
   *   @history 2013-06-05 Jeannie Backer - Modified valid min/max for signed and unsigned word
   *                           pixel types since the exported images do not care about Isis special
   *                           pixel DNs. Change ouput null/min/max member variable names for
   *                           clarity. Added absolute min/max member variables and accessors.
   *                           Added copy constructor and assignment operator. References #1380.
   *
   */
  class ExportDescription {
    public:
      /**
       * @brief Describes how a cube as a single color channel to be exported
       *
       * Specifies the filename, attributes, and input DN range of a cube to be
       * exported as a color channel in a standard image.
       *
       * @ingroup HighLevelCubeIO
       *
       * @author 2012-04-03 Travis Addair
       *
       * @internal
       *
       */
      class ChannelDescription {
        public:
          ChannelDescription(FileName &filename, CubeAttributeInput &att);

          //! Destruct the export description.
          virtual ~ChannelDescription() {};

          FileName filename() const;
          CubeAttributeInput attributes() const;

          void setInputRange(double min, double max);
          double inputMinimum() const;
          double inputMaximum() const;
          bool hasCustomRange() const;

        private:
          //! Input filename of the cube to be treated as a color channel.
          FileName m_filename;

          //! Attributes like which band to use from the input cube.
          CubeAttributeInput m_att;

          //! Whether or not the user has specified a custom input DN range.
          bool m_customRange;

          //! Minimum DN in the input, defaults to DBL_MIN.
          double m_inputMin;

          //! Maximum DN in the input, defaults to DBL_MAX.
          double m_inputMax;
      };

    public:
      ExportDescription();
      ExportDescription(const ExportDescription &descriptionToCopy);
      virtual ~ExportDescription();
      ExportDescription &operator=(const ExportDescription &descriptionToCopy);

      void setPixelType(PixelType type);
      PixelType pixelType() const;
      double outputPixelNull() const;
      double outputPixelValidMin() const;
      double outputPixelValidMax() const;
      double outputPixelAbsoluteMin() const;
      double outputPixelAbsoluteMax() const;

      int addChannel(FileName filename, CubeAttributeInput &att);
      int addChannel(FileName filename, CubeAttributeInput &att, double min, double max);
      const ChannelDescription &channel(int i) const;
      int channelCount() const;

    private:
      //! Pixel type to export the data to, defaults to None.
      PixelType m_type;

      double m_outputPixelNull;        /**< Value to which Null DNs will be mapped in the exported
                                            image file, defaults to 0.0. */
      double m_outputPixelValidMin;    /**< Value to which minimum valid DNs will be mapped in the
                                            exported image file, defaults to 0.0. */
      double m_outputPixelValidMax;    /**< Value to which maximum valid DNs will be mapped in the 
                                            exported image file, defaults to 255.0. */
      double m_outputPixelAbsoluteMin; /**< The smallest allowed pixel value in the exported image
                                            file. This is the same as the value to which Null DNs
                                            are mapped. */
      double m_outputPixelAbsoluteMax; /**< The largest allowed pixel value in the exported image
                                            file. This is the same as the value to which maximum
                                            DNs are mapped. */

      //! List of color channels to be exported into the output image.
      QList<ChannelDescription *> *m_channels;
  };
};


#endif
