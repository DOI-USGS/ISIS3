#include "JP2Importer.h"

#include <sstream>

#include "Filename.h"
#include "IException.h"
#include "ProcessImport.h"
#include "JP2Decoder.h"

using namespace std;
using namespace Isis;


namespace Isis {
  JP2Importer::JP2Importer(Filename inputName) : ImageImporter(inputName) {
    try {
      // Determine if input file is a JPEG2000 file
      JP2Decoder *JP2_decoder;
      JP2_decoder = new JP2Decoder(inputName.Expanded());
      JP2_decoder->OpenFile();
      setSamples(JP2_decoder->GetSampleDimension());
      setLines(JP2_decoder->GetLineDimension());
      setBands(JP2_decoder->GetBandDimension());
      m_pixelBytes = JP2_decoder->GetPixelBytes();
      m_signed = JP2_decoder->GetSignedData();
      delete JP2_decoder;
    }
    catch (IException &e) {
      throw IException(IException::User,
          "The file [" + inputName.Expanded() +
          "] does not contain a recognized image format",
          _FILEINFO_);
    }
  }


  JP2Importer::~JP2Importer() {
  }


  Cube * JP2Importer::import(Filename outputName, CubeAttributeOutput &att) {
    ProcessImport jp;
    jp.SetDimensions(samples(), lines(), bands());
    if (m_pixelBytes == 1) {
      jp.SetPixelType(Isis::UnsignedByte);
    }
    else if (m_pixelBytes == 2) {
      jp.SetPixelType(m_signed ? Isis::SignedWord : Isis::UnsignedWord);
    }
    else {
      throw IException(IException::User,
          "The file [" + filename().Expanded() +
          "] contains unsupported data type",
          _FILEINFO_);
    }

    //Cube *cube = createOutput(outputName, att);
    //jp.SetInputCube(cube);

    jp.SetInputFile(filename().Expanded());
    jp.SetOutputCube("TO");
    //jp.SetOutputCube(outputName.Expanded(), att, samples(), lines(), bands());
    jp.SetOrganization(ProcessImport::JP2);
    jp.StartProcess();
    jp.EndProcess();

    //return cube;
    return NULL;
  }


  bool JP2Importer::isGrayscale() const {
    return false;
  }


  bool JP2Importer::isRgb() const {
    return false;
  }


  bool JP2Importer::isArgb() const {
    return false;
  }


  int JP2Importer::getPixel(int s, int l) const {
    return 0;
  }


  int JP2Importer::getGray(int pixel) const {
    return 0;
  }


  int JP2Importer::getRed(int pixel) const {
    return 0;
  }


  int JP2Importer::getGreen(int pixel) const {
    return 0;
  }


  int JP2Importer::getBlue(int pixel) const {
    return 0;
  }


  int JP2Importer::getAlpha(int pixel) const {
    return 0;
  }
};

