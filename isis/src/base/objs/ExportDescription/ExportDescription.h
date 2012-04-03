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
#include "Filename.h"
#include "PixelType.h"

namespace Isis {
  /**
   * @brief Describes how a series of cubes should be exported
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2012-04-03 Travis Addair
   *
   * @internal
   *
   */
  class ExportDescription {
    public:
      class ChannelDescription {
        public:
          ChannelDescription(Filename &filename, CubeAttributeInput &att);
          virtual ~ChannelDescription() {};

          Filename filename() const;
          CubeAttributeInput attributes() const;

          void setInputRange(double min, double max);
          double getInputMinimum() const;
          double getInputMaximum() const;
          bool hasCustomRange() const;

        private:
          Filename m_filename;

          CubeAttributeInput m_att;

          bool m_customRange;

          double m_inputMin;

          double m_inputMax;
      };

    public:
      ExportDescription();
      virtual ~ExportDescription();

      void setPixelType(PixelType type);
      PixelType getPixelType() const;
      double getOutputMinimum() const;
      double getOutputMaximum() const;
      double getOutputNull() const;

      void addChannel(Filename filename, CubeAttributeInput &att);
      void addChannel(Filename filename, CubeAttributeInput &att,
          double min, double max);
      const ChannelDescription & getChannel(int i) const;
      const int channelCount() const;

    private:
      PixelType m_type;

      double m_outputMin;

      double m_outputMax;

      double m_outputNull;

      QList<ChannelDescription *> *m_channels;
  };
};


#endif
