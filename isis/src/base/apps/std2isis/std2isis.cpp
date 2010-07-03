#include "Isis.h"
#include "ProcessByLine.h"
#include "ProcessImport.h"
#include "SpecialPixel.h"
#include "UserInterface.h"
#include "JP2Decoder.h"
#include <QImage>

using namespace std;
using namespace Isis;

QImage *qimage;
int line;
int band;

//Initialize values to make special pixels invalid
double null_min = DBL_MAX;
double null_max = DBL_MIN;
double hrs_min = DBL_MAX;
double hrs_max = DBL_MIN;
double lrs_min = DBL_MAX;
double lrs_max = DBL_MIN;

void toGrayCube (Buffer &out);
void toRGBCube (Buffer &out);
void toARGBCube (Buffer &out);

double TestSpecial(const double pixel);

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  ProcessByLine p;


  // Set special pixel ranges
  if (ui.GetBoolean("SETNULLRANGE")) {
    null_min = ui.GetDouble("NULLMIN");
    null_max = ui.GetDouble("NULLMAX");
  }
  if (ui.GetBoolean("SETHRSRANGE")) {
    hrs_min = ui.GetDouble("HRSMIN");
    hrs_max = ui.GetDouble("HRSMAX");
  }
  if (ui.GetBoolean("SETLRSRANGE")) {
    lrs_min = ui.GetDouble("LRSMIN");
    lrs_max = ui.GetDouble("LRSMAX");
  }

  qimage = new QImage(iString(ui.GetFilename("FROM")));

  line = 0;
  band = 0;

  // qimage is NULL on failure
  if(qimage->isNull()) {
    delete qimage;
    qimage = NULL;

    // Determine if input file is a JPEG2000 file
    try {
      JP2Decoder *JP2_decoder;
      JP2_decoder = new JP2Decoder(iString(ui.GetFilename("FROM")));
      JP2_decoder->OpenFile();
      int nsamps = JP2_decoder->GetSampleDimension();
      int nlines = JP2_decoder->GetLineDimension();
      int nbands = JP2_decoder->GetBandDimension();
      int pixelbytes = JP2_decoder->GetPixelBytes();
      bool is_signed = JP2_decoder->GetSignedData();
      delete JP2_decoder;
      ProcessImport jp;
      jp.SetDimensions(nsamps,nlines,nbands);
      if (pixelbytes == 1) {
        jp.SetPixelType(Isis::UnsignedByte);
      } else if (pixelbytes == 2) {
        if (is_signed) {
          jp.SetPixelType(Isis::SignedWord);
        } else {
          jp.SetPixelType(Isis::UnsignedWord);
        }
      } else {
        throw iException::Message(iException::User, 
          "The file [" + ui.GetFilename("FROM") + "] contains unsupported data type.",
                              _FILEINFO_);
      }
      jp.SetInputFile(iString(ui.GetFilename("FROM")));
      jp.SetOutputCube("TO");
      jp.SetOrganization(ProcessImport::JP2);
      jp.StartProcess();
      jp.EndProcess();
    }
    catch (Isis::iException &e) {
      throw iException::Message(iException::User, 
       "The file [" + ui.GetFilename("FROM") + "] does not contain a recognized image format.",
                              _FILEINFO_);
    }
  } else {

    string mode = ui.GetString("MODE");
    if (mode == "AUTO") {
      if (qimage->isGrayscale())
        mode = "GRAYSCALE";
      else if (qimage->hasAlphaChannel())
        mode = "ARGB";
      else
        mode = "RGB";
    }

    if (mode == "GRAYSCALE") {
      Cube *outCube = p.SetOutputCube("TO", qimage->width(), qimage->height());
      Pvl *label = outCube->Label();
      PvlGroup bandBin ("BandBin");
      PvlKeyword name ("Name");
      name += "Gray";
      bandBin += name;
      label->AddGroup(bandBin);
      p.StartProcess(toGrayCube);
    }
    else if (mode == "RGB") {
      Cube *outCube = p.SetOutputCube("TO", qimage->width(), qimage->height(), 3);
      Pvl *label = outCube->Label();
      PvlGroup bandBin ("BandBin");
      PvlKeyword name ("Name");
      name += "Red";
      name += "Green";
      name += "Blue";
      bandBin += name;
      label->AddGroup(bandBin);
      p.StartProcess(toRGBCube);
    }
    else {
      Cube *outCube = p.SetOutputCube("TO", qimage->width(), qimage->height(), 4);
      Pvl *label = outCube->Label();
      PvlGroup bandBin ("BandBin");
      PvlKeyword name ("Name");
      name += "Red";
      name += "Green";
      name += "Blue";
      name += "Alpha";
      bandBin += name;
      label->AddGroup(bandBin);
      p.StartProcess(toARGBCube);
    }
 
    delete qimage;
    qimage = NULL;
    p.EndProcess();
  }
}

