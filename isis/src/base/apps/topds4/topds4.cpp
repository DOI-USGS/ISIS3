#include <fstream>
#include <iostream>

#include <inja/inja.hpp>

#include "topds4.h"

#include "FileName.h"
#include "OriginalLabel.h"
#include "PvlToJSON.h"

using namespace std;
using namespace inja;
using json = nlohmann::json;


namespace Isis {



  PvlGroup topds4(UserInterface &ui) {
    Cube *icube = new Cube();
    icube->open(ui.GetFileName("FROM"));
    return topds4(icube, ui);
  }


  PvlGroup topds4(Cube *icube, UserInterface &ui) {

    Process p;
    p.SetInputCube(icube);

    json dataSource;

    // We will need the main label even if it is not used as a template data source
    Pvl &cubeLabel = *icube->label();

    // Add the input cube PVL label to template engine data
    // *** TODO: make sure this label is inside a unique JSON element so nothing in it can 
    // conflict with other data sources
    if (ui.GetBoolean("MAINLABEL")) {
      dataSource["MainLabel"].update(pvlToJSON(cubeLabel));
    }
    
    // Add the original label (from an ingestion app) to the template engine data
    // Wrap it in an OriginalLabel so existing elements don't get overwritten
    // *** TODO: make sure this label is inside a unique JSON element so nothing in it can 
    // conflict with other data sources
    if (ui.GetBoolean("ORIGINALLABEL")) {
      if (cubeLabel.hasObject("OriginalLabel")) {
        OriginalLabel origBlob;
        icube->read(origBlob);
        Pvl origLabel;
        origLabel = origBlob.ReturnLabels();
        dataSource["OriginalLabel"].update(pvlToJSON(origLabel));
      }
      else if (cubeLabel.hasObject("OriginalXmlLabel")) {
        // get the xml label and add it to the template data
      }
    }
    
    std::cout << "=================================" << std::endl;
    std::cout << dataSource.dump(4) << std::endl;
    std::cout << "=================================" << std::endl;

    Environment env;
    std::string inputTemplate = ui.GetFileName("TEMPLATE").toStdString();
    std::string result = env.render_file(inputTemplate, dataSource);

    std::ofstream outFile(ui.GetFileName("TO").toStdString());
    outFile << result;
    outFile.close();

    std::cout << result << std::endl;

    // TODO: return something useful
    return cubeLabel.findGroup("Dimensions", Pvl::Traverse);
  }
}
