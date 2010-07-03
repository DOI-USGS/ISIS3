#include "Isis.h"

#include <sstream>

#include "Pvl.h"
#include "History.h"
#include "iString.h"

using namespace Isis;
using namespace std;

void IsisMain(){

  // Get user entered file name & mode 
  UserInterface &ui = Application::GetUserInterface();
  string file = ui.GetFilename("FROM");
  string mode = ui.GetString("MODE");

  // Extract history from file
  History hist("IsisCube", file);
  Pvl pvl = hist.ReturnHist();

  // Print full history
  if(mode=="FULL"){
    if (ui.IsInteractive()) {
      Application::GuiLog(pvl);    
    }
    else {
      cout << pvl << endl;
    }
  }

  // Print brief history in command line form
  else if(mode=="BRIEF"){  
    for(int i=0; i<pvl.Objects(); ++i){
      string all = pvl.Object(i).Name() + " ";
      PvlGroup user = pvl.Object(i).FindGroup("UserParameters");
      for(int j=0; j<user.Keywords(); ++j){
        ostringstream os;
        os << user[j];
        string temp = os.str();
        int index = temp.find("=");
        iString temp1(temp.substr(0,index-1));
        string temp2 = temp.substr(index+2);
        all += temp1.DownCase()+ "=" + temp2 + " "; 
      } 
      if (ui.IsInteractive()) {
        Application::GuiLog(all);
      }
      else {
        cout << all << endl;
      }
    }   
  }
}
