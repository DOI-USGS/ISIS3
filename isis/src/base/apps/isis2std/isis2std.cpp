#include "Isis.h"
#include "ProcessExport.h"
#include "JP2Encoder.h"

#include "UserInterface.h"
#include "Filename.h"
#include <QImageWriter>
#include <QImage>

using namespace std;
using namespace Isis;
QImage *qimage;
JP2Encoder *JP2_encoder;
char **jp2buf;
Cube *icube;
Isis::PixelType jp2type;
int datamin;
int datamax;

// Line-by-Line qimage output prototype
void checkDataSize (Isis::BigInt line, Isis::BigInt samp, iString mode);
void toGreyscaleImage (Buffer &in);
void toRGBImage (vector<Buffer *> &in);
void toARGBImage (vector<Buffer *> &in);
void toJP2 (Buffer &in);
void toJP2RGB (vector<Buffer *> &in);
void toJP2ARGB (vector<Buffer *> &in);

// Main program
void IsisMain() {
  // See if qts support the users desired output format
  UserInterface &ui = Application::GetUserInterface();
  iString format = ui.GetString("FORMAT");
  format.DownCase();
  int quality = ui.GetInteger("QUALITY");

  iString mode = ui.GetString("MODE");

  // Create an object for exporting image data
  ProcessExport p;

  if (format.Equal("jp2")) {
    // Setup the required extension and world file
    string extension(""),world("");
    extension = "jp2";
    world = "j2w";

    // Single band cubes will be greyscale
    if (mode == "GRAYSCALE") {
      // Open the input cube
      icube = p.SetInputCube("FROM",Isis::OneBand);

      // Apply the input to output stretch options
      p.SetInputRange();
      p.SetFormat(ProcessExport::BIL);
    
      jp2buf = new char* [1];
      // Determine bit size and output range 
      if (ui.GetString("BITTYPE") == "8BIT") {
        jp2type = Isis::UnsignedByte;
        jp2buf[0] = new char[icube->Samples()];
        p.SetOutputType(Isis::UnsignedByte);
        p.SetOutputRange(1.0,255.0);
        p.SetOutputNull(0.0);
        JP2_encoder = new JP2Encoder(ui.GetFilename("TO"),icube->Samples(),
               icube->Lines(),icube->Bands(),Isis::UnsignedByte);
        datamin = 0;
        datamax = 255;
      } else if (ui.GetString("BITTYPE") == "S16BIT") {
        jp2type = Isis::SignedWord;
        for (int i=0; i<icube->Bands(); i++) {
          jp2buf[i] = new char[icube->Samples()*2];
        }
        p.SetOutputType(Isis::SignedWord);
        p.SetOutputNull(-32768.0);
        p.SetOutputRange(-32752.0,32767.0);
        JP2_encoder = new JP2Encoder(ui.GetFilename("TO"),icube->Samples(),
               icube->Lines(),icube->Bands(),Isis::SignedWord);
        datamin = -32752;
        datamax = 32767;
      } else if (ui.GetString("BITTYPE") == "U16BIT") {
        jp2type = Isis::UnsignedWord;
        for (int i=0; i<icube->Bands(); i++) {
          jp2buf[i] = new char[icube->Samples()*2];
        }
        p.SetOutputType(Isis::UnsignedWord);
        p.SetOutputNull(0.0);
        p.SetOutputRange(3.0,65522.0);
        JP2_encoder = new JP2Encoder(ui.GetFilename("TO"),icube->Samples(),
               icube->Lines(),icube->Bands(),Isis::UnsignedWord);
        datamin = 3;
        datamax = 65522;
      }
      JP2_encoder->OpenFile();
      p.StartProcess(toJP2);
    }
    //  If RGB, create a 24-bit color image
    //     (1st band -> red, 2nd band -> green, 3rd band -> blue)
    else if (mode == "RGB") {
      Cube *redcube = p.SetInputCube("RED", Isis::OneBand);
      p.SetInputCube("GREEN", Isis::OneBand);
      p.SetInputCube("BLUE", Isis::OneBand);
      if (ui.GetString("STRETCH") == "MANUAL") {
        p.SetInputRange(ui.GetDouble("RMIN"), ui.GetDouble("RMAX"), 0);
        p.SetInputRange(ui.GetDouble("GMIN"), ui.GetDouble("GMAX"), 1);
        p.SetInputRange(ui.GetDouble("BMIN"), ui.GetDouble("BMAX"), 2);
      }
      else {
        p.SetInputRange();
        ui.Clear("MINIMUM");
        ui.Clear("MAXIMUM");
        ui.PutDouble("RMIN", p.GetInputMinimum(0));
        ui.PutDouble("RMAX", p.GetInputMaximum(0));
        ui.PutDouble("GMIN", p.GetInputMinimum(1));
        ui.PutDouble("GMAX", p.GetInputMaximum(1));
        ui.PutDouble("BMIN", p.GetInputMinimum(2));
        ui.PutDouble("BMAX", p.GetInputMaximum(2));
      }
      p.SetFormat(ProcessExport::BIL);
      jp2buf = new char* [3];
      // Determine bit size and output range 
      if (ui.GetString("BITTYPE") == "8BIT") {
        jp2type = Isis::UnsignedByte;
        for (int i=0; i<3; i++) {
          jp2buf[i] = new char[redcube->Samples()];
        }
        p.SetOutputType(Isis::UnsignedByte);
        p.SetOutputRange(1.0,255.0);
        p.SetOutputNull(0.0);
        JP2_encoder = new JP2Encoder(ui.GetFilename("TO"),redcube->Samples(),
               redcube->Lines(),3,Isis::UnsignedByte);
        datamin = 0;
        datamax = 255;
      } else if (ui.GetString("BITTYPE") == "S16BIT") {
        jp2type = Isis::SignedWord;
        for (int i=0; i<3; i++) {
          jp2buf[i] = new char[redcube->Samples()*2];
        }
        p.SetOutputType(Isis::SignedWord);
        p.SetOutputNull(-32768.0);
        p.SetOutputRange(-32752.0,32767.0);
        JP2_encoder = new JP2Encoder(ui.GetFilename("TO"),redcube->Samples(),
               redcube->Lines(),3,Isis::SignedWord);
        datamin = -32752;
        datamax = 32767;
      } else if (ui.GetString("BITTYPE") == "U16BIT") {
        jp2type = Isis::UnsignedWord;
        for (int i=0; i<3; i++) {
          jp2buf[i] = new char[redcube->Samples()*2];
        }
        p.SetOutputType(Isis::UnsignedWord);
        p.SetOutputNull(0.0);
        p.SetOutputRange(3.0,65522.0);
        JP2_encoder = new JP2Encoder(ui.GetFilename("TO"),redcube->Samples(),
               redcube->Lines(),3,Isis::UnsignedWord);
        datamin = 3;
        datamax = 65522;
      }
      JP2_encoder->OpenFile();
      p.StartProcess(toJP2RGB);
    } else if (mode == "ARGB") {
      Cube *alphacube = p.SetInputCube("ALPHA", Isis::OneBand);
      p.SetInputCube("RED", Isis::OneBand);
      p.SetInputCube("GREEN", Isis::OneBand);
      p.SetInputCube("BLUE", Isis::OneBand);
      if (ui.GetString("STRETCH") == "MANUAL") {
        p.SetInputRange(ui.GetDouble("AMIN"), ui.GetDouble("AMAX"), 0);
        p.SetInputRange(ui.GetDouble("RMIN"), ui.GetDouble("RMAX"), 1);
        p.SetInputRange(ui.GetDouble("GMIN"), ui.GetDouble("GMAX"), 2);
        p.SetInputRange(ui.GetDouble("BMIN"), ui.GetDouble("BMAX"), 3);
      }
      else {
        p.SetInputRange();
        ui.Clear("MINIMUM");
        ui.Clear("MAXIMUM");
        ui.PutDouble("AMIN", p.GetInputMinimum(0));
        ui.PutDouble("AMAX", p.GetInputMaximum(0));
        ui.PutDouble("RMIN", p.GetInputMinimum(1));
        ui.PutDouble("RMAX", p.GetInputMaximum(1));
        ui.PutDouble("GMIN", p.GetInputMinimum(2));
        ui.PutDouble("GMAX", p.GetInputMaximum(2));
        ui.PutDouble("BMIN", p.GetInputMinimum(3));
        ui.PutDouble("BMAX", p.GetInputMaximum(3));
      }
      p.SetFormat(ProcessExport::BIL);
      jp2buf = new char* [4];
      // Determine bit size and output range 
      if (ui.GetString("BITTYPE") == "8BIT") {
        jp2type = Isis::UnsignedByte;
        for (int i=0; i<4; i++) {
          jp2buf[i] = new char[alphacube->Samples()];
        }
        p.SetOutputType(Isis::UnsignedByte);
        p.SetOutputRange(1.0,255.0);
        p.SetOutputNull(0.0);
        JP2_encoder = new JP2Encoder(ui.GetFilename("TO"),alphacube->Samples(),
               alphacube->Lines(),4,Isis::UnsignedByte);
        datamin = 0;
        datamax = 255;
      } else if (ui.GetString("BITTYPE") == "S16BIT") {
        jp2type = Isis::SignedWord;
        for (int i=0; i<4; i++) {
          jp2buf[i] = new char[alphacube->Samples()*2];
        }
        p.SetOutputType(Isis::SignedWord);
        p.SetOutputNull(-32768.0);
        p.SetOutputRange(-32752.0,32767.0);
        JP2_encoder = new JP2Encoder(ui.GetFilename("TO"),alphacube->Samples(),
               alphacube->Lines(),4,Isis::SignedWord);
        datamin = -32752;
        datamax = 32767;
      } else if (ui.GetString("BITTYPE") == "U16BIT") {
        jp2type = Isis::UnsignedWord;
        for (int i=0; i<4; i++) {
          jp2buf[i] = new char[alphacube->Samples()*2];
        }
        p.SetOutputType(Isis::UnsignedWord);
        p.SetOutputNull(0.0);
        p.SetOutputRange(3.0,65522.0);
        JP2_encoder = new JP2Encoder(ui.GetFilename("TO"),alphacube->Samples(),
               alphacube->Lines(),4,Isis::UnsignedWord);
        datamin = 3;
        datamax = 65522;
      }
      JP2_encoder->OpenFile();
      p.StartProcess(toJP2ARGB);
    }
    // Create a world file if it has a map projection
    Filename fname(ui.GetFilename("TO"));
    fname.RemoveExtension();
    fname.AddExtension(world);
    p.CreateWorldFile(fname.Expanded());

    p.EndProcess();
    delete JP2_encoder;
  } else {
    QList<QByteArray> list = QImageWriter::supportedImageFormats();
    QList<QByteArray>::Iterator it = list.begin();
    bool supported = false;
    while (it != list.end()) {
      if (*it == QString(format.c_str())) supported = true;
      ++it;
    }

    if (!supported) {
      string msg = "The installation of Trolltech/Qt does not support ";
      msg += "your selected format ["+format+"]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    // Setup the required extension and world file
    string extension(""),world("");
    if (format == "png") {
      extension = "png";
      world = "pgw";
    }
    else if (format == "jpeg") {
      extension = "jpg";
      world = "jgw";
    }
    else if (format == "tiff") {
      extension = "tif";
      world = "tfw";
    }
    else if (format == "gif") {
      extension = "gif";
      world = "gfw";
    }
    else if (format == "bmp") {
      extension = "bmp";
      world = "bpw";
    }

    // Cubes with less than three bands will be greyscale
    if (mode == "GRAYSCALE") {
      Cube *icube = p.SetInputCube("FROM", Isis::OneBand);
      checkDataSize(icube->Lines(),icube->Samples(),mode);
      qimage = new QImage(icube->Samples(),icube->Lines(), QImage::Format_Indexed8);
      qimage->setNumColors(256);
      QVector<QRgb> colors;
      //  create the color table (black = 0 to white = 255)
      for (int i = 0; i<256; i++) {
        colors.push_back(qRgb(i,i,i));
      }
      qimage->setColorTable(colors);
      p.SetInputRange();
      p.SetOutputRange(1.0,255.0);
      p.SetOutputNull(0.0);
      p.StartProcess(toGreyscaleImage);
    }
    //  If RGB, create a 24-bit color image
    //     (1st band -> red, 2nd band -> green, 3rd band -> blue)
    else if (mode == "RGB") {
      Cube *redcube = p.SetInputCube("RED", Isis::OneBand);
      checkDataSize(redcube->Lines(),redcube->Samples(),mode);
      p.SetInputCube("GREEN", Isis::OneBand);
      p.SetInputCube("BLUE", Isis::OneBand);
      qimage = new QImage(redcube->Samples(),redcube->Lines(),QImage::Format_RGB32);
      if (ui.GetString("STRETCH") == "MANUAL") {
        p.SetInputRange(ui.GetDouble("RMIN"), ui.GetDouble("RMAX"), 0);
        p.SetInputRange(ui.GetDouble("GMIN"), ui.GetDouble("GMAX"), 1);
        p.SetInputRange(ui.GetDouble("BMIN"), ui.GetDouble("BMAX"), 2);
      }
      else {
        p.SetInputRange();
        ui.Clear("MINIMUM");
        ui.Clear("MAXIMUM");
        ui.PutDouble("RMIN", p.GetInputMinimum(0));
        ui.PutDouble("RMAX", p.GetInputMaximum(0));
        ui.PutDouble("GMIN", p.GetInputMinimum(1));
        ui.PutDouble("GMAX", p.GetInputMaximum(1));
        ui.PutDouble("BMIN", p.GetInputMinimum(2));
        ui.PutDouble("BMAX", p.GetInputMaximum(2));
      }
      p.SetOutputRange(1.0,255.0);
      p.SetOutputNull(0.0);
      p.StartProcess(toRGBImage);
    }
    else if (mode == "ARGB") {
      Cube *alpha = p.SetInputCube("ALPHA", Isis::OneBand);
      checkDataSize(alpha->Lines(),alpha->Samples(),mode);
      p.SetInputCube("RED", Isis::OneBand);
      p.SetInputCube("GREEN", Isis::OneBand);
      p.SetInputCube("BLUE", Isis::OneBand);
      qimage = new QImage(alpha->Samples(),alpha->Lines(),QImage::Format_ARGB32);
      if (ui.GetString("STRETCH") == "MANUAL") {
        p.SetInputRange(ui.GetDouble("AMIN"), ui.GetDouble("AMAX"), 0);
        p.SetInputRange(ui.GetDouble("RMIN"), ui.GetDouble("RMAX"), 1);
        p.SetInputRange(ui.GetDouble("GMIN"), ui.GetDouble("GMAX"), 2);
        p.SetInputRange(ui.GetDouble("BMIN"), ui.GetDouble("BMAX"), 3);
      }
      else {
        p.SetInputRange();
        ui.Clear("MINIMUM");
        ui.Clear("MAXIMUM");
        ui.PutDouble("AMIN", p.GetInputMinimum(0));
        ui.PutDouble("AMAX", p.GetInputMaximum(0));
        ui.PutDouble("RMIN", p.GetInputMinimum(1));
        ui.PutDouble("RMAX", p.GetInputMaximum(1));
        ui.PutDouble("GMIN", p.GetInputMinimum(2));
        ui.PutDouble("GMAX", p.GetInputMaximum(2));
        ui.PutDouble("BMIN", p.GetInputMinimum(3));
        ui.PutDouble("BMAX", p.GetInputMaximum(3));
      }
      p.SetOutputRange(1.0,255.0);
      p.SetOutputNull(0.0);
      p.StartProcess(toARGBImage);
    }

    // Get the name of the file and write it
    Filename fname(ui.GetFilename("TO"));
    fname.AddExtension(extension);
    string filename(fname.Expanded());

    // The return status is wrong for JPEG images, so the code will always
    //   continue.
    if(!qimage->save(filename.c_str(),format.c_str(),quality)) {
      delete qimage;
      iString err = "Unable to save [";
      err += filename.c_str();
      err += "] to the disk";
      throw iException::Message(iException::User, err, _FILEINFO_);
    }

    // Create a world file if it has a map projection
    fname.RemoveExtension();
    fname.AddExtension(world);
    p.CreateWorldFile(fname.Expanded());
    p.EndProcess();

    delete qimage;
  }
}

// Write a line of data to the greyscale qimage object
void toGreyscaleImage (Buffer &in) {
  // Loop for each column and load the pixel from in[i] which will
  // be in the range of [0,255]
  for (int i=0; i<in.size(); i++) {
    int dn = (int) in[i];
    if (dn < 0) dn = 0;
    if (dn > 255) dn = 255;
    qimage->setPixel(i, in.Line()-1, dn);
    // Since the plausable "exception" thrown by setPixel cannot be caught,
    //  the following if statement does it informally.
    if( !qimage->valid(i,in.Line()-1) ) {
      string msg = "QT has detected your file size as exceeding 2GB.";
      msg += " While your image might be under 2GB, your image labels are more";
      msg += " than likely pushing the file size over 2GB.";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
  }
}

// Check to see if the QImage will be larger than 2GB
// error if it will be
void checkDataSize (Isis::BigInt line, Isis::BigInt samp, iString mode){
  Isis::BigInt maxSize = (Isis::BigInt)2*1024*1024*1024;  //2GB limit
  Isis::BigInt size = 0;
  if(mode == "GRAYSCALE") {
    size = line*samp;
  } else if(mode == "RGB") {
    size = (line*samp)*3;
  } else if(mode == "ARGB") {
    size = (line*samp)*4;
  }
  if (size >= maxSize) {
    double inGB = (double)size / (1024*1024*1024);
    string msg = "Cube exceeds max size of 2GB. Qimage cannot support ";
    msg += "that much raw data. Your cube is "+(iString)inGB+" GB.";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }
  return;
}

// Write a line of data to the rgb qimage object
void toRGBImage (vector<Buffer *> &in) {
  Buffer &red = *in[0];
  Buffer &green = *in[1];
  Buffer &blue = *in[2];

  // Set magick pointer to the desired row and construct a Color
  QRgb *line = (QRgb *) qimage->scanLine(red.Line()-1);

  // Loop for each column and load the pixel from in[i] which will
  // be in the range of [0,255]
  for (int i=0; i<red.size(); i++) {
    int redDN = (int)red[i];
    if (redDN < 0) redDN = 0;
    else if (redDN > 255) redDN = 255;

    int greenDN = (int)green[i];
    if (greenDN < 0) greenDN = 0;
    else if (greenDN > 255) greenDN = 255;

    int blueDN = (int)blue[i];
    if (blueDN < 0) blueDN = 0;
    else if (blueDN > 255) blueDN = 255;

    line[i] = qRgb(redDN, greenDN, blueDN);
  }
}

// Write a line of data to the argb qimage object
void toARGBImage (vector<Buffer *> &in) {
  Buffer &alpha = *in[0];
  Buffer &red = *in[1];
  Buffer &green = *in[2];
  Buffer &blue = *in[3];

  // Set magick pointer to the desired row and construct a Color
  QRgb *line = (QRgb *) qimage->scanLine(red.Line()-1);

  // Loop for each column and load the pixel from in[i] which will
  // be in the range of [0,255]
  for (int i=0; i<red.size(); i++) {
    int redDN = (int)red[i];
    if (redDN < 0) redDN = 0;
    else if (redDN > 255) redDN = 255;

    int greenDN = (int)green[i];
    if (greenDN < 0) greenDN = 0;
    else if (greenDN > 255) greenDN = 255;

    int blueDN = (int)blue[i];
    if (blueDN < 0) blueDN = 0;
    else if (blueDN > 255) blueDN = 255;

    int alphaDN = (int)alpha[i];
    if (alphaDN < 0) alphaDN = 0;
    else if (alphaDN > 255) alphaDN = 255;

    line[i] = qRgba(redDN, greenDN, blueDN, alphaDN);
  }
}

// Write a line of data to the JP2 object
void toJP2 (Buffer &in) {
  int dn;
  for (int i=0; i<icube->Samples(); i++) {
    dn = ((int)in[i] < datamin) ? datamin : (int)in[i];
    dn = ((int)in[i] > datamax) ? datamax : (int)in[i];
    if (jp2type == Isis::UnsignedByte) {
      ((unsigned char*)jp2buf[0])[i] = (unsigned char)dn;
    } else if (jp2type == Isis::SignedWord) {
      ((short int*)jp2buf[0])[i] = (short int)dn;
    } else if (jp2type == Isis::UnsignedWord) {
      ((short unsigned int*)jp2buf[0])[i] = (short unsigned int)dn;
    }
  }
  if (jp2type == Isis::UnsignedByte) {
    JP2_encoder->Write((unsigned char**)jp2buf);
  } else {
    JP2_encoder->Write((short int**)jp2buf);
  }
}

// Write a line of data to the JP2 RGB object
void toJP2RGB (vector<Buffer *> &in) {
  Buffer &red = *in[0];
  Buffer &green = *in[1];
  Buffer &blue = *in[2];

  int reddn;
  int greendn;
  int bluedn;
  for (int i=0; i<red.size(); i++) {
    reddn = ((int)red[i] < datamin) ? datamin : (int)red[i];
    reddn = ((int)red[i] > datamax) ? datamax : (int)red[i];
    greendn = ((int)green[i] < datamin) ? datamin : (int)green[i];
    greendn = ((int)green[i] > datamax) ? datamax : (int)green[i];
    bluedn = ((int)blue[i] < datamin) ? datamin : (int)blue[i];
    bluedn = ((int)blue[i] > datamax) ? datamax : (int)blue[i];
    if (jp2type == Isis::UnsignedByte) {
      ((unsigned char*)jp2buf[0])[i] = (unsigned char)reddn;
      ((unsigned char*)jp2buf[1])[i] = (unsigned char)greendn;
      ((unsigned char*)jp2buf[2])[i] = (unsigned char)bluedn;
    } else if (jp2type == Isis::SignedWord) {
      ((short int*)jp2buf[0])[i] = (short int)reddn;
      ((short int*)jp2buf[1])[i] = (short int)greendn;
      ((short int*)jp2buf[2])[i] = (short int)bluedn;
    } else if (jp2type == Isis::UnsignedWord) {
      ((short unsigned int*)jp2buf[0])[i] = (short unsigned int)reddn;
      ((short unsigned int*)jp2buf[1])[i] = (short unsigned int)greendn;
      ((short unsigned int*)jp2buf[2])[i] = (short unsigned int)bluedn;
    }
  }
  if (jp2type == Isis::UnsignedByte) {
    JP2_encoder->Write((unsigned char**)jp2buf);
  } else {
    JP2_encoder->Write((short int**)jp2buf);
  }
}

// Write a line of data to the JP2 ARGB object
void toJP2ARGB (vector<Buffer *> &in) {
  Buffer &alpha = *in[0];
  Buffer &red = *in[1];
  Buffer &green = *in[2];
  Buffer &blue = *in[3];

  int reddn;
  int greendn;
  int bluedn;
  int alphadn;
  for (int i=0; i<red.size(); i++) {
    reddn = ((int)red[i] < datamin) ? datamin : (int)red[i];
    reddn = ((int)red[i] > datamax) ? datamax : (int)red[i];
    greendn = ((int)green[i] < datamin) ? datamin : (int)green[i];
    greendn = ((int)green[i] > datamax) ? datamax : (int)green[i];
    bluedn = ((int)blue[i] < datamin) ? datamin : (int)blue[i];
    bluedn = ((int)blue[i] > datamax) ? datamax : (int)blue[i];
    alphadn = ((int)alpha[i] < datamin) ? datamin : (int)alpha[i];
    alphadn = ((int)alpha[i] > datamax) ? datamax : (int)alpha[i];
    if (jp2type == Isis::UnsignedByte) {
      ((unsigned char*)jp2buf[0])[i] = (unsigned char)reddn;
      ((unsigned char*)jp2buf[1])[i] = (unsigned char)greendn;
      ((unsigned char*)jp2buf[2])[i] = (unsigned char)bluedn;
      ((unsigned char*)jp2buf[3])[i] = (unsigned char)alphadn;
    } else if (jp2type == Isis::SignedWord) {
      ((short int*)jp2buf[0])[i] = (short int)reddn;
      ((short int*)jp2buf[1])[i] = (short int)greendn;
      ((short int*)jp2buf[2])[i] = (short int)bluedn;
      ((short int*)jp2buf[3])[i] = (short int)alphadn;
    } else if (jp2type == Isis::UnsignedWord) {
      ((short unsigned int*)jp2buf[0])[i] = (short unsigned int)reddn;
      ((short unsigned int*)jp2buf[1])[i] = (short unsigned int)greendn;
      ((short unsigned int*)jp2buf[2])[i] = (short unsigned int)bluedn;
      ((short unsigned int*)jp2buf[3])[i] = (short unsigned int)alphadn;
    }
  }
  if (jp2type == Isis::UnsignedByte) {
    JP2_encoder->Write((unsigned char**)jp2buf);
  } else {
    JP2_encoder->Write((short int**)jp2buf);
  }
}
