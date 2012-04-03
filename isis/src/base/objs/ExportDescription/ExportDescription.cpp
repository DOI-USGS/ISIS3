#include "ExportDescription.h"

#include <float.h>

#include <QList>

using namespace Isis;


namespace Isis {
  /**
   * Construct the importer.
   *
   * @param inputName The name of the input image
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
   * Destruct the importer.  Also deletes the output cube handle.
   */
  ExportDescription::~ExportDescription() {
    for (int i = 0; i < m_channels->size(); i++) delete (*m_channels)[i];
    delete m_channels;
    m_channels = NULL;
  }


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


  PixelType ExportDescription::getPixelType() const {
    return m_type;
  }


  double ExportDescription::getOutputMinimum() const {
    return m_outputMin;
  }


  double ExportDescription::getOutputMaximum() const {
    return m_outputMax;
  }


  double ExportDescription::getOutputNull() const {
    return m_outputNull;
  }


  void ExportDescription::addChannel(Filename filename,
      CubeAttributeInput &att) {

    m_channels->append(new ChannelDescription(filename, att));
  }


  void ExportDescription::addChannel(
      Filename filename, CubeAttributeInput &att, double min, double max) {

    ChannelDescription *desc = new ChannelDescription(filename, att);
    desc->setInputRange(min, max);
    m_channels->append(desc);
  }


  const ExportDescription::ChannelDescription &
      ExportDescription::getChannel(int i) const {

    return *(*m_channels)[i];
  }


  const int ExportDescription::channelCount() const {
    return m_channels->size();
  }


  ExportDescription::ChannelDescription::ChannelDescription(
      Filename &filename, CubeAttributeInput &att) {

    m_filename = filename;
    m_att = att;

    m_customRange = false;
    m_inputMin = DBL_MIN;
    m_inputMax = DBL_MAX;
  }


  Filename ExportDescription::ChannelDescription::filename() const {
    return m_filename;
  }


  CubeAttributeInput ExportDescription::ChannelDescription::attributes() const {
    return m_att;
  }


  void ExportDescription::ChannelDescription::setInputRange(
      double min, double max) {

    m_inputMin = min;
    m_inputMax = max;
    m_customRange = true;
  }


  double ExportDescription::ChannelDescription::getInputMinimum() const {
    return m_inputMin;
  }


  double ExportDescription::ChannelDescription::getInputMaximum() const {
    return m_inputMax;
  }


  bool ExportDescription::ChannelDescription::hasCustomRange() const {
    return m_customRange;
  }
};

