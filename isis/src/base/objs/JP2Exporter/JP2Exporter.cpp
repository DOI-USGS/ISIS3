#include "JP2Exporter.h"

#include "Buffer.h"
#include "Filename.h"
#include "IException.h"
#include "JP2Encoder.h"

using namespace Isis;


namespace Isis {
  /**
   * Construct the importer.
   *
   * @param inputName The name of the input image
   */
  JP2Exporter::JP2Exporter() : StreamExporter() {
    m_encoder = NULL;
    m_buffer = NULL;

    setExtension("jp2");
  }


  /**
   * Destruct the importer.
   */
  JP2Exporter::~JP2Exporter() {
    delete m_encoder;
    m_encoder = NULL;

    delete [] m_buffer;
    m_buffer = NULL;
  }


  void JP2Exporter::createBuffer() {
    PixelType type = getPixelType();
    int mult = (type == Isis::UnsignedByte) ? 1 : 2;

    m_buffer = new char* [bands()];
    for (int i = 0; i < bands(); i++)
      m_buffer[i] = new char[samples() * mult];
  }


  void JP2Exporter::write(Filename outputName, int quality) {
    PixelType type = getPixelType();
    m_encoder = new JP2Encoder(
        outputName.Expanded(), samples(), lines(), bands(), type);
    m_encoder->OpenFile();

    ImageExporter::write(outputName, quality);
  }


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


  void JP2Exporter::writeLine(int l) const {
    PixelType type = getPixelType();
    if (type == Isis::UnsignedByte)
      m_encoder->Write((unsigned char **) m_buffer);
    else
      m_encoder->Write((short int **) m_buffer);
  }


  bool JP2Exporter::canWriteFormat(iString format) {
    return format == "jp2";
  }
};

