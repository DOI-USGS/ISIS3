#include <iostream>

#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

#include "CubeAttribute.h"
#include "FileName.h"
#include "XmlToJson.h"
#include "ProcessImport.h"

#include "isisimport.h"

using namespace std;
using namespace inja;
using json = nlohmann::json;

namespace Isis {

  void isisimport(UserInterface &ui, Pvl *log) {
    FileName xmlFileName = ui.GetFileName("FROM");

    // Convert xml file to json so inja can use it
    json pds4Data = xmlToJson(xmlFileName.toString());

    // std::cout << pds4Data.dump(4);
    std::string inputTemplate = ui.GetFileName("TEMPLATE").toStdString();

    Environment env;

    // Use inja to get number of lines, samples, and bands from the input PDS4 label
    std::string result = env.render_file(inputTemplate, pds4Data);

    // Turn this into a Pvl label
    Pvl newLabel;
    newLabel.fromString(result);

    // To read the DN data
    ProcessImport importer;
    importer.SetInputFile(xmlFileName.removeExtension().addExtension("img").expanded());

    // Set everything needed by ProcessImport
    // TODO: cassis-specific
    int ns = toInt(QString::fromStdString(pds4Data["Product_Observational"]["File_Area_Observational"]["Array_2D_Image"]["Axis_Array"][0]["elements"]));
    int nl = toInt(QString::fromStdString(pds4Data["Product_Observational"]["File_Area_Observational"]["Array_2D_Image"]["Axis_Array"][1]["elements"]));
    int nb = 1; 
    importer.SetDimensions(ns, nl, nb);

    importer.SetPixelType(PixelTypeEnumeration("Real"));
    importer.SetByteOrder(ByteOrderEnumeration("Lsb"));
    importer.SetBase(0.0);
    importer.SetMultiplier(1.0);
    importer.SetFileHeaderBytes(0); 

    CubeAttributeOutput &att = ui.GetOutputAttribute("TO");
    Cube *outputCube = importer.SetOutputCube(ui.GetFileName("TO"), att);

    importer.StartProcess();

    // Write the updated label 
    Isis::PvlObject &newCubeLabel = newLabel.findObject("IsisCube");
    Isis::Pvl &outLabel(*outputCube->label());
    Isis::PvlObject &outCubeLabel = outLabel.findObject("IsisCube");

    for(int g = 0; g < newCubeLabel.groups(); g++) {
      outCubeLabel.addGroup(newCubeLabel.group(g));
    }
    importer.EndProcess();

    return;
  }
}
