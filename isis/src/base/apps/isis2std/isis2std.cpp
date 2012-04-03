#include "Isis.h"

#include "ExportDescription.h"
#include "Filename.h"
#include "ImageExporter.h"
#include "UserInterface.h"

using namespace Isis;


void addChannel(ExportDescription &desc, iString param, iString mode);


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

  iString mode = ui.GetString("MODE");
  if (mode == "GRAYSCALE") {
    addChannel(desc, "FROM", mode);
    exporter->setGrayscale(desc);
  }
  else {
    addChannel(desc, "RED", mode);
    addChannel(desc, "GREEN", mode);
    addChannel(desc, "BLUE", mode);

    if (mode == "RGBA") {
      addChannel(desc, "ALPHA", mode);
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

    ui.PutDouble("RMIN", exporter->getInputMinimum(0));
    ui.PutDouble("RMAX", exporter->getInputMaximum(0));
    ui.PutDouble("GMIN", exporter->getInputMinimum(1));
    ui.PutDouble("GMAX", exporter->getInputMaximum(1));
    ui.PutDouble("BMIN", exporter->getInputMinimum(2));
    ui.PutDouble("BMAX", exporter->getInputMaximum(2));

    if (mode == "ARGB") {
      ui.PutDouble("AMIN", exporter->getInputMinimum(3));
      ui.PutDouble("AMAX", exporter->getInputMaximum(3));
    }
  }

  // Write out the results
  PvlGroup results("Results");
  results += PvlKeyword("OutputFilename", outputName.Expanded());

  if (mode == "GRAYSCALE") {
    results += PvlKeyword("InputMinimum", exporter->getInputMinimum(0));
    results += PvlKeyword("InputMaximum", exporter->getInputMaximum(0));
  }
  else {
    int redIndex = 0;
    int greenIndex = 1;
    int blueIndex = 2;
    int alphaIndex = 3;

    results += PvlKeyword(
        "RedInputMinimum", exporter->getInputMinimum(redIndex));
    results += PvlKeyword(
        "RedInputMaximum", exporter->getInputMaximum(redIndex));
    results += PvlKeyword(
        "GreenInputMinimum", exporter->getInputMinimum(greenIndex));
    results += PvlKeyword(
        "GreenInputMaximum", exporter->getInputMaximum(greenIndex));
    results += PvlKeyword(
        "BlueInputMinimum", exporter->getInputMinimum(blueIndex));
    results += PvlKeyword(
        "BlueInputMaximum", exporter->getInputMaximum(blueIndex));

    if (mode == "ARGB") {
      results += PvlKeyword(
          "AlphaInputMinimum", exporter->getInputMinimum(alphaIndex));
      results += PvlKeyword(
          "AlphaInputMaximum", exporter->getInputMaximum(alphaIndex));
    }
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

