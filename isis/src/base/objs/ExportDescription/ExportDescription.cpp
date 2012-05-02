#include "ExportDescription.h"

#include <float.h>

#include <QList>

using namespace Isis;


namespace Isis {
  /**
   * Construct the export description.
   */
  ExportDescription::ExportDescription() {
    m_channels = NULL;
    m_channels = new QList<ChannelDescription *>;

    m_type = Isis::None;
    m_outputMin = 0.0;
    m_outputMax = 255.0;
    m_outputNull = 0.0;
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
        m_outputMin = 1.0;
        m_outputMax = 255.0;
        break;
      case SignedWord:
        m_outputMin = -32752.0;
        m_outputMax = 32767.0;
        m_outputNull = -32768.0;
        break;
      case UnsignedWord:
        m_outputMin = 3.0;
        m_outputMax = 65522.0;
        break;
      default:
        throw IException(IException::Programmer,
            "Invalid export pixel type [" + iString(type) + "]",
            _FILEINFO_);
    }
  }


  /**
   * Returns the pixel type.  Defaults to None if not set by the user.
   *
   * @return The pixel type: {None, UnsignedByte, SignedWord, UnsignedWord}
   */
  PixelType ExportDescription::getPixelType() const {
    return m_type;
  }


  /**
   * Returns the output minimum.  Defaults to 0.0 if not set by the user.
   *
   * @return The output minimum
   */
  double ExportDescription::getOutputMinimum() const {
    return m_outputMin;
  }


  /**
   * Returns the output maximum.  Defaults to 255.0 if not set by the user.
   *
   * @return The output maximum
   */
  double ExportDescription::getOutputMaximum() const {
    return m_outputMax;
  }


  /**
   * Returns the output value for Null pixels.  Defaults to 0.0 if not set by
   * the user.
   *
   * @return The output value for Nulls
   */
  double ExportDescription::getOutputNull() const {
    return m_outputNull;
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
  int ExportDescription::addChannel(FileName filename,
      CubeAttributeInput &att) {

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
  int ExportDescription::addChannel(
      FileName filename, CubeAttributeInput &att, double min, double max) {

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
  const ExportDescription::ChannelDescription &
      ExportDescription::getChannel(int i) const {

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
  ExportDescription::ChannelDescription::ChannelDescription(
      FileName &filename, CubeAttributeInput &att) {

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
  void ExportDescription::ChannelDescription::setInputRange(
      double min, double max) {

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
  double ExportDescription::ChannelDescription::getInputMinimum() const {
    return m_inputMin;
  }


  /**
   * Returns the input maximum for this channel.  Any value of max in the input
   * cube will be mapped to the output max of the output channel.
   *
   * @return The maximum DN in the input mapped to the maximum of the output
   */
  double ExportDescription::ChannelDescription::getInputMaximum() const {
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

