#include <iostream>
#include <string>
#include <sstream>
#include "IsisAml.h"
#include "iException.h"
#include "Filename.h"
#include "Pvl.h"
#include "Preference.h"


using namespace std;
class Inheritor : public IsisAml {
  public:
    Inheritor (const char *xmlfile);
    ~Inheritor ();
};

Inheritor::Inheritor (const char *xmlfile): IsisAml (xmlfile) {

  try {
    cout << "--------- Tests for private data ----------" << endl;

    cout << "  App name = " << name << endl;
    cout << "  App brief = " << brief << endl;
    cout << "  App description = " << description << endl;

    // Print categories
    for (unsigned int l=0; l<categorys.size(); l++) {
      cout << "  category[" << l << "] = " << categorys[l] << endl;
    }

    // Loop for each group
    for (unsigned int g=0; g<groups.size(); g++) {
      cout << "  group " << g << " name = " << groups[g].name << endl;

      // Loop for each parameter in the group
      for (unsigned int p=0; p<groups[g].parameters.size(); p++) {
        cout << "    parameter " << p << ", name = " <<
                groups[g].parameters[p].name << endl;
        cout << "      type = " << groups[g].parameters[p].type << endl;
        cout << "      brief = " << groups[g].parameters[p].brief << endl;
        cout << "      description = " <<
                groups[g].parameters[p].description << endl;
        cout << "      internal def = " <<
                groups[g].parameters[p].internalDefault << endl;
        // Loop for each helper in the parameter
        cout << "      helpers = " << groups[g].parameters[p].helpers.size() 
             << endl;
        for (unsigned int h=0; h<groups[g].parameters[p].helpers.size(); h++) {
          cout << "        name = " << groups[g].parameters[p].helpers[h].name << endl;
          cout << "        brief = " << groups[g].parameters[p].helpers[h].brief << endl;
          cout << "        description = " << groups[g].parameters[p].helpers[h].description << endl;
          cout << "        function = " << groups[g].parameters[p].helpers[h].function << endl;
          cout << "        icon = " << groups[g].parameters[p].helpers[h].icon << endl;

        }
        cout << "      count = " << groups[g].parameters[p].count << endl;
        cout << "      minimum = " << groups[g].parameters[p].minimum << endl;
        cout << "      minimum inclusive = " <<
                groups[g].parameters[p].minimum_inclusive << endl;
        cout << "      maximum = " << groups[g].parameters[p].maximum << endl;
        cout << "      maximum inclusive = " <<
                groups[g].parameters[p].maximum_inclusive << endl;
        cout << "      filter = " << groups[g].parameters[p].filter << endl;
        cout << "      file mode = " << groups[g].parameters[p].fileMode << endl;
        cout << "      odd = " << groups[g].parameters[p].odd << endl;

        // Print the current values
        cout << "      Values:" << endl;
        for (unsigned int l=0; l<groups[g].parameters[p].values.size(); l++) {
          cout << "        [" << l << "] = " <<
                  groups[g].parameters[p].values[l] << endl;
        }

        // Print the default values
        cout << "      Default Values:" << endl;
        for (unsigned int l=0; l<groups[g].parameters[p].defaultValues.size(); l++) {
          cout << "        [" << l << "] = " <<
                  groups[g].parameters[p].defaultValues[l] << endl;
        }

        // Print the parameters which this one must be greater than
        cout << "      Greater Than:" << endl;
        for (unsigned int l=0; l<groups[g].parameters[p].greaterThan.size(); l++) {
          cout << "        [" << l << "] = " <<
                  groups[g].parameters[p].greaterThan[l] << endl;
        }

        // Print the parameters which this one must be greater than or equal
        cout << "      Greater or Equal Than:" << endl;
        for (unsigned int l=0; l<groups[g].parameters[p].greaterThanOrEqual.size(); l++) {
          cout << "        [" << l << "] = " <<
                  groups[g].parameters[p].greaterThanOrEqual[l] << endl;
        }

        // Print the parameters which this one must be less than
        cout << "      Less Than:" << endl;
        for (unsigned int l=0; l<groups[g].parameters[p].lessThan.size(); l++) {
          cout << "        [" << l << "] = " <<
                  groups[g].parameters[p].lessThan[l] << endl;
        }

        // Print the parameters which this one must be less than or equal
        cout << "      Less Than or Equal:" << endl;
        for (unsigned int l=0; l<groups[g].parameters[p].lessThanOrEqual.size(); l++) {
          cout << "        [" << l << "] = " <<
                  groups[g].parameters[p].lessThanOrEqual[l] << endl;
        }

        // Print the parameters which this one must not be equal to
        cout << "      Not equal to:" << endl;
        for (unsigned int l=0; l<groups[g].parameters[p].notEqual.size(); l++) {
          cout << "        [" << l << "] = " <<
                  groups[g].parameters[p].notEqual[l] << endl;
        }

        // Print the parameters which must be included if this parameter is used
        cout << "      Include parameters:" << endl;
        for (unsigned int l=0; l<groups[g].parameters[p].include.size(); l++) {
          cout << "        [" << l << "] = " <<
                  groups[g].parameters[p].include[l] << endl;
        }

        // Print the parameters which must NOT be included if this parameter is used
        cout << "      Exclude parameters:" << endl;
        for (unsigned int l=0; l<groups[g].parameters[p].exclude.size(); l++) {
          cout << "        [" << l << "] = " <<
                  groups[g].parameters[p].exclude[l] << endl;
        }

        // Print the parameter's list options
        cout << "      List data:" << endl;
        for (unsigned int l=0; l<groups[g].parameters[p].listOptions.size(); l++) {
          cout << "        value [" << l << "] = " <<
                  groups[g].parameters[p].listOptions[l].value << endl;
          cout << "        brief [" << l << "] = " <<
                  groups[g].parameters[p].listOptions[l].brief << endl;
          cout << "        description [" << l << "] = " <<
                  groups[g].parameters[p].listOptions[l].description << endl;
            for (unsigned int e=0; e<groups[g].parameters[p].listOptions[l].exclude.size(); e++) {
              cout << "          exclude = " <<
              groups[g].parameters[p].listOptions[l].exclude[e] << endl;
          }
        }
      }
    }
  }
  catch (Isis::iException &error) {
    error.Report(false);
  }
}


