#include "Application.h"
#include "UserInterface.h"
#include "System.h"
#include "FileList.h"
#include "iString.h"

using namespace std; 
using namespace Isis;

void MakeCompletion(const string &appName);
string GetParamCompletion(int grp, int param);
string BuildStaticCompletes(std::string paramList);
void PrintHelp();

int main (int argc, char *argv[]) {
  if(argc < 2) {
    PrintHelp();
    return 0;
  }

  // Too slow if in debug mode
#ifndef CWDEBUG
  for(int i = 1; i < argc; i++) {
    MakeCompletion(argv[i]);
  }
#endif

  return 0;
}

void PrintHelp() {
  cout << "This program is used to set up the isis tab-completion for the tcsh shell. ";
  cout << "The output of this program is a list of commands for the isis setup script to execute." << endl;
  cout << "Usage: isiscomplete isisappname [isisappname2 isisappname3 ...]" << endl;
}

void MakeCompletion(const string &appName) {
  static char *argv[2];
  static int argc(2);
  argv[0] = (char*)appName.c_str();
  argv[1] = "-nogui";

  // Do not complete image-viewer applications
  if(appName.compare("qview") == 0) {
    cout << "complete qview 'c/-/(new)/'; ";
    return;
  }

  if(appName.compare("qnet") == 0) return;

  if(appName.compare("isisui") == 0) {
    string binPath = Filename("$ISISROOT/bin").Expanded();
    cout << "complete isisui 'n@*@F:" << binPath << "/@'; ";
    return;
  }

  // Do not complete self
  if(appName.compare("isiscomplete") == 0) return;

  Application app(argc,argv);
  UserInterface &ui = Application::GetUserInterface();
  string paramList = "";
  string completeCommand;
  vector<string> paramDetails;

  for(int grp = 0; grp < ui.NumGroups(); grp++) {
    for(int param = 0; param < ui.NumParams(grp); param++) {
      paramList += " " + iString(ui.ParamName(grp, param)).DownCase();
      paramDetails.push_back(GetParamCompletion(grp, param));
    }
  }

  for(unsigned int param = 0; param < paramDetails.size(); param ++) {
    completeCommand += "'" + paramDetails[param] + "' ";
  }

  completeCommand += " " + BuildStaticCompletes(paramList) + " ";
  completeCommand += "'n/*/(-" + paramList + ")/='";
  completeCommand = "complete " + appName + " " + completeCommand + "; ";
  cout << completeCommand;
}

string BuildStaticCompletes(std::string paramList) {
  string completion = "";

  // Batchlist
  completion += " 'c/-[bB][aA][tT][cC][hH][lL][iI][sS][tT]=/f/'";

  // Errlist
  completion += " 'c/-[eE][rR][rR][lL][iI][sS][tT]=/f/'";

  // Help
  completion += " 'c/-[hH][eE][lL][pP]=/(" + paramList + ")/'";

  // Info
  completion += " 'c/-[iI][nN][fF][oO]=/f/'";

  // Log
  completion += " 'c/-[lL][oO][gG]=/f/'";

  // Onerror
  completion += " 'c/-[oO][nN][eE][rR][rR][oO][rR]=/(abort continue)/'";

  // Preference
  completion += " 'c/-[pP][rR][eE][fF][eE][rR][eE][nN][cC][eE]=/f/'";

  // Restore
  completion += " 'c/-[rR][eE][sS][tT][oO][rR][eE]=/f:*/'";

  // Save
  completion += " 'c/-[sS][aA][vV][eE]=/f/'";

  completion += " 'c/-/(batchlist= errlist= gui nogui help help= info info= last log log=";
  completion += " onerror= preference= restore= save save= verbose webhelp)//'";

  return completion;
}

string GetParamCompletion(int grp, int param) {
  string completion = "c/";
  UserInterface &ui = Application::GetUserInterface();


  string paramName = ui.ParamName(grp, param);
  for(unsigned int curIndex = 0; curIndex < paramName.length(); curIndex++) {
    if(isalpha(paramName[curIndex])) {
      completion += "[";
      completion += toupper(paramName[curIndex]);
      completion += tolower(paramName[curIndex]);
      completion += "]";
    }
    else {
      completion += paramName[curIndex];
    }
  }
  completion += "=/";

  string type = ui.ParamType(grp, param);

  if(type == "cube") {
    completion += "f:*.[cC][uU][bB]";
  }
  else if(type == "filename") {
    completion += "f";
  }
  else {
    completion += "(" + ui.ParamDefault(grp,param) + ")";
  }

  completion += "/";

  return completion;
}
