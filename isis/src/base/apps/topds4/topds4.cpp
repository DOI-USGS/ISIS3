#include <iostream>

#include <inja/inja.hpp>

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



    json data;
    data["name"] = "world";
    render("Hello {{ name }}!", data); // Returns std::string "Hello world!"
    render_to(std::cout, "Hello {{ name }}!", data); // Writes "Hello world!" to stream




    std::cout << label << std::endl;
    return label.findGroup("Dimensions", Pvl::Traverse);
  }
}
