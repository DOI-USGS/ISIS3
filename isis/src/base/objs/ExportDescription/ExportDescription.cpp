/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "ExportDescription.h"

#include <float.h>
#include <QList>

#include "CubeAttribute.h"
#include "FileName.h"
#include "IString.h"
#include "IException.h"
#include "PixelType.h"

namespace Isis {
  /**
   * Construct the export description.
   */
  ExportDescription::ExportDescription() {
    m_channels = NULL;
    m_channels = new QList<ChannelDescription *>;

    m_type = Isis::None;

    m_outputPixelNull = 0.0;
    m_outputPixelValidMin = 0.0;
    m_outputPixelValidMax = 255.0;
    m_outputPixelAbsoluteMin = 0.0;
    m_outputPixelAbsoluteMax = 255.0;
  }


  /**
   * Copy constructor for the export description. 
   * @param descriptionToCopy ExportDescription to be copied.
   */
  ExportDescription::ExportDescription(const ExportDescription &descriptionToCopy) {
    m_channels = NULL;

    if(descriptionToCopy.m_channels) {
      m_channels = new QList<ChannelDescription *>();
      for (int i = 0; i < descriptionToCopy.channelCount(); i++) {
        const ExportDescription::ChannelDescription &channel = descriptionToCopy.channel(i);
        CubeAttributeInput att = channel.attributes();
        if (channel.hasCustomRange()) {
          addChannel(channel.filename(), att, channel.inputMinimum(), channel.inputMaximum());
        }
        else {
          addChannel(channel.filename(), att);
        }
      }
    }

    setPixelType(descriptionToCopy.m_type);
  }

  /**
   * Assignment operator for the export description.
   * @param descriptionToCopy ExportDescription to be copied. 
   * @return ExportDescription with same number of channels, type, and pixel 
   *         values.
   */
  ExportDescription &ExportDescription::operator=(const ExportDescription &descriptionToCopy) {
    if (m_channels) {
      delete m_channels;
      m_channels = NULL;
    }

    if(descriptionToCopy.m_channels) {
      m_channels = new QList<ChannelDescription *>();
      for (int i = 0; i < descriptionToCopy.channelCount(); i++) {
        const ExportDescription::ChannelDescription &channel = descriptionToCopy.channel(i);
        CubeAttributeInput att = channel.attributes();
        if (channel.hasCustomRange()) {
          addChannel(channel.filename(), att, channel.inputMinimum(), channel.inputMaximum());
        }
        else {
          addChannel(channel.filename(), att);
        }
      }
    }

    setPixelType(descriptionToCopy.m_type);
    return *this;
  }

  /**
   * Destruct the export description.  Also deletes the list of channels.
   */
  ExportDescription::~ExportDescription() {
    for (int i = 0; i < m_channels->size(); i++) delete (*m_channels)[i];
    delete m_channels;
    m_channels = NULL;
  }


  /**
   * Set the pixel type for the output image.  Automatically establishes valid
   * output minimum, maximum, and null values based on the type.
   *
   * @param type The pixel type: {UnsignedByte, SignedWord, UnsignedWord}
   */
  void ExportDescription::setPixelType(PixelType type) {
    m_type = type;
    switch (type) {
      case UnsignedByte:
        m_outputPixelNull = 0.0;
        m_outputPixelValidMin = 1.0;
        m_outputPixelValidMax = 255.0;
        break;
      case SignedWord:
        m_outputPixelNull = -32768.0;
        m_outputPixelValidMin = -32767.0; // Changed from -32752.0 since this variable represents 
                                          // the smallest valid exported pixel value, not our 
                                          // special pixel min valid value
        m_outputPixelValidMax = 32767.0;
        break;
      case UnsignedWord:
        m_outputPixelNull = 0.0;
        m_outputPixelValidMin = 1.0; // Changed from 3.0 since this variable is used to set the 
                                     // smallest valid exported pixel value, not our special pixel 
                                     // min valid value
        m_outputPixelValidMax = 65535.0; // Changed from 65522.0 since this variable represents the 
                                         // largest valid exported pixel value, not our special 
                                         // pixel max valid value
        break;
      default:
        throw IException(IException::Programmer,
            "Invalid export pixel type [" + toString(type) + "]",
            _FILEINFO_);
    }
    // in each case above, the smallest possible output pixel value is the null value
    m_outputPixelAbsoluteMin = m_outputPixelNull;
    // in each case above, the largest possible output pixel value is the maximum value
    m_outputPixelAbsoluteMax = m_outputPixelValidMax;

  }


  /**
   * Returns the pixel type. Defaults to None if not set by the user.
   *
   * @return The pixel type: {None, UnsignedByte, SignedWord, UnsignedWord}
   */
  PixelType ExportDescription::pixelType() const {
    return m_type;
  }


