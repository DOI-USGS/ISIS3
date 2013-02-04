#include "ImageExporter.h"

#include "Buffer.h"
#include "CubeAttribute.h"
#include "ExportDescription.h"
#include "FileName.h"
#include "JP2Exporter.h"
#include "ProcessExport.h"
#include "QtExporter.h"
#include "TiffExporter.h"

using namespace Isis;


namespace Isis {
  /**
   * Construct the exporter.
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
   * Destruct the exporter.  Also deletes the process object.
   */
  ImageExporter::~ImageExporter() {
    delete m_process;
    m_process = NULL;
  }


  /**
   * The method for writing a line of input image data (with potentially several
   * bands representing color channels) to the output image.  It is called for
   * each line of data in the input images (provides a vector containing the
   * same line from each single-band input image).  Enables the exporter to be
   * used as a functor in a custom ProcessExport routine, bypassing the
   * black-box write() method entirely.
   *
   * @param in Vector containing a line of input data from each input channel
   */
  void ImageExporter::operator()(vector<Buffer *> &in) const {
    (this->*m_writeMethod)(in);
  }


  /**
   * Export the Isis cube channels to the given standard image.  If supported by
   * the specific exporter child, will also produce an image with the given
   * scaled quality.  This will do a black-box export using a ProcessExport
   * routine.  After the export is completed, a world file will also be created
   * if a map projection exists.
   *
   * @param outputName The filename of the output cube
   * @param quality The quality of the output from 0 to 100, defaults to 100
   */
  void ImageExporter::write(FileName outputName, int quality) {
    ProcessExport &p = getProcess();
    if (!p.HasInputRange()) p.SetInputRange();
    p.ProcessCubes(*this);

    outputName = outputName.addExtension(m_extension);
    createWorldFile(outputName);
  }


  /**
   * Number of samples (columns) in the output image.
   *
   * @return The width of the output
   */
  int ImageExporter::samples() const {
    return m_samples;
  }


  /**
   * Number of lines (rows) in the output image.
   *
   * @return The height of the output
   */
  int ImageExporter::lines() const {
    return m_lines;
  }


  /**
   * Number of bands (channels) in the output image.
   *
   * @return The depth of the output
   */
  int ImageExporter::bands() const {
    return m_bands;
  }


  /**
   * Returns the input minimum for the given channel.  Any value of min in the
   * input cube will be mapped to the output min of the output channel.
   *
   * @param channel Index of the channel within the process
   *
   * @return The minimum DN in the input mapped to the minimum of the output
   */
  double ImageExporter::getInputMinimum(int channel) const {
    return m_process->GetInputMinimum(channel);
  }


  /**
   * Returns the input maximum for the given channel.  Any value of max in the
   * input cube will be mapped to the output max of the output channel.
   *
   * @param channel Index of the channel within the process
   *
   * @return The maximum DN in the input mapped to the maximum of the output
   */
  double ImageExporter::getInputMaximum(int channel) const {
    return m_process->GetInputMaximum(channel);
  }


  /**
   * Set the input DN floor and ceiling.  All input DNs less than the min will
   * be set to the min in the output.  Similarly, all input DNs greater than the
   * max will be set to the max in the output.
   *
   * @param min The minimum
   * @param max The maximum
   */
  void ImageExporter::setOutputRange(double min, double max) {
    m_dataMin = min;
    m_dataMax = max;
  }


  /**
   * Sets the extension for the output image and generates the extension for the
   * world file from it.
   *
   * @param extension The extension for the output image
   */
  void ImageExporter::setExtension(QString extension) {
    m_extension = extension;

    // World file extension is the first and last characters of the extension
    // with an added 'w' at the end
    int last = extension.length() - 1;
    m_world = extension.mid(0, 1) + extension.mid(last) + "w";
  }


  /**
   * Sets up the export process with the parameters described within the given
   * description.  Opens cubes for retrieving input data, establishes the
   * dimensions of the output image, and determines whether to write the data as
   * grayscale, RGB, or RGBA.
   *
   * @param desc The export description
   *
   * @return A cube pointer to the first channel created, owned by the process
   */
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
            "Cannot export an image with [" + QString(desc.channelCount()) +
            "] channels",
            _FILEINFO_);
    }

    ProcessExport &p = getProcess();
    Cube *cube = addChannel(desc, 0);
    m_samples = cube->sampleCount();
    m_lines = cube->lineCount();
    m_bands = desc.channelCount();

    for (int i = 1; i < desc.channelCount(); i++) addChannel(desc, i);

    p.SetOutputRange(1.0, 255.0);
    p.SetOutputNull(0.0);
    return cube;
  }


  /**
   * Get a reference to the process object, useful for subclasses to access and
   * manipulate the process.
   *
   * @return A reference to the process object
   */
  ProcessExport & ImageExporter::getProcess() const {
    return *m_process;
  }


  /**
   * Return the output clamped integer pixel value from the input
   * double-precision DN.
   *
   * @param dn The input DN
   *
   * @return The pixel value for the output
   */
  int ImageExporter::getPixel(double dn) const {
    return (dn < m_dataMin) ? m_dataMin : (dn > m_dataMax) ? m_dataMax : dn;
  }


  /**
   * Add a channel of input data to the process from the export description at
   * the given index.
   *
   * @param desc The export description
   * @param i Index of the channel to add within the export description
   *
   * @return A cube pointer to the channel added, owned by the process
   */
  Cube * ImageExporter::addChannel(ExportDescription &desc, int i) {
    ProcessExport &p = getProcess();

    const ExportDescription::ChannelDescription &channel = desc.getChannel(i);
    Cube *cube = p.SetInputCube(
        channel.filename().expanded(), channel.attributes(), Isis::OneBand);

    if (channel.hasCustomRange())
      p.SetInputRange(channel.getInputMinimum(), channel.getInputMaximum(), i);

    return cube;
  }


  /**
   * Creates a world file is the input has a map projection.
   *
   * @param outputName Name of the world file
   */
  void ImageExporter::createWorldFile(FileName outputName) {
    outputName = outputName.removeExtension();
    outputName = outputName.addExtension(m_world);

    ProcessExport &p = getProcess();
    p.CreateWorldFile(outputName.expanded());
    p.EndProcess();
  }


  /**
   * A static (factory) method for constructing an ImageExporter instance from
   * an output format.  The specific subclass of the returned instance is
   * determined from the desired output image format provided.  Each subclass
   * has knowledge of whether or not it can write a particular format.  Because
   * the ability to export an image format is not mutually exclusive amongst
   * exporters, the order of condieration here matters.  For example, using a
   * TIFF exporter takes precedence over a Qt exporter for TIFF images, because
   * the former can process cubes greater than 2GB while the latter cannot.  It
   * is the caller's responsibility to delete the exporter instance when they
   * are finished with it.
   *
   * @param format The format for the output image to be created
   *
   * @return A pointer to the instantiated exporter owned by the caller
   */
  ImageExporter * ImageExporter::fromFormat(QString format) {
    ImageExporter *exporter = NULL;

    format = format.toLower();
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

