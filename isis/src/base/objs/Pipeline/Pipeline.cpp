#include <iostream>

#include "Pipeline.h"
#include "PipelineApplication.h"
#include "iException.h"
#include "Application.h"
#include "Preference.h"
#include "TextFile.h"
#include "Filename.h"

using namespace Isis;
using namespace std;

namespace Isis {


  /**
   * This is the one and only Pipeline constructor. This will initialize a 
   * pipeline object. 
   * 
   * @param procAppName This is an option parameter that gives the pipeline a name
   */
  Pipeline::Pipeline(const iString &procAppName) {
    p_procAppName = procAppName;
    p_addedCubeatt = false;
    p_outputListNeedsModifiers = false;
  }


  /**
   * This destroys the pipeline
   * 
   */
  Pipeline::~Pipeline(){
    for(int i = 0; i < (int)p_apps.size(); i++) {
      delete p_apps[i];
    }

    p_apps.clear();
  }


  /**
   * This method is the core of the pipeline class. This method tells each
   * PipelineApplication to learn about itself and calculate necessary filenames,
   * execution calls, etc... Pipeline error checking happens in here, so if a
   * Pipeline is invalid this method will throw the iException.
   * 
   */
  void Pipeline::Prepare() {
    // Nothing in the pipeline? quit
    if(p_apps.size() == 0) return;

    // We might have to modify the pipeline and try again, so keep track of if this is necessary
    bool successfulPrepare = false;

    while(!successfulPrepare) {
      // Assume we'll be successful
      successfulPrepare = true;
      bool foundFirst = false;

      // Keep track of whether or not we must remove virtual bands
      bool mustElimBands = false;

      // Look to see if we need to eliminate virtual bands...
      for(unsigned int i = 0; i < p_virtualBands.size(); i++) {
        mustElimBands |= !p_virtualBands[i].empty();
      }

      // Keep track of temp files for conflicts
      vector<iString> tmpFiles;
  
      // Loop through all the pipeline apps, look for a good place to remove virtual
      //   bands and tell the apps the prepare themselves. Double check the first program
      //   is not branched (expecting multiple inputs).
      for(int i = 0; i < (int)p_apps.size() && successfulPrepare; i++) {
        if(mustElimBands && p_apps[i]->SupportsVirtualBands()) {
          if(i != 0 && p_virtualBands.size() != 1) {
            iString message = "If multiple original inputs were set in the pipeline, the first application must support virtual bands.";
            throw iException::Message(iException::Programmer, message, _FILEINFO_);
          }

          p_apps[i]->SetVirtualBands(p_virtualBands);
          mustElimBands = false;
  
          // We might have added the "cubeatt" program to eliminate bands,
          //   remove it if we found something else to do the virtual bands.
          // **This causes a failure in our calculations, start over.
          if(p_addedCubeatt && i != (int)p_apps.size() - 1) {
            delete p_apps[p_apps.size() - 1];
            p_apps.resize(p_apps.size() - 1);
            p_appIdentifiers.resize(p_appIdentifiers.size() - 1);
            p_apps[p_apps.size() - 1]->SetNext(NULL);
            p_addedCubeatt = false;
            successfulPrepare = false;
            continue;
          }
        }
        else {
          // Pipeline is responsible for the virtual bands, reset any apps
          //   who have an old virtual bands setting.
          vector<iString> empty;
          p_apps[i]->SetVirtualBands(empty);
        }
  
        // This instructs the pipeline app to prepare itself; all previous pipeline apps must
        //   be already prepared. Future pipeline apps do not have to be.
        p_apps[i]->BuildParamString();
  
        // keep track of tmp files
        vector<iString> theseTempFiles = p_apps[i]->TemporaryFiles();
        for(int tmpFile = 0; tmpFile < (int)theseTempFiles.size(); tmpFile++) {
          tmpFiles.push_back(theseTempFiles[tmpFile]);
        }
  
        if(!foundFirst && p_apps[i]->Enabled()) {
          foundFirst = true;
  
          if(p_apps[i]->InputBranches().size() != OriginalBranches().size()) {
            string msg = "The program [" + p_apps[i]->Name() + "] can not be the first in the pipeline";
            msg += " because it must be run multiple times with unspecified varying inputs";
            throw iException::Message(iException::Programmer, msg, _FILEINFO_);
          }
        }
      }

      // Make sure we found an app!
      if(!foundFirst) {
        string msg = "No applications are enabled in the pipeline";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
  
      // Make sure all tmp files are unique!
      for(int i = 0; successfulPrepare && i < (int)tmpFiles.size(); i++) {
        for(int j = i+1; j < (int)tmpFiles.size(); j++) {
          if(tmpFiles[i] == tmpFiles[j]) {
            string msg = "There is a conflict with the temporary file naming. The temporary file [";
            msg += tmpFiles[i] + "] is created twice.";
            throw iException::Message(iException::Programmer, msg, _FILEINFO_);
          }
        }
      }
  
      // We failed at eliminating bands, add stretch to our programs and try again
      if(successfulPrepare && mustElimBands) {
        AddToPipeline("cubeatt", "~PIPELINE_RESERVED_FOR_BANDS~");
        Application("~PIPELINE_RESERVED_FOR_BANDS~").SetInputParameter("FROM", true);
        Application("~PIPELINE_RESERVED_FOR_BANDS~").SetOutputParameter("TO", "final");
        p_addedCubeatt = true;
        successfulPrepare = false;
      }

      if(p_apps[p_apps.size()-1]->GetOutputs().size() == 0) {
        string msg = "There are no outputted files in the pipeline. At least one program must generate an output file.";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
    }
  }


  /**
   * This method executes the pipeline. When you're ready to start your proc
   * program, call this method.
   */
  void Pipeline::Run(){
    // Prepare the pipeline programs
    Prepare();

    // Go through these programs, executing them
    for(int i = 0; i < Size(); i++) {
      if(Application(i).Enabled()) {
        // grab the sets of parameters this program needs to be run with
        const vector<iString> &params = Application(i).ParamString();
        for(int j = 0; j < (int)params.size(); j++) {

          // check for non-program run special strings
          iString special(params[j].substr(0,7));

          // If ">>LIST", then we need to make a list file
          if(special == ">>LIST ") {
            iString cmd = params[j].substr(7);
            iString listFilename = cmd.Token(" ");
            TextFile listFile(listFilename, "overwrite");

            while(!cmd.empty()) {
              listFile.PutLine(cmd.Token(" "));
            }

            listFile.Close();
          }
          else {
            // Nothing special is happening, just execute the program
            Isis::iApp->Exec(Application(i).Name(), params[j]);
          }
        }
      }
    }

    // Remove temporary files now
    if(!KeepTemporaryFiles()) {
      for(int i = 0; i < Size(); i++) {
        if(Application(i).Enabled()) {
          vector<iString> tmpFiles = Application(i).TemporaryFiles();
          for(int file = 0; file < (int)tmpFiles.size(); file++) {
            remove(tmpFiles[file].c_str());
          }
        }
      }
    }
  }


  /**
   * This method is used to set the original input file. This file is the first 
   * program's input, and the virtual bands will be taken directly from this 
   * parameter. 
   * 
   * @param inputParam The parameter to get from the user interface that contains 
   *                   the input file
   */
  void Pipeline::SetInputFile(const iString &inputParam) {
    UserInterface &ui = Application::GetUserInterface();
    p_originalInput.push_back(ui.GetFilename(inputParam));
    p_originalBranches.push_back(inputParam);
    p_virtualBands.push_back(ui.GetInputAttribute(inputParam).BandsStr());
  }


  /**
   * This method is used to set the original input file. No virtual bands will 
   * be read, and the inputFile parameter will be read as a path to a file instead 
   * of as a parameter. 
   * 
   * @param inputParam A filename object containing the location of the input file
   */
  void Pipeline::SetInputFile(const Filename &inputFile) {
    p_originalInput.push_back(inputFile.Expanded());
    p_originalBranches.push_back(inputFile.Expanded());
    p_virtualBands.push_back("");
  }


  /**
   * This method is used to set the original input files. These files are the 
   * first program's input, a branch will be added for every line in the file, and
   * the virtual bands will be taken directly from this parameter. 
   * 
   * @param inputParam The parameter to get from the user interface that contains 
   *                   the input file
   */
  void Pipeline::SetInputListFile(const iString &inputParam) {
    UserInterface &ui = Application::GetUserInterface();

    TextFile filelist( Filename(ui.GetFilename(inputParam)).Expanded() );
    string filename;
    int branch = 1;

    while(filelist.GetLineNoFilter(filename)) {
      p_originalInput.push_back(filename);
      p_originalBranches.push_back(inputParam + iString(branch));
      p_virtualBands.push_back("");
      p_finalOutput.push_back(Filename(filename).Name());

      branch ++;
    }

    p_outputListNeedsModifiers = true;
  }


  /**
   * This method is used to set the original input files. These files are the 
   * first program's input, a branch will be added for every line in the file.
   * 
   * @param inputParam The filename of the list file contains the input files
   */
  void Pipeline::SetInputListFile(const Filename &inputFilename) {
    TextFile filelist( inputFilename.Expanded() );
    string filename;
    int branch = 1;

    while(filelist.GetLineNoFilter(filename)) {
      p_originalInput.push_back(filename);
      p_originalBranches.push_back(Filename(inputFilename).Expanded() + " " + iString(branch));
      p_finalOutput.push_back(Filename(filename).Name());
      p_virtualBands.push_back("");

      branch ++;
    }

    p_outputListNeedsModifiers = true;
  }


  /**
   * This method is used to set the original input file. This file is the first 
   * program's input. 
   * 
   * @param inputParam The parameter to get from the user interface that contains 
   *                   the input file
   * @param virtualBandsParam The parameter to get from the user interface that 
   *                          contains the virtual bands list; internal default is
   *                          supported. Empty string if no virtual bands
   *                          parameter exists.
   */
  void Pipeline::SetInputFile(const iString &inputParam, const iString &virtualBandsParam) {
    UserInterface &ui = Application::GetUserInterface();
    p_originalInput.push_back(ui.GetAsString(inputParam));
    p_originalBranches.push_back(inputParam);

    if(!virtualBandsParam.empty() && ui.WasEntered(virtualBandsParam)) {
      p_virtualBands.push_back(ui.GetAsString(virtualBandsParam));
    }
    else {
      p_virtualBands.push_back("");
    }
  }


  /**
   * This method is used to set the final output file. If no programs generate 
   * output, the final output file will not be used. If the output file was not 
   * entered, one will be generated automatically and placed into the current 
   * working folder. 
   * 
   * @param outputParam The parameter to get from the user interface that contains 
   *                    the output file; internal default is supported.
   */
  void Pipeline::SetOutputFile(const iString &outputParam) {
    UserInterface &ui = Application::GetUserInterface();
    p_finalOutput.clear();

    if(ui.WasEntered(outputParam)) {
      p_finalOutput.push_back(ui.GetAsString(outputParam));
    }
  }


  /**
   * This method is used to set the final output file. If no programs generate 
   * output, the final output file will not be used. If the output file was not 
   * entered, one will be generated automatically and placed into the current 
   * working folder.
   * 
   * @param outputFile The filename of the output file; NOT the parameter name.
   */
  void Pipeline::SetOutputFile(const Filename &outputFile) {
    p_finalOutput.clear();
    p_finalOutput.push_back(outputFile.Expanded());
  }


  /**
   * This method is used to set an output list file. Basically, this means the 
   * output filenames are specified in the list file. Internal defaults/automatic 
   * name calculations are supported. 
   * 
   * @param outputFilenameList Parameter name containing the path to the output 
   *                           list file
   */
  void Pipeline::SetOutputListFile(const iString &outputFilenameParam) {
    UserInterface &ui = Application::GetUserInterface();

    if(ui.WasEntered(outputFilenameParam)) {
      SetOutputListFile(Filename(ui.GetFilename(outputFilenameParam)));
    }
    else {
      p_finalOutput.clear();

      // Calculate output files
      for(unsigned int i = 0; i < p_originalInput.size(); i++) {
        p_finalOutput.push_back(Filename(p_originalInput[i]).Name());
      }

      p_outputListNeedsModifiers = true;
    }
  }


  /**
   * This method is used to set an output list file. Basically, this means the 
   * output filenames are specified in the list file.  
   * 
   * @param outputFilenameList List file with output cube names
   */
  void Pipeline::SetOutputListFile(const Filename &outputFilenameList) {
    p_finalOutput.clear();

    TextFile filelist( outputFilenameList.Expanded() );
    string filename;

    while(filelist.GetLineNoFilter(filename)) {
      p_finalOutput.push_back(filename);
    }

    p_outputListNeedsModifiers = false;
  }


  /**
   * Set whether or not to keep temporary files (files generated in the middle of 
   * the pipeline that are neither the original input nor the final output). 
   * 
   * @param keep True means don't delete, false means delete. 
   */
  void Pipeline::KeepTemporaryFiles(bool keep) {
    p_keepTemporary = keep;
  }


  /**
   * Add a new program to the pipeline. This method must be called before calling 
   * Pipeline::Application(...) to access it. The string identifier will access
   * this program.
   * 
   * @param appname The name of the new application 
   * @param identifier The program's identifier for when calling 
   *                   Pipeline::Application
   */
  void Pipeline::AddToPipeline(const iString &appname, const iString &identifier) {
    // Check uniqueness first
    for(unsigned int appIdentifier = 0; appIdentifier < p_appIdentifiers.size(); appIdentifier++) {
      if(p_appIdentifiers[appIdentifier] == identifier) {
        iString message = "The application identifier [" + identifier + "] is not unique. " +
                          "Please providing a unique identifier";
        throw iException::Message(iException::Programmer, message, _FILEINFO_);
      }
    }

    // If we've got cubeatt on our list of applications for band eliminating, take it away temporarily
    PipelineApplication *cubeAtt = NULL;
    iString cubeAttId = "";
    if(p_addedCubeatt) {
      cubeAtt = p_apps[p_apps.size()-1];
      cubeAttId = p_appIdentifiers[p_appIdentifiers.size()-1];
      p_apps.resize(p_apps.size()-1);
      p_appIdentifiers.resize(p_appIdentifiers.size()-1);
      p_apps[p_apps.size()-1]->SetNext(NULL);
    }

    // Add the new application
    if(p_apps.size() == 0) {
      p_apps.push_back(new PipelineApplication(appname, this));
    }
    else {
      p_apps.push_back(new PipelineApplication(appname, p_apps[p_apps.size()-1]));
    }

    p_appIdentifiers.push_back(identifier);
    
    // If we have stretch, put it back where it belongs
    if(cubeAtt) {
      p_apps[p_apps.size()-1]->SetNext(cubeAtt);
      cubeAtt->SetPrevious(p_apps[p_apps.size()-1]);
      p_apps.push_back(cubeAtt);
      p_appIdentifiers.push_back(cubeAttId);
    }
  }


  /**
   * Add a new program to the pipeline. This method must be called before calling 
   * Pipeline::Application(...) to access it. The string used to access this 
   * program will be the program's name. 
   * 
   * @param appname The name of the new application
   */
  void Pipeline::AddToPipeline(const iString &appname) {
    // Check uniqueness first
    for(unsigned int appIdentifier = 0; appIdentifier < p_appIdentifiers.size(); appIdentifier++) {
      if(p_appIdentifiers[appIdentifier] == appname) {
        iString message = "The application identifier [" + appname + "] is not unique. Please use " +
                          "the other AddToPipeline method providing a unique identifier";
        throw iException::Message(iException::Programmer, message, _FILEINFO_);
      }
    }

    // If we've got cubeatt on our list of applications for band eliminating, take it away temporarily
    PipelineApplication *cubeAtt = NULL;
    iString cubeAttId = "";
    if(p_addedCubeatt) {
      cubeAtt = p_apps[p_apps.size()-1];
      cubeAttId = p_appIdentifiers[p_appIdentifiers.size()-1];
      p_apps.resize(p_apps.size()-1);
      p_appIdentifiers.resize(p_appIdentifiers.size()-1);
      p_apps[p_apps.size()-1]->SetNext(NULL);
    }

    // Add the new application
    if(p_apps.size() == 0) {
      p_apps.push_back(new PipelineApplication(appname, this));
    }
    else {
      p_apps.push_back(new PipelineApplication(appname, p_apps[p_apps.size()-1]));
    }

    p_appIdentifiers.push_back(appname);
    
    // If we have stretch, put it back where it belongs
    if(cubeAtt) {
      p_apps[p_apps.size()-1]->SetNext(cubeAtt);
      cubeAtt->SetPrevious(p_apps[p_apps.size()-1]);
      p_apps.push_back(cubeAtt);
      p_appIdentifiers.push_back(cubeAttId);
    }
  }


  /**
   * This is an accessor to get a specific PipelineApplication. This is the 
   * recommended accessor. 
   * 
   * @param identifier The identifier (usually name) of the application to access,
   *                such as "spiceinit"
   * 
   * @return PipelineApplication& The application's representation in the pipeline
   */
  PipelineApplication &Pipeline::Application(const iString &identifier) {
    int index = 0;
    bool found = false;

    while(!found && index < Size()) {
      if(p_appIdentifiers[index] == identifier) {
        found = true;
      }
      else {
        index ++;
      }
    }

    if(!found) {
      iString msg = "Application identified by [" + identifier + "] has not been added to the pipeline";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }

    return *p_apps[index];
  }


  /**
   * This is an accessor to get a specific PipelineApplication. This accessor is 
   * mainly in place for the output operator; it is not recommended. 
   * 
   * @param index The index of the pipeline application
   * 
   * @return PipelineApplication& The pipeline application
   */
  PipelineApplication &Pipeline::Application(const int &index) {
    if(index > Size()) {
      iString msg = "Index [" + iString(index) + "] out of bounds";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }

    return *p_apps[index];
  }


  /**
   * This method disables all applications up to this one. Effectively, the 
   * application specified becomes the first application. This is not a guarantee 
   * that the application specified is enabled, it only guarantees no applications
   * before it will be run. 
   * 
   * @param appname The program to start with
   */
  void Pipeline::SetFirstApplication(const iString &appname) {
    int appIndex = 0;
    for(appIndex = 0; appIndex < (int)p_apps.size() && p_apps[appIndex]->Name() != appname; appIndex++) {
      p_apps[appIndex]->Disable();
    }

    if(appIndex >= (int)p_apps.size()) {
      string msg = "Pipeline could not find application [" + appname + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * This method disables all applications after to this one. Effectively, the 
   * application specified becomes the last application. This is not a guarantee 
   * that the application specified is enabled, it only guarantees no applications
   * after it will be run. 
   * 
   * @param appname The program to end with
   */
  void Pipeline::SetLastApplication(const iString &appname) {
    int appIndex = p_apps.size() - 1;
    for(appIndex = p_apps.size() - 1; appIndex >= 0 && p_apps[appIndex]->Name() != appname; appIndex --) {
      p_apps[appIndex]->Disable();
    }

    if(appIndex < 0) {
      string msg = "Pipeline could not find application [" + appname + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * This gets the final output for the specified branch; this is necessary for 
   * the PipelineApplications to calculate the correct outputs to use. 
   * 
   * @param branch Branch index to get the final output for
   * @param addModifiers Whether or not to add the last name modifier
   * 
   * @return iString The final output string
   */
  iString Pipeline::FinalOutput(int branch, bool addModifiers) {
    iString output = ((p_finalOutput.size() != 0)? p_finalOutput[0] : "");

    if(p_apps.size() == 0) return output;

    if(p_finalOutput.size() > 1) {
      if((unsigned int)branch >= p_finalOutput.size()) {
        iString msg = "Output not set for branch [" + iString(branch) + "]";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }

      if(!p_outputListNeedsModifiers) {
        return p_finalOutput[branch];
      }
      else {
        output = p_finalOutput[branch];
        addModifiers = true;
      }
    }

    PipelineApplication *last = p_apps[p_apps.size()-1];
    if(!last->Enabled()) last = last->Previous();

    if(output == "" || p_finalOutput.size() > 1) {
      if(output == "") {
        output = "./" + Filename(p_originalInput[0]).Basename();
      }
      else {
        output = "./" + Filename(p_originalInput[branch]).Basename();
      }

      // Base filename off of first input file
      if(!addModifiers || last->OutputBranches().size() == 1) {
        if(addModifiers && p_finalOutput.size() > 1)
          output += "." + last->OutputNameModifier();

        output += "." + last->OutputExtension();
      }
      else {
        // If we have multuple final outputs, rely on them to
        //   differentiate the branches
        if(p_finalOutput.size() <= 1) {
          output += "." + last->OutputBranches()[branch];
        }

        if(addModifiers && p_finalOutput.size() > 1)
          output += "." + last->OutputNameModifier();
        
        output += "." + last->OutputExtension();
      }
    }
    else if(addModifiers) {
      PipelineApplication *last = p_apps[p_apps.size()-1];
      if(!last->Enabled()) last = last->Previous();
    
      output = Filename(p_finalOutput[0]).Path() + "/" +
               Filename(p_finalOutput[0]).Basename() + "." + 
               last->OutputBranches()[branch] + ".";

      if(p_finalOutput.size() > 1) {
        output += last->OutputNameModifier() + ".";
      }

      output += last->OutputExtension();
    }
    
    return output;
  }


  /**
   * This method returns the user's temporary folder for temporary files. It's 
   * simply a conveinient accessor to the user's preferences. 
   * 
   * @return iString The temporary folder
   */
  iString Pipeline::TemporaryFolder() {
    Pvl &pref = Preference::Preferences();
    return (string)pref.FindGroup("DataDirectory")["Temporary"];
  }


  /**
   * This method re-enables all applications. This resets the effects of
   * PipelineApplication::Disable, SetFirstApplication and SetLastApplication.
   * 
   */
  void Pipeline::EnableAllApplications() {
    for(int i = 0; i < Size(); i++) {
      p_apps[i]->Enable();
    }
  }


  /**
   * This is the output operator for a Pipeline, which enables things such as: 
   * @code 
   *   Pipeline p; 
   *   cout <<  p << endl;
   * @endcode
   * 
   * @param os The output stream (usually cout) 
   * @param pipeline The pipeline object to output
   * 
   * @return ostream& The modified output stream
   */
  ostream &operator<<(ostream &os, Pipeline &pipeline) {
    pipeline.Prepare();

    if(!pipeline.Name().empty()) {
      os << "PIPELINE -------> " << pipeline.Name() << " <------- PIPELINE" <<endl;
    }

    for(int i = 0; i < pipeline.Size(); i++) {
      if(pipeline.Application(i).Enabled()) {
        const vector<iString> &params = pipeline.Application(i).ParamString();
        for(int j = 0; j < (int)params.size(); j++) {
          iString special(params[j].substr(0,7));
          if(special == ">>LIST ") {
            iString cmd = params[j].substr(7);
            iString file = cmd.Token(" ");
            os << "echo " << cmd << " > " << file << endl;
          }
          else {
            os << pipeline.Application(i).Name() << " " << params[j] << endl;
          }
        }
      }
    }

    if(!pipeline.KeepTemporaryFiles()) {
      for(int i = 0; i < pipeline.Size(); i++) {
        if(pipeline.Application(i).Enabled()) {
          vector<iString> tmpFiles = pipeline.Application(i).TemporaryFiles();
          for(int file = 0; file < (int)tmpFiles.size(); file++) {
            os << "rm " << tmpFiles[file] << endl;
          }
        }
      }
    }

    if(!pipeline.Name().empty()) {
      os << "PIPELINE -------> " << pipeline.Name() << " <------- PIPELINE" <<endl;
    }

    return os;
  }
}