  /**
   * Returns the output pixel value for Null DNs. Defaults to 0.0 if not set by the user. 
   *
   * @return The output Null
   */
  double ExportDescription::outputPixelNull() const {
    return m_outputPixelNull;
  }


  /**
   * Returns the output pixel value for the valid minimum. Defaults to 0.0 if not set by the user. 
   *
   * @return The valid minimum
   */
  double ExportDescription::outputPixelValidMin() const {
    return m_outputPixelValidMin;
  }


  /**
   * Returns the output pixel value for the valid maximum. Defaults to 255.0 if not set by the user. 
   *
   * @return The valid maximum
   */
  double ExportDescription::outputPixelValidMax() const {
    return m_outputPixelValidMax;
  }


  /**
   * Returns the absolute minimum value for output pixels. Defaults to 0.0 if not set by the user. 
   *
   * @return The absolute minimum
   */
  double ExportDescription::outputPixelAbsoluteMin() const {
    return m_outputPixelAbsoluteMin;
  }


  /**
   * Returns the absolute maximum value for output pixels. Defaults to 255.0 if not set by the user. 
   *
   * @return The absolute maximum
   */
  double ExportDescription::outputPixelAbsoluteMax() const {
    return m_outputPixelAbsoluteMax;
  }


  /**
   * Add an export color channel for the given input cube and attributes
   * (typically band number).
   *
   * @param filename The name of the cube
   * @param att The cube attributes such as band number
   *
   * @return The index of the newly added channel
   */
  int ExportDescription::addChannel(FileName filename, CubeAttributeInput &att) {

    m_channels->append(new ChannelDescription(filename, att));
    return m_channels->size() - 1;
  }


  /**
   * Add an export color channel for the given input cube, attributes
   * (typically band number), and input DN range.
   *
   * @param filename The name of the cube
   * @param att The cube attributes such as band number
   * @param min The minimum DN in the input mapped to the minimum of the output
   * @param max The maximum DN in the input mapped to the maximum of the output
   *
   * @return The index of the newly added channel
   */
  int ExportDescription::addChannel(FileName filename, CubeAttributeInput &att, 
                                    double min, double max) {

    ChannelDescription *desc = new ChannelDescription(filename, att);
    desc->setInputRange(min, max);
    m_channels->append(desc);
    return m_channels->size() - 1;
  }


  /**
   * Return the channels description at the given index.
   *
   * @param i Index of the desired channel
   *
   * @return Constant reference to the channel
   */
  const ExportDescription::ChannelDescription &ExportDescription::channel(int i) const {

    return *(*m_channels)[i];
  }


  /**
   * Count of the number of channels in the description.
   *
   * @return The channel count
   */
  int ExportDescription::channelCount() const {
    return m_channels->size();
  }


  /**
   * Construct the channel description with the given input name and attributes.
   *
   * @param filename The name of the cube
   * @param att The cube attributes such as band number
   */
  ExportDescription::ChannelDescription::ChannelDescription(FileName &filename, 
                                                            CubeAttributeInput &att) {

    m_filename = filename;
    m_att = att;

    m_customRange = false;
    m_inputMin = DBL_MIN;
    m_inputMax = DBL_MAX;
  }


  /**
   * Returns a copy of the filename associated with this channel.
   *
   * @return The name of the cube
   */
  FileName ExportDescription::ChannelDescription::filename() const {
    return m_filename;
  }


  /**
   * Returns a copy of the input attributes associated with this channel.
   *
   * @return The cube attributes such as band number
   */
  CubeAttributeInput ExportDescription::ChannelDescription::attributes() const {
    return m_att;
  }


  /**
   * Sets the input range for this channel.  Any value of min in the input cube
   * will be mapped to the output min of the output channel.  Similar for the
   * max.
   *
   * @param min The minimum DN in the input mapped to the minimum of the output
   * @param max The maximum DN in the input mapped to the maximum of the output
   */
  void ExportDescription::ChannelDescription::setInputRange(double min, double max) {

    m_inputMin = min;
    m_inputMax = max;
    m_customRange = true;
  }


  /**
   * Returns the input minimum for this channel.  Any value of min in the input
   * cube will be mapped to the output min of the output channel.
   *
   * @return The minimum DN in the input mapped to the minimum of the output
   */
  double ExportDescription::ChannelDescription::inputMinimum() const {
    return m_inputMin;
  }


  /**
   * Returns the input maximum for this channel.  Any value of max in the input
   * cube will be mapped to the output max of the output channel.
   *
   * @return The maximum DN in the input mapped to the maximum of the output
   */
  double ExportDescription::ChannelDescription::inputMaximum() const {
    return m_inputMax;
  }


  /**
   * Returns true if the user of this instance has set a custom input range for
   * this channel.
   *
   * @return True if a custom range has been set, false otherwise
   */
  bool ExportDescription::ChannelDescription::hasCustomRange() const {
    return m_customRange;
  }
};

