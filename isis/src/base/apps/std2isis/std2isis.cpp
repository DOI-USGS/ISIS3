#include "Isis.h"

#include <QString>

#include "ImageImporter.h"
#include "UserInterface.h"

using namespace Isis;


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  Filename inputName = ui.GetFilename("FROM");
  ImageImporter *importer = ImageImporter::fromFilename(inputName);

  // Explicitly set band dimension if a specific color mode is desired
  iString mode = ui.GetString("MODE");
  if (mode != "AUTO") {
    int bands = mode == "GRAYSCALE" ? 1 : mode == "ARGB" ? 4 : 3;
    importer->setBands(bands);
  }

  // Set special pixel ranges
  if (ui.GetBoolean("SETNULLRANGE"))
    importer->setNullRange(ui.GetDouble("NULLMIN"), ui.GetDouble("NULLMAX"));
  if (ui.GetBoolean("SETHRSRANGE"))
    importer->setHrsRange(ui.GetDouble("HRSMIN"), ui.GetDouble("HRSMAX"));
  if (ui.GetBoolean("SETLRSRANGE"))
    importer->setLrsRange(ui.GetDouble("LRSMIN"), ui.GetDouble("LRSMAX"));

  // Import the image
  Filename outputName = ui.GetFilename("TO");
  CubeAttributeOutput &att = ui.GetOutputAttribute("TO");
  importer->import(outputName, att);

  delete importer;
}

