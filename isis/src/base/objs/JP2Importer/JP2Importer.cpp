#include "JP2Importer.h"

#include "Filename.h"
#include "IException.h"
#include "JP2Decoder.h"
#include "ProcessImport.h"

using namespace Isis;


namespace Isis {
  JP2Importer::JP2Importer(Filename inputName) : ImageImporter(inputName) {
    m_decoder = NULL;
    m_buffer = NULL;

    try {
      // Determine if input file is a JPEG2000 file
      m_decoder = new JP2Decoder(inputName.Expanded());
      m_decoder->OpenFile();
      setSamples(m_decoder->GetSampleDimension());
      setLines(m_decoder->GetLineDimension());
      setBands(m_decoder->GetBandDimension());

      int pixelBytes = m_decoder->GetPixelBytes();
      if (pixelBytes == 1) {
        m_pixelType = Isis::UnsignedByte;
      }
      else if (pixelBytes == 2) {
        bool signedData = m_decoder->GetSignedData();
        m_pixelType = signedData ? Isis::SignedWord : Isis::UnsignedWord;
      }
      else {
        throw IException(IException::User,
            "The file [" + filename().Expanded() +
            "] contains unsupported data type",
            _FILEINFO_);
      }

      int pixelSize = Isis::SizeOf(m_pixelType);
      int readBytes = pixelSize * samples() * bands();

      m_buffer = new char* [bands()];
      for (int i = 0; i < bands(); i++) m_buffer[i] = new char [readBytes];
    }
    catch (IException &e) {
      throw IException(IException::Programmer,
          "The file [" + inputName.Expanded() +
          "] cannot be opened as a JPEG 2000 file",
          _FILEINFO_);
    }
  }


  JP2Importer::~JP2Importer() {
    delete m_decoder;
    m_decoder = NULL;

    delete [] m_buffer;
    m_buffer = NULL;
  }


  bool JP2Importer::isGrayscale() const {
    return m_decoder->GetBandDimension() == 1;
  }


  bool JP2Importer::isRgb() const {
    return m_decoder->GetBandDimension() == 3;
  }


  bool JP2Importer::isArgb() const {
    return m_decoder->GetBandDimension() == 4;
  }


  void JP2Importer::updateRawBuffer(int line, int band) const {
    // Only read a new chunk of data when we move to a new line, since we read
    // all the input bands for the current line at once
    if (band == 1) {
      if (m_pixelType == Isis::UnsignedByte)
        m_decoder->Read((unsigned char **) m_buffer);
      else
        m_decoder->Read((short int **) m_buffer);
    }
  }


  int JP2Importer::getPixel(int s, int l) const {
    return s;
  }


  int JP2Importer::getGray(int pixel) const {
    return isGrayscale() ? getFromBuffer(pixel, 0) : convertRgbToGray(pixel);
  }


  int JP2Importer::getRed(int pixel) const {
    return getFromBuffer(pixel, 0);
  }


  int JP2Importer::getGreen(int pixel) const {
    return getFromBuffer(pixel, 1);
  }


  int JP2Importer::getBlue(int pixel) const {
    return getFromBuffer(pixel, 2);
  }


  int JP2Importer::getAlpha(int pixel) const {
    return getFromBuffer(pixel, 3);
  }


  int JP2Importer::getFromBuffer(int s, int b) const {
    int value;

    switch (m_pixelType) {
      case Isis::UnsignedByte:
        value = (int) ((unsigned char *) m_buffer[b])[s];
        break;
      case Isis::UnsignedWord:
        value = (int) ((unsigned short int *) m_buffer[b])[s];
        break;
      case Isis::SignedWord:
        value = (int) ((short int *) m_buffer[b])[s];
        break;
      default:
        throw IException(IException::Programmer,
            "Unknown pixel type [" + iString(m_pixelType) + "]",
            _FILEINFO_);
    }

    return value;
  }
};

