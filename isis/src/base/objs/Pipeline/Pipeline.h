#ifndef Pipeline_h
#define Pipeline_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.4 $                                                             
 * $Date: 2008/12/19 21:13:06 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       

#include <vector>

#include "iString.h"
#include "PipelineApplication.h"

using namespace std;

namespace Isis {
  class Filename;

/** 
 * This class is designed to do most of the work in the "proc" programs, such as 
 * thmproc and mocproc. This object works by setting the initial input, final 
 * output, and every program in between. It is suggested that you "cout" this 
 * object in order to debug you're usage of the class. 
 *  
 * Here's an example usage of this class: 
 * @code 
 * UserInterface &ui = Application::GetUserInterface();
 * Pipeline p("YourAppName");
 *
 * p.SetInputFile("FROM", "BANDS");
 * p.SetOutputFile("TO");
 *
 * p.KeepTemporaryFiles(!ui.GetBoolean("REMOVE"));
 *
 * p.AddToPipeline("mission2isis");
 * p.Application("mission2isis").SetInputParameter("FROM", false);
 * p.Application("mission2isis").SetOutputParameter("TO", "raw");

 * p.AddToPipeline("spiceinit");
 * p.Application("spiceinit").SetInputParameter("FROM", false);
 * p.Application("spiceinit").AddParameter("PCK", "PCK");
 * p.Application("spiceinit").AddParameter("CK", "CK");
 *
 * p.AddToPipeline("cam2map");
 * p.Application("cam2map").SetInputParameter("FROM", true);
 * p.Application("cam2map").SetOutputParameter("TO", "lev2");
 * p.Application("cam2map").AddParameter("MAP", "MAP");
 * p.Application("cam2map").AddParameter("PIXRES", "RESOLUTION");

 * if(ui.WasEntered("PIXRES")) {
 *   p.Application("cam2map").AddConstParameter("PIXRES", "MPP");
 * }

 * if(ui.GetBoolean("INGESTION")) {
 *   p.SetFirstApplication("mission2isis");
 * }
 * else{
 *   p.SetFirstApplication("spiceinit");
 * }

 * if(ui.GetBoolean("MAPPING")) {
 *   p.SetLastApplication("cam2map");
 * }
 * else {
 *   p.SetLastApplication("spiceinit");
 * } 
 *  
 * p.Run(); 
 * 
 * @endcode
 *  
 * @author 2008-08-04 Steven Lambright 
 *  
 * @internal 
 *   @history 2008-09-25 Added features: Application identifiers other than the
 *            application names, branched original input, branching from
 *            branches, partial branch merging (discontinuing branches*)
 *   @history 2008-10-28 The input no longer has to have virtual bands if the
 *            SetInputFile(iString,iString) has an empty parameter name for the
 *            virtual bands parameter. SetInputListFile(...) method added.
 *   @history 2008-12-19 List files are now fully supported, along with output
 *            list files.
 */
  class Pipeline {
    public:
      Pipeline(const iString &procAppName = "");
      ~Pipeline();

      void Prepare();
      void Run();

      void SetInputFile(const iString &inputParam);
      void SetInputFile(const iString &inputParam, const iString &virtualBandsParam);
      void SetInputListFile(const iString &inputParam);
      void SetInputFile(const Filename &inputFilename);
      void SetInputListFile(const Filename &inputFilename);

      void SetOutputFile(const iString &outputParam);
      void SetOutputFile(const Filename &outputFile);
      void SetOutputListFile(const iString &outputFilenameParam);
      void SetOutputListFile(const Filename &outputFilenameList);
      void KeepTemporaryFiles(bool keep);
      //! Returns true if temporary files will not be deleted, false if they will
      bool KeepTemporaryFiles() { return p_keepTemporary; }

      void AddToPipeline(const iString &appname);
      void AddToPipeline(const iString &appname, const iString &identifier);
      PipelineApplication &Application(const iString &identifier);
      PipelineApplication &Application(const int &index);

      void SetFirstApplication(const iString &appname);
      void SetLastApplication(const iString &appname);

      friend ostream &operator<<(ostream &os, Pipeline &pipeline);

      //! Returns the name of the pipeline
      iString Name() const { return p_procAppName; }
      //! Returns the number of applications in the pipeline
      int Size() const { return (int)p_apps.size(); }

      /**
       * Returns the initial input file for the pipeline
       * 
       * @param branch Branch of the original input to get the filename from
       * 
       * @return iString Name of the original input file
       */
      iString OriginalInput(unsigned int branch) { return ((branch < p_originalInput.size())? p_originalInput[branch] : ""); }

      //! Returns the names of the original branches of the pipeline (multiple input files)
      vector<iString> OriginalBranches() { return p_originalBranches; }
      iString FinalOutput(int branch = 0, bool addModifiers = true);
      iString TemporaryFolder();

      void EnableAllApplications();

    private:
      iString p_procAppName; //!< The name of the pipeline
      vector<iString> p_originalInput; //!< The original input file
      vector<iString> p_originalBranches; //!< The original branch names
      vector<iString> p_finalOutput; //!< The final output file (empty if needs calculated)
      vector<iString> p_virtualBands; //!< The virtual bands string
      bool p_keepTemporary; //!< True if keeping temporary files
      bool p_addedCubeatt; //!< True if the "cubeatt" program was added
      vector< PipelineApplication * > p_apps; //!< The pipeline applications
      vector< iString > p_appIdentifiers; //!< The strings to identify the pipeline applications
      bool p_outputListNeedsModifiers;
  };
};

#endif