int main (void)
{
  Isis::Preference::Preferences(true);
  // Create the aml object
  cout << "Create the aml object" << endl;
  Inheritor *aml;
  string xmlFile = Isis::Filename("./unitTest.xml").Expanded();
  try {
    aml = new Inheritor(xmlFile.c_str());
  }
  catch (Isis::iException &error) {
    error.Report (false);
    exit (1);
  }


  // Test public members
  try {
    cout << "Application information" << endl;

    cout << "Program name : " << aml->ProgramName () << endl;
    cout << "Brief description: " << aml->Brief() << endl;
    cout << "Full description: " << aml->Description() << endl;
    cout << "Version date: " << aml->Version() << endl << endl;

    cout << "Number of parameter groups : " << aml->NumGroups () << endl << endl;
    cout << "Parameter information:" << endl;
    for (int g=0; g<aml->NumGroups(); g++) {
      cout << "  Group number: " << g << endl;
      cout << "  Group name : " << aml->GroupName (g) << endl;
      for (int p=0; p<aml->NumParams(g); p++) {
        cout << "    Parameter number: " << p << endl;
        cout << "      Name: " << aml->ParamName(g,p) << endl;
        cout << "      Type: " << aml->ParamType(g,p) << endl;
        cout << "      Brief: " << aml->ParamBrief(g,p) << endl;
        cout << "      Default: " << aml->ParamDefault(g,p) << endl;
        cout << "      Internal default: " << aml->ParamInternalDefault(g,p) << endl;
        cout << "      Filter: " << aml->ParamFilter(g,p) << endl;
        cout << "      File Mode: " << aml->ParamFileMode(g,p) << endl;
        cout << "      Helper Information:" << endl;
        for (int h=0; h<aml->HelpersSize(g,p); h++) {
          cout << "        Name: " << aml->HelperButtonName(g,p,h) << endl;
          cout << "        Brief: " << aml->HelperBrief(g,p,h) << endl;
          cout << "        Description: " << aml->HelperDescription(g,p,h) << endl;
          cout << "        Function: " << aml->HelperFunction(g,p,h) << endl;
          cout << "        Icon: " << aml->HelperIcon(g,p,h) << endl;
        }
        cout << "      List Information:" << endl;
        for (int o=0; o<aml->ParamListSize (g,p); o++) {
          cout << "        Value: " << aml->ParamListValue (g,p,o) << endl;
          cout << "        Brief: " << aml->ParamListBrief (g,p,o) << endl;
          cout << "        Description: " << aml->ParamListDescription(g,p,o) << endl;
          cout << "        List exclusions: " << endl;
          for (int e=0; e<aml->ParamListExcludeSize (g,p,o); e++) {
            cout << "          Exclude parameter: " << aml->ParamListExclude (g,p, o,e) << endl;
          }
          cout << "        List inclusions: " << endl;
          for (int i=0; i<aml->ParamListIncludeSize (g,p,o); i++) {
            cout << "          Include parameter: " << aml->ParamListInclude (g,p, o,i) << endl;
          }
        }
      }
    }

    cout << "Get/Put/Clear/WasEntered tests" << endl;
    cout << "Default value of G0P1 is " << aml->GetFilename ("G0P1") << endl;
    cout << "G0P1 WasEntered value " << aml->WasEntered ("G0P1") << endl;
    aml->PutAsString ("G0P1", "/home/user/file1.cub");
    cout << "G0P1 WasEntered value " << aml->WasEntered ("G0P1") << endl;
    cout << "The value of G0P1 is " << aml->GetFilename ("G0P1") << endl;
    aml->Clear ("G0P1");
    cout << "Default value of G0P1 is " << aml->GetFilename ("G0P1") << endl;
    aml->PutFilename ("G0P1", "/home/user/file2.dat");
    cout << "The value of G0P1 is " << aml->GetFilename ("G0P1") << endl << endl;
    cout << "The value of G0P1 is " << aml->GetFilename ("G0P1","txt") << endl << endl;
    aml->Clear ("G0P1");

    cout << "Default value of G1P1 is " << aml->GetInteger("G1P1") << endl;
    aml->PutInteger ("G1P1", 33);
    cout << "The value of G1P1 is " << aml->GetInteger("G1P1") << endl << endl;
    aml->Clear ("G1P1");

    cout << "Default value of G1P2 is " << aml->GetDouble("G1P2") << endl;
    aml->PutDouble ("G1P2", 0.000000001);
    cout << "The value of G1P2 is " << aml->GetDouble("G1P2") << endl << endl;
    aml->Clear ("G1P2");

    cout << "Default value of G1P0 is " << aml->GetString("G1P0") << endl;
    aml->PutString ("G1P0", "G1p0L1");
    cout << "The value of G1P0 is " << aml->GetString("G1P0") << endl << endl;
    aml->Clear ("G1P0");

    aml->PutBoolean ("G6P0", true);
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");
    aml->PutBoolean ("G6P0", false);
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");

    aml->PutAsString ("G6P0", "TRUE");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");
    aml->PutAsString ("G6P0", "FALSE");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");
    aml->PutAsString ("G6P0", "true");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");
    aml->PutAsString ("G6P0", "false");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");

    aml->PutAsString ("G6P0", "YES");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");
    aml->PutAsString ("G6P0", "NO");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");
    aml->PutAsString ("G6P0", "yes");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");
    aml->PutAsString ("G6P0", "no");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");

    aml->PutAsString ("G6P0", "T");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");
    aml->PutAsString ("G6P0", "F");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");
    aml->PutAsString ("G6P0", "t");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");
    aml->PutAsString ("G6P0", "f");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");

    aml->PutAsString ("G6P0", "Y");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");
    aml->PutAsString ("G6P0", "N");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");
    aml->PutAsString ("G6P0", "y");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");
    aml->PutAsString ("G6P0", "n");
    cout << "The value of G6P0 is " << aml->GetBoolean("G6P0") << endl << endl;
    aml->Clear ("G6P0");

    cout << "Exact and partial name match tests:" << endl;
    cout << "  FROM's value = " << aml->GetString ("from") << endl;
    cout << "  FROM1's value = " << aml->GetString ("from1") << endl;
    cout << "  FR's value = " << aml->GetString ("fr") << endl;
    cout << endl;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  cout << endl;

  // Test the error catching for public members

  cout << "---------- Test error throwing ----------";

  try {
    cout << "  PutAsString:" << endl;
    try {
      aml->PutAsString ("G1P0", "11111");
      aml->PutAsString ("G1P0", "22222");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
    aml->Clear ("G1P0");
  
    cout << endl << "  PutString:" << endl;
    try {
      aml->PutString ("G1P0", "G1P0L0");
      aml->PutString ("G1P0", "22222");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
    aml->Clear ("G1P0");
  
    cout << endl;
    try {
      aml->PutString ("G2P4", "xxxxxx");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    cout << endl << "  PutFilename:" << endl;
    try {
      aml->PutFilename ("G0P0", "xxxxxxx");
      aml->PutFilename ("G0P0", "yyyyyyy");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
    aml->Clear ("G0P0");
  
    cout << endl;
    try {
      aml->PutFilename ("G2P4", "xxxxxx");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    cout << endl << "  Cube tests:" << endl;
    try {
      aml->PutFilename ("CUBE1", "xxxxxxx.cub+1,2-4");
      Isis::CubeAttributeInput &att = aml->GetInputAttribute("CUBE1");
      cout << "    " << att.BandsStr() << endl;
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
    aml->Clear ("CUBE1");
  
    try {
      aml->PutFilename ("CUBE2", "yyyyyyy.cub+8-bit+BSQ+detached");
      Isis::CubeAttributeOutput &att = aml->GetOutputAttribute("CUBE2");
      string strng;
      att.Write(strng);
      cout << "    Att string  = " << strng << endl;
      cout << "    File format = " << att.FileFormatStr() << endl;
      cout << "    Pixel type  = " << Isis::PixelTypeName(att.PixelType()) << endl;
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
    aml->Clear ("CUBE2");
  
  
  
  
    cout << endl << "  PutInteger:" << endl;
    try {
      aml->PutInteger ("G6P2", 1);
      aml->PutInteger ("G6P2", 1);
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
    aml->Clear ("G6P2");
  
    cout << endl;
    try {
      aml->PutInteger ("G6P0", 1);
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    cout << endl << "  PutDouble:" << endl;
    try {
      aml->PutDouble ("G1P2", 1.0);
      aml->PutDouble ("G1P2", 1.0);
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
    aml->Clear ("G1P2");
  
    cout << endl;
    try {
      aml->PutDouble ("G0P0", 1.0);
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
  
    cout << endl << "  PutBoolean:" << endl;
    try {
      aml->PutBoolean ("G6P0", true);
      aml->PutBoolean ("G6P0", false);
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
    aml->Clear ("G6P0");
  
    cout << endl;
    try {
      aml->PutBoolean ("G0P0", false);
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    cout << endl << "  GetAsString:" << endl;
    try {
      string s = aml->GetAsString ("G2P0");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    cout << endl << "  GetFilename:" << endl;
    try {
      string s = aml->GetFilename ("G0P0");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    cout << endl;
    try {
      string s = aml->GetFilename ("G2P4");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
  #if 0
    cout << endl << "  GetCube:" << endl;
    try {
      string s = aml->GetCube ("CUBE1");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    cout << endl;
    try {
      string s = aml->GetCube ("G2P4");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  #endif
  
    cout << endl << "  GetString:" << endl;
    try {
      string s = aml->GetString ("G6P3");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    cout << endl;
    try {
      string s = aml->GetString ("G2P4");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    cout << endl << "  GetInteger:" << endl;
    try {
      aml->GetInteger ("G2P0");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    cout << endl;
    try {
      aml->GetInteger ("G0P0");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    cout << endl << "  GetDouble:" << endl;
    try {
      aml->GetDouble ("G1P3");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    cout << endl;
    try {
      aml->GetDouble ("G0P1");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    cout << endl << "  GetBoolean:" << endl;
    try {
      aml->GetBoolean ("G6P0");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    try {
      aml->PutAsString("G6P0", "cccc");
      aml->GetBoolean ("G6P0");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
    aml->Clear ("G6P0");
  
    cout << endl;
    try {
      aml->GetBoolean ("G1P1");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    try {
      string s = aml->GetString("F");
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }
  
    cout << endl;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  // Load up a valid set of parameters

  cout << "---------- Validate a correctly loaded set of parameters ----------" << endl;
  aml->Clear ("G0P0");
  aml->PutAsString ("G0P0", "FILE1");

  aml->Clear ("G0P1");
  aml->PutAsString ("G0P1", "FILE2");

  aml->Clear ("G1P0");
  aml->PutAsString ("G1P0", "G1P0L0");

  aml->Clear ("G1P1");

  aml->Clear ("G1P2");

  aml->Clear ("G1P3");
  aml->PutAsString ("G1P3", "1.1");

  aml->Clear ("G2P0");

  aml->Clear ("G2P1");
  aml->PutAsString ("G2P1", "1");

  aml->Clear ("G2P2");
  aml->PutAsString ("G2P2", "3");

  aml->Clear ("G2P3");
  aml->PutAsString ("G2P3", "4");

  aml->Clear ("G2P4");
  aml->PutAsString ("G2P4", "5");

  aml->Clear ("G3P0");

  aml->Clear ("G3P1");
  aml->PutAsString ("G3P1", "1.2");

  aml->Clear ("G3P2");
  aml->PutAsString ("G3P2", "1.3");

  aml->Clear ("G3P3");
  aml->PutAsString ("G3P3", "1.4");

  aml->Clear ("G3P4");
  aml->PutAsString ("G3P4", "1.5");

  aml->Clear ("G4P0");
  aml->PutAsString ("G4P0", "1.6");

  aml->Clear ("G4P1");
  aml->PutAsString ("G4P1", "6");

  aml->Clear ("G4P2");
  aml->PutAsString ("G4P2", "6.7");

  aml->Clear ("G4P3");
  aml->PutAsString ("G4P3", "7");

  aml->Clear ("G4P4");
  aml->PutAsString ("G4P4", "7.8");

  aml->Clear ("G4P5");
  aml->PutAsString ("G4P5", "8");

  aml->Clear ("G5P0");
  aml->PutAsString ("G5P0", "9");

  aml->Clear ("G5P1");
  aml->PutAsString ("G5P1", "10");

  aml->Clear ("G5P2");
  aml->PutAsString ("G5P2", "11");

  aml->Clear ("G5P3");

  aml->Clear ("G6P0");
  aml->PutAsString ("G6P0", "yes");

  aml->Clear ("G6P1");
  aml->PutAsString ("G6P1", "13");

  aml->Clear ("G6P2");

  aml->Clear ("G6P3");
  aml->PutAsString ("G6P3", "STRING2");

  aml->Clear ("FROM");
  aml->PutAsString ("FROM", "STRING3");

  aml->Clear ("FROM1");
  aml->PutAsString ("FROM1", "STRING4");

  aml->Clear ("FR");
  aml->PutAsString ("FR", "STRING4");

  aml->Clear ("CUBE1");
  aml->PutAsString ("CUBE1", "CUBE.DAT");

  aml->Clear ("CUBE2");
  aml->PutAsString ("CUBE2", "CUBE2.DAT");

  try {
    aml->VerifyAll();
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  cout << endl;

  // Test the command line member
  cout << "The current command line:" << endl;
  try {
    Isis::Pvl lab;
    aml->CommandLine(lab);
    cout << lab << endl << endl;
  }
  catch (Isis::iException &error) {
    error.Report(false);
  }

  cout << endl;

  cout << "--- Check for NO value in an option/list/included parameter ---" << endl;
  aml->Clear ("G1P0");
  aml->PutAsString ("G1P0", "G1P0L1X");
  aml->Clear ("G1P3");
  try {
    aml->VerifyAll();
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  aml->Clear("G1P0");
  aml->PutAsString ("G1P3", "1.1");
  aml->PutAsString ("G1P0", "G1P0L0");
  cout << endl;

  cout << "----- Check for value in an option/list/excluded parameter -----" << endl;
  aml->PutAsString ("G2P0", "0");
  try {
    aml->VerifyAll();
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  aml->Clear("G2P0");
  cout << endl;

  cout << "---------- Check error for unknow parameter ----------" << endl;
  try {
    aml->Clear ("xyz");
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
}

