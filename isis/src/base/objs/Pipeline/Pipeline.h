#ifndef Pipeline_h
#define Pipeline_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>

#include <QString>

#include "PipelineApplication.h"

namespace Isis {
  class FileName;

  /**
   * This class helps to call other Isis Applications in a Pipeline. This object
   * works by creating a Pipeline and setting the initial input, final
   * output of the Pipeline which are got from the user interface. Applications
   * are added to the Pipeline and parameters relevant to the application are set
   * using relevant APIs'.
   *
   * The Pipeline will control the flow of calls to the different applications in
   * the Pipeline in First In First Out fashion. Also the first and last application
   * in the Pipeline can be explicitly set. Pipeline automatically calculates the
   * input to an application from the output of the previous application.
   *
   * The Pipeline branches automatically for a list file with multiple input files
   * or user can explicitly create branches from the Pipeline directly or from any
   * application in the Pipeline. The branches are required when an application
   * generates multiple outputs. Branches may be disabled / enabled and the Pipeline
   * will figure out the input for an application whose previous app in the same
   * branch was disabled.
   *
   * Parameters for an application in a branch can be set commonly for all branches
   * or explicitly for a branch by specifiying the branch name.
   *
   * Option continue allows the pipeline to proceed with the execution inspite of any
   * exceptions encountered while running an application in the pipeline. If continue
   * is not enabled, then on encountering  an exception the pipeline exits.
   *
   * Temporary files are created and will be deleted when explicitly set by the user.
   *
   * The Pipeline calls cubeatt app inherently if virtual bands are true.
   *
   * It is suggested that you "cout" this object in order to debug you're usage of
   * the class.
   *
   * Refer to Applications thmproc, mocproc, hidestripe, hiproc, hinoise2 for Pipeline usage.
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
   * // The app "thm2isis" generates multiple outputs for a single input.
   * // Hence the branches have to be created to process the outputs odd, even.
   * p.AddToPipeline("thm2isis");
   * p.Application("thm2isis").SetInputParameter("FROM", false);
   * p.Application("thm2isis").SetOutputParameter("TO", "raw");
   * p.Application("thm2isis").AddBranch("even", PipelineApplication::ConstantStrings);
   * p.Application("thm2isis").AddBranch("odd", PipelineApplication::ConstantStrings);

   * p.AddToPipeline("spiceinit");
   * p.Application("spiceinit").SetInputParameter("FROM", false);
   * p.Application("spiceinit").AddParameter("PCK", "PCK");
   * p.Application("spiceinit").AddParameter("CK", "CK");
   *
   * p.AddToPipeline("cam2map");
   * p.Application("cam2map").SetInputParameter("FROM", true);
   * p.Application("cam2map").SetOutputParameter("TO", "lev2");
   * p.Application("cam2map").AddParameter("even", "MAP", "MAP");
   * p.Application("cam2map").AddParameter("even", "PIXRES", "RESOLUTION");

   * if(ui.WasEntered("PIXRES")) {
   *   p.Application("cam2map").AddConstParameter("even", "PIXRES", "MPP");
   * }

   * p.Application("cam2map").AddParameter("odd", "MAP", PipelineApplication::LastOutput);
   * p.Application("cam2map").AddConstParameter("odd", "PIXRES", "MAP");
   * p.Application("cam2map").AddConstParameter("odd", "DEFAULTRANGE", "MAP");

   * if(ui.WasEntered("PIXRES")) {
   *   p.Application("cam2map").AddConstParameter("PIXRES", "MPP");
   * }

   * if(ui.GetBoolean("INGESTION")) {
   *   p.SetFirstApplication("thm2isis");
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
   *   @history 2008-09-25 Unknown - Added features: Application identifiers
   *                           other than the application names, branched
   *                           original input, branching from branches, partial
   *                           branch merging (discontinuing branches*)
   *   @history 2008-10-28 Unknown - The input no longer has to have virtual
   *                           bands if the SetInputFile(QString,QString) has an
   *                           empty parameter name for the virtual bands
   *                           parameter. SetInputListFile(...) method added.
   *   @history 2008-12-19 Unknown - List files are now fully supported, along
   *                           with output list files.
   *   @history 2010-12-20 Sharmila Prasad - Added ability to add branches right
   *                           off of the Pipeline
   *   @history 2010-12-21 Sharmila Prasad - Added documentation and ignore temp
   *                           files from disabled branches
   *   @history 2011-02-09 Sharmila Prasad - Added option continue to proceed
   *                           with execution even if an application in the
   *                           pipeline encounters an exception.
   *   @history 2011-08-15 Debbie A. Cook - Added member p_pausePosition and
   *                           method AddPause and modified method Run to allow
   *                           the pipeline to be stopped temporarily and
   *                           resumed.  When AddPause is used, Run will need to
   *                           be called an additional time for each AddPause
   *                           included in the pipeline to resume the execution
   *                           of the pipeline.
   *   @history 2012-11-21 Jeannie Backer - Added Progress output to indicate
   *                           which application is running. Added padding to
   *                           control statements. References # 795.
   *   @history 2016-08-28 Kelvin Rodriguez - Removed useless if statement comparing
   *                           a reference variable to Null. Part of porting to OS X 10.11.
   *   @history 2021-17-03 Adam Paquette - Update SetInputListFile to use the
   *                           FileList object rather than the TextFile object.
   *                           Also updated SetInputListFile isisparameter
   *                           function to chain into the SetInputListFile FileName
   *                           function.
   */
  class Pipeline {
    public:
      Pipeline(const QString &procAppName = "");
      ~Pipeline();

