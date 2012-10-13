#include "Isis.h"

#include "ExportDescription.h"
#include "FileName.h"
#include "ImageExporter.h"
#include "UserInterface.h"

using namespace Isis;


int addChannel(ExportDescription &desc, IString param, IString mode);
void addResults(PvlGroup &results, ImageExporter *exporter,
    IString channel, int index);


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  IString format = ui.GetString("FORMAT");
  ImageExporter *exporter = ImageExporter::fromFormat(format);

  ExportDescription desc;
  if (ui.GetString("BITTYPE") == "8BIT")
    desc.setPixelType(UnsignedByte);
  else if (ui.GetString("BITTYPE") == "S16BIT")
    desc.setPixelType(SignedWord);
  else if (ui.GetString("BITTYPE") == "U16BIT")
    desc.setPixelType(UnsignedWord);

  int redIndex = -1;
  int greenIndex = -1;
  int blueIndex = -1;
  int alphaIndex = -1;

  IString mode = ui.GetString("MODE");
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

  FileName outputName = ui.GetFileName("TO");
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


int addChannel(ExportDescription &desc, IString param, IString mode) {
  UserInterface &ui = Application::GetUserInterface();
  FileName name = ui.GetFileName(param);
  CubeAttributeInput &att = ui.GetInputAttribute(param);

  int index = -1;
  if (mode != "GRAYSCALE" && ui.GetString("STRETCH") == "MANUAL") {
    IString bandId = param.substr(0, 1);
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
    IString channel, int index) {

  results += PvlKeyword(
      channel + "InputMinimum", exporter->getInputMinimum(index));
  results += PvlKeyword(
      channel + "InputMaximum", exporter->getInputMaximum(index));
}

