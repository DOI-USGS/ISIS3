#include "ImageExporter.h"

#include "Buffer.h"
#include "CubeAttribute.h"
#include "ExportDescription.h"
#include "Filename.h"
#include "JP2Exporter.h"
#include "ProcessExport.h"
#include "QtExporter.h"
#include "TiffExporter.h"

using namespace Isis;


namespace Isis {
  /**
   * Construct the importer.
   *
   * @param inputName The name of the input image
   */
  ImageExporter::ImageExporter() {
    m_process = NULL;
    m_process = new ProcessExport;

    m_writeMethod = NULL;

    m_extension = "";
    m_world = "";

    m_samples = 0;
    m_lines = 0;
    m_bands = 0;

    m_dataMin = 0.0;
    m_dataMax = 255.0;
  }


  /**
   * Destruct the importer.  Also deletes the output cube handle.
   */
  ImageExporter::~ImageExporter() {
    delete m_process;
    m_process = NULL;
  }


  void ImageExporter::operator()(vector<Buffer *> &in) const {
    (this->*m_writeMethod)(in);
  }


  void ImageExporter::write(Filename outputName, int quality) {
    ProcessExport &p = getProcess();
    if (!p.HasInputRange()) p.SetInputRange();
    p.ProcessCubes(*this);

    outputName.AddExtension(m_extension);
    createWorldFile(outputName);
  }


  void ImageExporter::setExtension(iString extension) {
    m_extension = extension;

    // World file extension is the first and last characters of the extension
    // with an added 'w' at the end
    int last = extension.length() - 1;
    m_world = extension.substr(0, 1) + extension.substr(last) + "w";
  }


  Cube * ImageExporter::setInput(ExportDescription &desc) {
    switch (desc.channelCount()) {
      case 1:
        m_writeMethod = &ImageExporter::writeGrayscale;
        break;
      case 3:
        m_writeMethod = &ImageExporter::writeRgb;
        break;
      case 4:
        m_writeMethod = &ImageExporter::writeRgba;
        break;
      default:
        throw IException(IException::Programmer,
            "Cannot export an image with [" + iString(desc.channelCount()) +
            "] channels",
            _FILEINFO_);
    }

    ProcessExport &p = getProcess();
    Cube *cube = addChannel(desc, 0);
    m_samples = cube->getSampleCount();
    m_lines = cube->getLineCount();
    m_bands = desc.channelCount();

    for (int i = 1; i < desc.channelCount(); i++) addChannel(desc, i);

    p.SetOutputRange(1.0, 255.0);
    p.SetOutputNull(0.0);
    return cube;
  }


  Cube * ImageExporter::addChannel(ExportDescription &desc, int i) {
    ProcessExport &p = getProcess();

    const ExportDescription::ChannelDescription &channel = desc.getChannel(i);
    Cube *cube = p.SetInputCube(
        channel.filename().Expanded(), channel.attributes(), Isis::OneBand);

    if (channel.hasCustomRange())
      p.SetInputRange(channel.getInputMinimum(), channel.getInputMaximum(), i);

    return cube;
  }


  ProcessExport & ImageExporter::getProcess() const {
    return *m_process;
  }


  int ImageExporter::samples() const {
    return m_samples;
  }


  int ImageExporter::lines() const {
    return m_lines;
  }


  int ImageExporter::bands() const {
    return m_bands;
  }


  double ImageExporter::getInputMinimum(int channel) const {
    return m_process->GetInputMinimum(channel);
  }


  double ImageExporter::getInputMaximum(int channel) const {
    return m_process->GetInputMaximum(channel);
  }


  void ImageExporter::setOutputRange(double min, double max) {
    m_dataMin = min;
    m_dataMax = max;
  }


  int ImageExporter::getPixel(double dn) const {
    return (dn < m_dataMin) ? m_dataMin : (dn > m_dataMax) ? m_dataMax : dn;
  }


  void ImageExporter::createWorldFile(Filename outputName) {
    // Create a world file if it has a map projection
    outputName.RemoveExtension();
    outputName.AddExtension(m_world);

    ProcessExport &p = getProcess();
    p.CreateWorldFile(outputName.Expanded());
    p.EndProcess();
  }


  ImageExporter * ImageExporter::fromFormat(iString format) {
    ImageExporter *exporter = NULL;

    format.DownCase();
    if (TiffExporter::canWriteFormat(format)) {
      exporter = new TiffExporter();
    }
    else if (QtExporter::canWriteFormat(format)) {
      exporter = new QtExporter(format);
    }
    else if (JP2Exporter::canWriteFormat(format)) {
      exporter = new JP2Exporter();
    }
    else {
      throw IException(IException::Programmer,
          "Cannot export image as format [" + format + "]",
          _FILEINFO_);
    }

    return exporter;
  }
};