      void Prepare();
      void Run();

      void SetInputFile(const char *inputParam);
      void SetInputFile(const QString &inputParam);
      void SetInputFile(const char *inputParam, const char *virtualBandsParam);
      void SetInputFile(const QString &inputParam, const QString &virtualBandsParam);
      void SetInputListFile(const char *inputParam);
      void SetInputListFile(const QString &inputParam);
      void SetInputFile(const FileName &inputFileName);
      void SetInputListFile(const FileName &inputFileName);

      void SetOutputFile(const char *outputParam);
      void SetOutputFile(const QString &outputParam);
      void SetOutputFile(const FileName &outputFile);
      void SetOutputListFile(const char *outputFileNameParam);
      void SetOutputListFile(const QString &outputFileNameParam);
      void SetOutputListFile(const FileName &outputFileNameList);
      void KeepTemporaryFiles(bool keep);
      //! Returns true if temporary files will not be deleted, false if they will
      bool KeepTemporaryFiles() {
        return p_keepTemporary;
      }

      void AddPause();
      void AddToPipeline(const QString &appname);
      void AddToPipeline(const QString &appname, const QString &identifier);
      PipelineApplication &Application(const QString &identifier);
      PipelineApplication &Application(const int &index);

      void SetFirstApplication(const QString &appname);
      void SetLastApplication(const QString &appname);

      friend std::ostream &operator<<(std::ostream &os, Pipeline &pipeline);

      //! Returns the name of the pipeline
      QString Name() const {
        return p_procAppName;
      }
      //! Returns the number of applications in the pipeline
      int Size() const {
        return (int)p_apps.size();
      }

      /**
       * Returns the initial input file for the pipeline
       *
       * @param branch Branch of the original input to get the filename from
       *
       * @return QString Name of the original input file
       */
      QString OriginalInput(unsigned int branch) {
        return ((branch < p_originalInput.size()) ? p_originalInput[branch] : "");
      }

      //! Returns the number of input files
      int OriginalInputSize() {
         return p_originalInput.size();
      }

      /**
       * Returns the total number of input branches
       * Original branches = Number of input files * Number of branches
       *
       * @author Sharmila Prasad (12/20/2010)
       *
       * @return int - Total number of branches
       */
      int OriginalBranchesSize() {
        if (p_originalBranches.size() > 0){
          return p_originalBranches.size();
        }
        return p_inputBranches.size();
      }

      //! Returns the names of the original branches of the pipeline
      //! (input files * branches if any)
      std::vector<QString> OriginalBranches() {
        if (p_originalBranches.size() > 0){
          return p_originalBranches;
        }
        return p_inputBranches;
      }

      QString FinalOutput(int branch = 0, bool addModifiers = true);
      QString TemporaryFolder();

      void EnableAllApplications();

      /**
       * Start off the branches directly from the pipeline
       *
       * @author Sharmila Prasad (12/20/2010)
       *
       * @param branch - Branch name to be added
       */
      void AddOriginalBranch(QString branch){
        int size = (int)p_inputBranches.size();
        if (size == 1) {
          p_originalBranches.push_back(branch);
        }
        else {
          for (int i=0; i<size; i++) {
            p_originalBranches.push_back(p_inputBranches[i] + "." + branch);
          }
        }
      }

      /**
       * Set the continue flag
       *
       * @author Sharmila Prasad (2/9/2011)
       *
       * @param pbFlag - true/false
       */
      void SetContinue(bool pbFlag) {
        p_continue = pbFlag;
      };

    private:
      int p_pausePosition;
      QString p_procAppName; //!< The name of the pipeline
      std::vector<QString> p_originalInput; //!< The original input file
      std::vector<QString> p_inputBranches; //!< Branches for input list
      std::vector<QString> p_originalBranches; //!< The input file(s) + original branches from pipeline
      std::vector<QString> p_finalOutput; //!< The final output file (empty if needs calculated)
      std::vector<QString> p_virtualBands;//!< The virtual bands string
      bool p_keepTemporary; //!< True if keeping temporary files
      bool p_addedCubeatt; //!< True if the "cubeatt" program was added
      std::vector< PipelineApplication * > p_apps; //!< The pipeline applications
      std::vector< QString > p_appIdentifiers; //!< The strings to identify the pipeline applications
      bool p_outputListNeedsModifiers;
      bool p_continue; //!< continue the execution even if exception is encountered.
  };
};

#endif
