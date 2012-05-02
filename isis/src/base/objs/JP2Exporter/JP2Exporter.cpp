#include "JP2Exporter.h"

#include "Buffer.h"
#include "FileName.h"
#include "IException.h"
#include "JP2Encoder.h"

using namespace Isis;


namespace Isis {
  /**
   * Construct the JPEG 2000 exporter.
   */
  JP2Exporter::JP2Exporter() : StreamExporter() {
    m_encoder = NULL;
    m_buffer = NULL;

    setExtension("jp2");
  }


  /**
   * Destruct the exporter.
   */
  JP2Exporter::~JP2Exporter() {
    delete m_encoder;
    m_encoder = NULL;

    delete [] m_buffer;
    m_buffer = NULL;
  }


  /**
   * Creates the buffer to store a chunk of streamed line data with one or more
   * bands.
   */
  void JP2Exporter::createBuffer() {
    PixelType type = getPixelType();
    int mult = (type == Isis::UnsignedByte) ? 1 : 2;

    m_buffer = new char* [bands()];
    for (int i = 0; i < bands(); i++)
      m_buffer[i] = new char[samples() * mult];
  }


  /**
   * Initialize the encoder, open the output file for writing, then let the base
   * ImageExporter handle the generic black-box writing routine.
   *
   * @param outputName The filename of the output cube
   * @param quality The quality of the output, not used for JPEG 2000
   */
  void JP2Exporter::write(FileName outputName, int quality) {
    PixelType type = getPixelType();
    m_encoder = new JP2Encoder(
        outputName.expanded(), samples(), lines(), bands(), type);
    m_encoder->OpenFile();

    ImageExporter::write(outputName, quality);
  }


  /**
   * Set the DN value at the given sample and band of the line buffer.
   *
   * @param s The sample index into the buffer
   * @param b The band index into the buffer
   * @param dn The value to set at the given index
   */
  void JP2Exporter::setBuffer(int s, int b, int dn) const {
    PixelType type = getPixelType();
    switch (type) {
      case UnsignedByte:
        ((unsigned char *) m_buffer[b])[s] = (unsigned char) dn;
        break;
      case SignedWord:
        ((short int *) m_buffer[b])[s] = (short int) dn;
        break;
      case UnsignedWord:
        ((short unsigned int *) m_buffer[b])[s] = (short unsigned int) dn;
        break;
      default:
        throw IException(IException::Programmer,
            "Invalid pixel type for data [" + iString(type) + "]",
            _FILEINFO_);
    }
  }


  /**
   * Writes a line of buffered data to the output image on disk.
   *
   * @param l The line of the output image, unused for JPEG 2000
   */
  void JP2Exporter::writeLine(int l) const {
    PixelType type = getPixelType();
    if (type == Isis::UnsignedByte)
      m_encoder->Write((unsigned char **) m_buffer);
    else
      m_encoder->Write((short int **) m_buffer);
  }


  /**
   * Returns true if the format is "jp2".
   *
   * @param format Lowercase format abbreviation
   *
   * @return True if "jp2", false otherwise
   */
  bool JP2Exporter::canWriteFormat(iString format) {
    return format == "jp2";
  }
};

