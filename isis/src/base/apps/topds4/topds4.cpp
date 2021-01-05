#include <fstream>
#include <iostream>
#include <time.h>

#include <inja/inja.hpp>

#include "md5wrapper.h"

#include "topds4.h"

#include "FileName.h"
#include "OriginalLabel.h"
#include "PvlToJSON.h"

using namespace std;
using namespace inja;
using json = nlohmann::json;


namespace Isis {

  void topds4(UserInterface &ui, Pvl *log) {
    Cube *icube = new Cube();
    icube->open(ui.GetFileName("FROM"));
    topds4(icube, ui);
  }

  void topds4(Cube *icube, UserInterface &ui, Pvl *log) {

    Process p;
    p.SetInputCube(icube);

    json dataSource;

    Pvl &cubeLabel = *icube->label();

    // Add the input cube PVL label to template engine data
    dataSource["MainLabel"].update(pvlToJSON(cubeLabel));

    // Add the original label (from an ingestion app) to the template engine data
    // Wrap it in an OriginalLabel so existing elements don't get overwritten
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

    std::cout << "=============Data================" << std::endl;
    std::cout << dataSource.dump(4) << std::endl;
    std::cout << "=================================" << std::endl;


    Environment env;
    // Call back functions
    /**
     * Renders to the current UTC time formatted as YYYY-MM-DDTHH:MM:SS
     */
    env.add_callback("currentTime", 0, [](Arguments& args) {
      time_t startTime = time(NULL);
      struct tm *tmbuf = gmtime(&startTime);
      char timestr[80];
      strftime(timestr, 80, "%Y-%m-%dT%H:%M:%S", tmbuf);
      return string(timestr);
    });

    /**
     * Renders to the filename of the output img
     */
    env.add_callback("imageFileName", 0, [icube](Arguments& args) {
      QString cubeFilename = icube->fileName().split("/").back();
      return (cubeFilename.split(".")[0] + ".img").toStdString();
    });

    /**
     * Renders to the MD5 hash for the input cube
     */
    env.add_callback("md5Hash", 0, [icube](Arguments& args) {
      md5wrapper md5;
      return md5.getHashFromFile(icube->fileName()).toStdString();
    });

    std::string inputTemplate = ui.GetFileName("TEMPLATE").toStdString();

    std::string result = env.render_file(inputTemplate, dataSource);

    std::ofstream outFile(ui.GetFileName("TO").toStdString());
    outFile << result;
    outFile.close();

    std::cout << "============Result===============" << std::endl;
    std::cout << result << std::endl;
    std::cout << "=================================" << std::endl;
  }
}
