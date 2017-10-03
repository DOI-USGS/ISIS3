#include "Isis.h"

#include <QString>

#include "Cube.h"
#include "Preference.h"

#include "ProcessExportPds4.h"

using namespace std;
using namespace Isis;

/**
 * @author 2017-05-30 Marjorie Hahn
 */
void IsisMain() {
  Preference::Preferences(true);
  
  try {
    Isis::ProcessExportPds4 p;
    
    QString cubeName = "$tgo/testData/CAS-MCO-2016-11-26T22.32.39.582-BLU-03025-00.cub";
    
    Isis::Cube cub; 
    cub.open(cubeName, "r"); 
    
    p.SetInputCube(&cub);
    
    // Remove the schema from the lable because we cannot ensure that
    // attributes come out in the same order every time
    std::cout << p.StandardPds4Label().toString().remove(QRegExp("xmlns.*\""));
             
    std::ofstream ofs;
    p.OutputLabel(ofs);

    p.StartProcess(ofs);

    std::cout << p.GetLabel().toString().remove(QRegExp("xmlns.*\""));

    p.WritePds4("temp.img");
    remove("temp.img");
    remove("temp.xml");

    try {
      std::cout << "Test creating a standard Pds4Label with no input" << std::endl;
      ProcessExportPds4 emptyProcess;
      emptyProcess.StandardPds4Label();
    }
    catch(Isis::IException &e) {
      e.print();
    }
  }
  catch(Isis::IException &e) {
    e.print();
  }
}
