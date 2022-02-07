#include "Isis.h"

#include "ExportDescription.h"
#include "FileName.h"
#include "ImageExporter.h"
#include "UserInterface.h"

using namespace Isis;
using namespace std;


int addChannel(ExportDescription &desc, QString param, QString mode);
void addResults(PvlGroup &results, ImageExporter *exporter,
    QString channel, int index);


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  QString format = ui.GetString("FORMAT");
  ImageExporter *exporter = ImageExporter::fromFormat(format);

  ExportDescription desc;
  if (ui.GetString("BITTYPE") == "8BIT") {
    desc.setPixelType(UnsignedByte);
  }
  else if (ui.GetString("BITTYPE") == "U16BIT") {
    desc.setPixelType(UnsignedWord);
  }
  else { //if (ui.GetString("BITTYPE") == "S16BIT")
    desc.setPixelType(SignedWord);
  }

  int redIndex = -1;
  int greenIndex = -1;
  int blueIndex = -1;
  int alphaIndex = -1;

  QString mode = ui.GetString("MODE");
  if (mode == "GRAYSCALE") {
    addChannel(desc, "FROM", mode);
    exporter->setGrayscale(desc);
  }
  else {
    redIndex = addChannel(desc, "RED", mode);
    greenIndex = addChannel(desc, "GREEN", mode);
    blueIndex = addChannel(desc, "BLUE", mode);

    if (mode == "ARGB") {
      alphaIndex = addChannel(desc, "ALPHA", mode);
      exporter->setRgba(desc);
    }

    else {
      exporter->setRgb(desc);
    }
  }

  FileName outputName = ui.GetFileName("TO");
  int quality = ui.GetInteger("QUALITY");


  QString compression;
  if (format == "TIFF") {
    compression = ui.GetString("COMPRESSION").toLower();

  }
  else {
    compression = "none";
  }

  exporter->write(outputName, quality, compression);

  if (mode != "GRAYSCALE" && ui.GetString("STRETCH") != "MANUAL") {
    ui.Clear("MINIMUM");
    ui.Clear("MAXIMUM");

    ui.PutDouble("RMIN", exporter->inputMinimum(redIndex));
    ui.PutDouble("RMAX", exporter->inputMaximum(redIndex));
    ui.PutDouble("GMIN", exporter->inputMinimum(greenIndex));
    ui.PutDouble("GMAX", exporter->inputMaximum(greenIndex));
    ui.PutDouble("BMIN", exporter->inputMinimum(blueIndex));
    ui.PutDouble("BMAX", exporter->inputMaximum(blueIndex));

    if (mode == "ARGB") {
      ui.PutDouble("AMIN", exporter->inputMinimum(alphaIndex));
      ui.PutDouble("AMAX", exporter->inputMaximum(alphaIndex));
    }
  }

  // Write out the results
  PvlGroup results("Results");
  results += PvlKeyword("OutputFileName", outputName.expanded());

  if (mode == "GRAYSCALE") {
    addResults(results, exporter, "", 0);
  }
  else {
    addResults(results, exporter, "Red", redIndex);
    addResults(results, exporter, "Green", greenIndex);
    addResults(results, exporter, "Blue", blueIndex);

    if (mode == "ARGB") addResults(results, exporter, "Alpha", alphaIndex);
  }

  Application::Log(results);

  delete exporter;
}


int addChannel(ExportDescription &desc, QString param, QString mode) {
  UserInterface &ui = Application::GetUserInterface();
  FileName name = ui.GetCubeName(param);
  CubeAttributeInput &att = ui.GetInputAttribute(param);

  int index = -1;
  if (mode != "GRAYSCALE" && ui.GetString("STRETCH") == "MANUAL") {
    QString bandId = param.mid(0, 1);
    double min = ui.GetDouble(bandId + "MIN");
    double max = ui.GetDouble(bandId + "MAX");

    index = desc.addChannel(name, att, min, max);
  }
  else {
    index = desc.addChannel(name, att);
  }

  return index;
}


void addResults(PvlGroup &results, ImageExporter *exporter,
    QString channel, int index) {

  results += PvlKeyword(channel + "InputMinimum", toString(exporter->inputMinimum(index)));
  results += PvlKeyword(channel + "InputMaximum", toString(exporter->inputMaximum(index)));
}