void toGrayCube (Buffer &out) {
  for(int sample = 0; sample < out.SampleDimension(); sample ++) {
    double pixel = qGray(qimage->pixel(sample, line));
    out[sample] = TestSpecial(pixel);
  }

  line ++; 
}

void toRGBCube (Buffer &out) {
  if (band == 0) {
    for(int sample = 0; sample < out.SampleDimension(); sample ++) { 
      double pixel = qRed(qimage->pixel(sample, line));
      out[sample] = TestSpecial(pixel);
    }
  }
  else if (band == 1) {
    for(int sample = 0; sample < out.SampleDimension(); sample ++) { 
      double pixel = qGreen(qimage->pixel(sample, line));
      out[sample] = TestSpecial(pixel);
    }
  }
  else if (band == 2) {
    for(int sample = 0; sample < out.SampleDimension(); sample ++) { 
      double pixel = qBlue(qimage->pixel(sample, line));
      out[sample] = TestSpecial(pixel);
    }
  }
  else {
    string msg = "RGB cubes must have exactly three bands.";
    throw iException::Message(iException::Programmer,msg,_FILEINFO_);
  }

  line++;
  if (line == qimage->height()) {
    line = 0;
    band++;
  }
}

void toARGBCube (Buffer &out) {
  if (band == 0) {
    for(int sample = 0; sample < out.SampleDimension(); sample ++) { 
      double pixel = qRed(qimage->pixel(sample, line));
      out[sample] = TestSpecial(pixel);
    }
  }
  else if (band == 1) {
    for(int sample = 0; sample < out.SampleDimension(); sample ++) { 
      double pixel = qGreen(qimage->pixel(sample, line));
      out[sample] = TestSpecial(pixel);
    }
  }
  else if (band == 2) {
    for(int sample = 0; sample < out.SampleDimension(); sample ++) { 
      double pixel = qBlue(qimage->pixel(sample, line));
      out[sample] = TestSpecial(pixel);
    }
  }
  else if (band == 3) {
    for(int sample = 0; sample < out.SampleDimension(); sample ++) { 
      double pixel = qAlpha(qimage->pixel(sample, line));
      out[sample] = TestSpecial(pixel);
    }
  }
  else {
    string msg = "ARGB cubes must have exactly four bands.";
    throw iException::Message(iException::Programmer,msg,_FILEINFO_);
  }

  line++;
  if (line == qimage->height()) {
    line = 0;
    band++;
  }
}

/** 
  * Tests the pixel. If it is valid it will return the dn value,
  * otherwise it will return the Isis special pixel value that
  * corresponds to it
  * 
  * @param pixel The double precision value that represents a
  *              pixel.
  * @return double  The double precision value representing the
  *         pixel will return as a valid dn or changed to an isis
  *         special pixel.
  */
  double TestSpecial(const double pixel){
    if (pixel <= null_max && pixel >= null_min){
      return Isis::NULL8;
    } 
    else if (pixel <= hrs_max && pixel >= hrs_min){
      return Isis::HIGH_REPR_SAT8;
    } 
    else if (pixel <= lrs_max && pixel >= lrs_min){
      return Isis::LOW_REPR_SAT8;
    } 
    else {
      return pixel;
    }
  }
