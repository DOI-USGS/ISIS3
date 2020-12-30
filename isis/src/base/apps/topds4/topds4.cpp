#include <iostream>
#include <time.h>

#include <inja/inja.hpp>

#include "md5wrapper.h"

#include "topds4.h"

using namespace std;
using namespace inja;
using json = nlohmann::json;


namespace Isis {



  PvlGroup topds4(UserInterface &ui) {
    Cube *icube = new Cube();
    icube->open(ui.GetFileName("FROM"));
    return topds4(icube, ui);
  }


  PvlGroup topds4(Cube *cube, UserInterface &ui) {
    Process p;
    p.SetInputCube(cube);

    Pvl &label = *cube->label();

    Environment renderEnv;

    // Call back functions
    /**
     * Renders to the current UTC time formatted as YYYY-MM-DDTHH:MM:SS
     */
    renderEnv.add_callback("currentTime", 0, [](Arguments& args) {
      time_t startTime = time(NULL);
      struct tm *tmbuf = gmtime(&startTime);
      char timestr[80];
      strftime(timestr, 80, "%Y-%m-%dT%H:%M:%S", tmbuf);
      return string(timestr);
    });

    /**
     * Renders to the filename of the output img
     */
    renderEnv.add_callback("imageFileName", 0, [cube](Arguments& args) {
      QString cubeFilename = cube->fileName().split("/").back();
      return (cubeFilename.split(".")[0] + ".img").toStdString();
    });

    /**
     * Renders to the MD5 hash for the input cube
     */
    renderEnv.add_callback("md5Hash", 0, [cube](Arguments& args) {
      md5wrapper md5;
      return md5.getHashFromFile(cube->fileName()).toStdString();
    });

    json data;
    data["name"] = "world";
    renderEnv.render("Hello {{ name }}!", data); // Returns std::string "Hello world!"
    renderEnv.write(renderEnv.parse("{{ currentTime() }}\n"), data, ui.GetFileName("TO").toStdString());
    // renderEnv.write(renderEnv.parse("{{ imageFileName() }}\n"), data, ui.GetFileName("TO").toStdString());
    // renderEnv.write(renderEnv.parse("{{ md5Hash() }}\n"), data, ui.GetFileName("TO").toStdString());
    renderEnv.render_to(std::cout, renderEnv.parse("{{ currentTime() }}\n"), data);
    renderEnv.render_to(std::cout, renderEnv.parse("{{ imageFileName() }}\n"), data);
    renderEnv.render_to(std::cout, renderEnv.parse("{{ md5Hash() }}\n"), data);

    // std::cout << label << std::endl;
    return label.findGroup("Dimensions", Pvl::Traverse);
  }
}
