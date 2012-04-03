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

