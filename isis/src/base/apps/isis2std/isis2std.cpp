#include "Isis.h"

#include "ExportDescription.h"
#include "Filename.h"
#include "ImageExporter.h"
#include "UserInterface.h"

using namespace Isis;


void addChannel(ExportDescription &desc, iString param, iString mode);
void addResults(PvlGroup &results, ImageExporter *exporter,
    iString channel, int index);


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  iString format = ui.GetString("FORMAT");
  ImageExporter *exporter = ImageExporter::fromFormat(format);

  ExportDescription desc;
  if (ui.GetString("BITTYPE") == "8BIT")
    desc.setPixelType(UnsignedByte);
  else if (ui.GetString("BITTYPE") == "S16BIT")
    desc.setPixelType(SignedWord);
  else if (ui.GetString("BITTYPE") == "U16BIT")
    desc.setPixelType(UnsignedWord);

  int redIndex;
  int greenIndex;
  int blueIndex;
  int alphaIndex;

  iString mode = ui.GetString("MODE");
  if (mode == "GRAYSCALE") {
    addChannel(desc, "FROM", mode);
    exporter->setGrayscale(desc);
  }
  else {
    redIndex = addChannel(desc, "RED", mode);
    greenIndex = addChannel(desc, "GREEN", mode);
    blueIndex = addChannel(desc, "BLUE", mode);

    if (mode == "RGBA") {
      alphaIndex = addChannel(desc, "ALPHA", mode);
      exporter->setRgba(desc);
    }
    else {
      exporter->setRgb(desc);
    }
  }

  Filename outputName = ui.GetFilename("TO");
  int quality = ui.GetInteger("QUALITY");
  exporter->write(outputName, quality);

  if (mode != "GRAYSCALE" && ui.GetString("STRETCH") != "MANUAL") {
    ui.Clear("MINIMUM");
    ui.Clear("MAXIMUM");

    ui.PutDouble("RMIN", exporter->getInputMinimum(redIndex));
    ui.PutDouble("RMAX", exporter->getInputMaximum(redIndex));
    ui.PutDouble("GMIN", exporter->getInputMinimum(greenIndex));
    ui.PutDouble("GMAX", exporter->getInputMaximum(greenIndex));
    ui.PutDouble("BMIN", exporter->getInputMinimum(blueIndex));
    ui.PutDouble("BMAX", exporter->getInputMaximum(blueIndex));

    if (mode == "ARGB") {
      ui.PutDouble("AMIN", exporter->getInputMinimum(alphaIndex));
      ui.PutDouble("AMAX", exporter->getInputMaximum(alphaIndex));
    }
  }

  // Write out the results
  PvlGroup results("Results");
  results += PvlKeyword("OutputFilename", outputName.Expanded());

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


void addChannel(ExportDescription &desc, iString param, iString mode) {
  UserInterface &ui = Application::GetUserInterface();
  Filename name = ui.GetFilename(param);
  CubeAttributeInput &att = ui.GetInputAttribute(param);

  if (mode != "GRAYSCALE" && ui.GetString("STRETCH") == "MANUAL") {
    iString bandId = param.substr(0, 1);
    double min = ui.GetDouble(bandId + "MIN");
    double max = ui.GetDouble(bandId + "MAX");

    desc.addChannel(name, att, min, max);
  }
  else {
    desc.addChannel(name, att);
  }
}


void addResults(PvlGroup &results, ImageExporter *exporter,
    iString channel, int index) {

  results += PvlKeyword(
      channel + "InputMinimum", exporter->getInputMinimum(index));
  results += PvlKeyword(
      channel + "InputMaximum", exporter->getInputMaximum(index));
}

