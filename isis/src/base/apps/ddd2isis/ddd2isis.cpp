#include "Isis.h"
#include "ProcessImport.h"
#include "UserInterface.h"
#include "SpecialPixel.h"
#include "FileName.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  ProcessImport p;
  IString from = ui.GetFileName("FROM");
  EndianSwapper swp("MSB");
  int nsamples = 0;
  int nlines = 0;
  int nbands = 1;
  int noffset = 0;
  int bittype = 0;
  int nbytes = 0;

  union {
    char readChars[4];
    long readLong;
    float readFloat;
  } readBytes;

  ifstream fin;
  fin.open(from.c_str(), ios::in | ios::binary);
  if( !fin.is_open() ) {
    string msg = "Cannot open input file [" + from + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  /**
   *  0-rel byte offset   value
   *       0          32-bit integer magic number
   *       4          32-bit integer number of image lines
   *       8          32-bit integer number of bytes per image line
   *      12          32-bit integer number of bits per image elements
   *      16          32-bit integer currently unused
   *      20          32-bit integer number of bytes to start of image data
   *      24          ASCII label up to 1000 characters long
   *                  The label is NUL-terminated
   *
   */

  // Verify that the file is a .ddd by reading in the first 4 bytes and
  // comparing the magic numbers
  readBytes.readLong = 0;
  fin.seekg(0);
  fin.read(readBytes.readChars, 4);
  readBytes.readFloat = swp.Float(readBytes.readChars);

  if(readBytes.readLong != 0x67B) {
    string msg = "Input file [" + from + "] does not appear to be in ddd format";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  // Read bytes 4-7 to get number of lines
  fin.read(readBytes.readChars, 4);
  readBytes.readFloat = swp.Float(readBytes.readChars);
  nlines = (int)readBytes.readLong;

  // Read bytes 8-11 to get number of bytes
  fin.read(readBytes.readChars, 4);
  readBytes.readFloat = swp.Float(readBytes.readChars);
  nbytes = (int)readBytes.readLong;

  // Read bytes 12-15 to get bittype
  fin.read(readBytes.readChars, 4);
  readBytes.readFloat = swp.Float(readBytes.readChars);

  if( fin.fail() || fin.eof() ) {
    string msg = "An error ocurred when reading the input file [" + from + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  bittype = readBytes.readLong;

  fin.read(readBytes.readChars, 4);
  readBytes.readFloat = swp.Float(readBytes.readChars);

  fin.read(readBytes.readChars, 4);
  readBytes.readFloat = swp.Float(readBytes.readChars);
  noffset = (int)readBytes.readLong;
  if (noffset < 1024) {
    noffset = 1024;
  }

  PvlGroup results("FileInfo");
  results += PvlKeyword( "NumberOfLines", toString(nlines) );
  results += PvlKeyword( "NumberOfBytesPerLine", toString(nbytes) );
  results += PvlKeyword( "BitType", toString(bittype) );
  nsamples = nbytes / (bittype / 8);
  results += PvlKeyword( "NumberOfSamples", toString(nsamples) );
  nbands = nbytes / nsamples;
  results += PvlKeyword( "NumberOfBands", toString(nbands) );
  results += PvlKeyword( "LabelBytes", toString(noffset) );
  Application::Log(results);

  fin.close();

  if (ui.WasEntered("TO")) {
    switch(bittype) {
      case 8:
        p.SetPixelType(Isis::UnsignedByte);
        break;
      case 16:
        p.SetPixelType(Isis::UnsignedWord);
        break;
      case 32:
        p.SetPixelType(Isis::Real);
        break;
      default:
        IString msg = "Unsupported bit per pixel count [" + IString(bittype) + "]. ";
        msg += "(Use the raw2isis and crop programs to import the file in case it is ";
        msg += "line or sample interleaved.)";
        throw IException(IException::Io, msg, _FILEINFO_);
    }

    p.SetDimensions(nsamples, nlines, nbands);
    p.SetFileHeaderBytes(noffset);
    p.SetByteOrder(Isis::Msb);
    p.SetInputFile( ui.GetFileName("FROM") );
    p.SetOutputCube("TO");

    p.StartProcess();
    p.EndProcess();
  }

  return;
}
