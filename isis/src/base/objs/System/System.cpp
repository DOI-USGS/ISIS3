/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2008/07/09 17:59:17 $
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch 
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website, 
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */                                                                     

#include <fstream>
#include <stdlib.h>
#include <string>

#include "System.h"
#include "Filename.h"
#include "iString.h"
#include "iException.h"
#include <PvlKeyword.h>

using namespace std;
namespace Isis {

  /** 
   * Runs the command contained in the argument
   * 
   * @param command Command to be executed
   * 
   * @throws Isis::iException::Programmer
   */
  void System (string &command) {
    int status = system (command.c_str());

    if (status != 0) {
      string msg = "Unable to execute [" + command +
                   "] return status [" + Isis::iString(status) + "]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
    }

    return;
  }

  /**
   * Runs various system specific uname commands and returns the results
   * 
   * @return PvlGroup containing uname information
   */
  PvlGroup GetUnameInfo () {
    // Create a temporary file to store console output to
    Isis::Filename temp;
    temp.Temporary("/tmp/UnameConsoleInfo","txt");
    std::string tempFile = temp.Expanded();

    // Uname commands output to temp file with each of the following
    // values on its own line in this order:
    // machine hardware name, processor, hardware platform name,
    // operating system, kernel name, kernel version, kernel release, all
    Isis::PvlGroup unameGroup("UNAME");
    std::ifstream readTemp;

    #if defined(__linux__)
    // Write uname outputs to temp file
    string uname1 = "uname -m > " + tempFile;
    string uname2 = "uname -p >> " + tempFile;
    string uname3 = "uname -i >> " + tempFile;
    string uname4 = "uname -o >> " + tempFile;
    string uname5 = "uname -s >> " + tempFile;
    string uname6 = "uname -v >> " + tempFile;
    string uname7 = "uname -r >> " + tempFile;
    string uname8 = "uname -a >> " + tempFile;
    System (uname1);
    System (uname2);
    System (uname3);
    System (uname4);
    System (uname5);
    System (uname6);
    System (uname7);
    System (uname8);
    // Read data from temp file
    char value[256];
    readTemp.open(tempFile.c_str(), ifstream::in);
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("MachineHardware", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("Processor", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("HardwarePlatform", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("OperatingSystem", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("KernelName", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("KernelVersion", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("KernelRelease", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("FullUnameString", value));

    #elif defined(__APPLE__)
    // Write uname outputs to temp file
    string uname1 = "uname -m > " + tempFile;
    string uname2 = "uname -p >> " + tempFile;
    string uname3 = "uname -s >> " + tempFile;
    string uname4 = "uname -v >> " + tempFile;
    string uname5 = "uname -r >> " + tempFile;
    string uname6 = "uname -a >> " + tempFile;
    System (uname1);
    System (uname2);
    System (uname3);
    System (uname4);
    System (uname5);
    System (uname6);
    // Read data from temp file
    char value[256];
    readTemp.open(tempFile.c_str(), ifstream::in);
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("MachineHardware", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("Processor", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("OperatingSystem", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("OperatingSystemVersion", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("OperatingSystemRelease", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("FullUnameString", value));

    #elif defined (__sun__)
    // Write uname outputs to temp file
    string uname1 = "uname -m > " + tempFile;
    string uname2 = "uname -p >> " + tempFile;
    string uname3 = "uname -i >> " + tempFile;
    string uname4 = "uname -s >> " + tempFile;
    string uname5 = "uname -v >> " + tempFile;
    string uname6 = "uname -r >> " + tempFile;
    string uname7 = "uname -a >> " + tempFile;
    System (uname1);
    System (uname2);
    System (uname3);
    System (uname4);
    System (uname5);
    System (uname6);
    System (uname7);
    // Read data from temp file
    char value[256];
    readTemp.open(tempFile.c_str(), ifstream::in);
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("MachineHardware", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("Processor", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("HardwarePlatform", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("OperatingSystem", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("OperatingSystemVersion", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("OperatingSystemRelease", value));
    readTemp.getline(value,256);
    unameGroup.AddKeyword(Isis::PvlKeyword("FullUnameString", value));
    #endif

    // remove temp file and return
    std::string cleanup = "rm -f " + tempFile;
    System (cleanup);
    return unameGroup;
  }

  /**
   * Runs some printenv commands that return Isis related Enviroment Variables.
   * 
   * @return PvlGroup containing Enviroment information 
   * @todo Replace printenv commands with c library getenv 
   * @todo  
   */
  Isis::PvlGroup GetEnviromentInfo (){
    // Create a temporary file to store console output to
    Isis::Filename temp;
    temp.Temporary("/tmp/EnviromentInfo","txt");
    std::string tempFile = temp.Expanded();
    Isis::PvlGroup envGroup("EnviromentVariables");
    std::ifstream readTemp;

    string env1 = "printenv SHELL > " + tempFile;
    string env2 = "printenv HOME >> " + tempFile;
    string env3 = "printenv PWD >> " + tempFile;
    string env4;
    #if defined(__APPLE__)
    env4 = "printenv DYLD_LIBRARY_PATH >> " + tempFile;
    #else 
    env4 = "printenv LD_LIBRARY_PATH >> " + tempFile;
    #endif
    string env5 = "printenv ISISROOT >> " + tempFile;
    string env6 = "printenv ISIS3DATA >> " + tempFile;
    System (env1);
    System (env2);
    System (env3);
    System (env4);
    System (env5);
    System (env6);
    // Read data from temp file
    char value[511];
    readTemp.open(tempFile.c_str(), ifstream::in);
    readTemp.getline(value,255);
    envGroup.AddKeyword(Isis::PvlKeyword("Shell", value));
    readTemp.getline(value,255);
    envGroup.AddKeyword(Isis::PvlKeyword("Home", value));
    readTemp.getline(value,255);
    envGroup.AddKeyword(Isis::PvlKeyword("Pwd", value));
    readTemp.getline(value,511);
    #if defined(__APPLE__)
    envGroup.AddKeyword(Isis::PvlKeyword("DYLDLibraryPath", value));
    #else 
    envGroup.AddKeyword(Isis::PvlKeyword("LDLibraryPath", value));
    #endif
    readTemp.getline(value,255);
    envGroup.AddKeyword(Isis::PvlKeyword("ISISROOT", value));
    readTemp.getline(value,255);
    envGroup.AddKeyword(Isis::PvlKeyword("ISIS3DATA", value));

    // remove temp file and return
    std::string cleanup = "rm -f " + tempFile;
    System (cleanup);
    return envGroup;
  }

  /**
   * Runs df to see the disk space availability
   * 
   * @return Isis::iString containing df results
   */
  Isis::iString SystemDiskSpace (){
    Isis::Filename temp;
    temp.Temporary("/tmp/SystemDiskSpace","txt");
    std::string tempFile = temp.Expanded();
    std::ifstream readTemp;
    string diskspace = "df > " + tempFile;
    System (diskspace);
    readTemp.open(tempFile.c_str(), ifstream::in);

    iString results = "";
    char tmp[512];
    while(!readTemp.eof()){
      readTemp.getline(tmp,512);
      results += tmp;
      results += "\n";
    }

    // remove temp file and return
    std::string cleanup = "rm -f " + tempFile;
    System (cleanup);
    return results;
  }

  /**
   * Runs ldd on linux and sun and otool on macs to get information about the applicaiton run
   * 
   * @return Isis::iString containing application information
   */
  Isis::iString GetLibraryDependencies (const std::string &file){
    Isis::Filename temp;
    temp.Temporary("/tmp/LibraryDependencies","txt");
    std::string tempFile = temp.Expanded();
    std::ifstream readTemp;
    string dependencies = "";
    #if defined(__linux__)
    dependencies = "ldd -v " + file + " > " + tempFile;
    #elif defined(__APPLE__)
    dependencies = "otool -L " + file + " > " + tempFile;
    #elif defined (__sun__)
    dependencies = "ldd -v " + file + " > " + tempFile;
    #endif
    System (dependencies);
    readTemp.open(tempFile.c_str(), ifstream::in);

    iString results = "";
    char tmp[512];
    while(!readTemp.eof()){
      readTemp.getline(tmp,512);
      results += tmp;
      results += "\n";
    }

    // remove temp file and return
    std::string cleanup = "rm -f " + tempFile;
    System (cleanup);
    return results;
  }
} // end namespace isis
