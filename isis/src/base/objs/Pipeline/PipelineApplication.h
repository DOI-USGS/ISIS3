#ifndef PipelineApplication_h
#define PipelineApplication_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>

#include <QString>

#include "IException.h"

namespace Isis {
  class Pipeline;
  class PipelineParameter;

  /**
   * This class represents one application in the pipeline. This contains methods
   * for setting application-specific parameters, such as application inputs and
   * outputs. Only pipeline should be instantiating these!
   *
   * The applications work as a doubly-linked list in order to calculate inputs
   * and outputs correctly. Calling SetNext or SetPrevious is not recommended.
   *
   *
   * @author 2008-08-04 Steven Lambright
   *
   * @internal
   *   @history 2008-09-25 Added features: Application identifiers other than the
   *            application names, branched original input, branching from
   *            branches, partial branch merging (discontinuing branches*)
   *   @history 2008-09-26 Steven Lambright Fixed introduced bug where input data
   *            was not being found when no previous programs in the pipeline
   *            generated output (needed to go back to original input).
   *   @history 2008-09-26 Steven Lambright Changed CalculateOutputFile(...)
   *            to work in more cases. Pipeline::FinalOutput is relied upon more
   *            heavily now to do the right thing.
   *   @history 2010-12-20 Sharmila Prasad - Added the ability to enable/disable a
   *            branch.
   *   @history 2011-02-09 Sharmila Prasad - Added option continue to proceed with execution
   *            of the next application in the pipeline if the current encounters an exception.
   */
  class PipelineApplication {
    public:
      PipelineApplication(QString appName, Pipeline *pipe);
      PipelineApplication(QString appName, PipelineApplication *previous);

      //! This is the destructor
      ~PipelineApplication() {};

      /**
       * This is used for branches. Right now, only known strings can be used to
       * specify each branch.
       */
      enum NameModifierType {
        //! Known strings
        ConstantStrings
      };

      /**
       * This is used to set custom values that must be calculated on the fly
       */
      enum CustomParameterValue {
        //! The very last output file. Do not use this for input parameters if it's not necessary, that is done automatically
        LastOutput,
        //! A list of files from the last run application's output.
        // Implies branches will be merged if this is set as an input parameter.
        LastAppOutputList,
        //! A list of files from the last run application's output.
        // Implies branches will NOT be merged if this is set as an input parameter.
        LastAppOutputListNoMerge
      };

      //! Get the name of this pipeline application
      const QString &Name() const {
        return p_name;
      }
      //! Get the parameters for running this program; one element in the vector per run
      const std::vector<QString> &ParamString() const {
        return p_paramString;
      }
      //! Get the branches this program expects as input
      const std::vector<QString> &InputBranches() const {
        return p_inBranches;
      }
      //! Get the branches this program has as output
      const std::vector<QString> &OutputBranches() const {
        if(!Enabled() && Previous()) {
          return Previous()->OutputBranches();
        }
        else if(Enabled()) {
          return p_outBranches;
        }
        else {
          return p_inBranches;
        }
      }

      //! This method enables this program to be run
      void Enable() {
        p_enabled = true;
      };

      //! This method disables this program, causing it to be ignored
      void Disable() {
        p_enabled = false;
      };

      //! Returns true if this program will be run
      const bool &Enabled() const {
        return p_enabled;
      }

      void SetInputParameter(const QString &inputParamName, bool supportsVirtualBands);
      void SetInputParameter(const QString &inputParamName, CustomParameterValue value, bool supportsVirtualBands);

      void SetOutputParameter(const QString &outputParamName, const QString &outNameModifier, const QString &outFileExtension = "cub");
      void SetOutputParameter(const QString &branch, const QString &outputParamName,
                              const QString &outNameModifier, const QString &outFileExtension);

      void AddBranch(const QString &modString, NameModifierType type);

      void AddParameter(const QString &inputParamName, const QString &appParamName);
      void AddParameter(const QString &branch, const QString &inputParamName, const QString &appParamName);

      void AddConstParameter(const QString &appParamName, const QString &appParamValue);
      void AddConstParameter(const QString &branch, const QString &appParamName, const QString &appParamValue);

      void AddParameter(const QString &appParamName, CustomParameterValue value);
      void AddParameter(const QString &branch, const QString &appParamName, CustomParameterValue value);

      //! This returns this application's output name modifier
      QString OutputNameModifier() {
        return (!p_outputMod.isEmpty() || !Previous()) ? p_outputMod : Previous()->OutputNameModifier();
      }
      //! This returns this application's output file name's extension
      QString OutputExtension() {
        return (!p_outputExtension.isEmpty() || !Previous()) ? p_outputExtension : Previous()->OutputExtension();
      }
      //! This returns this application's output files. Only valid after BuildParamString is called.
      std::vector<QString> &GetOutputs();

      std::vector<QString> TemporaryFiles();

      /**
       * Link to the next application in the pipeline
       *
       * @param next The next pipeline application
       */
      void SetNext(PipelineApplication *next) {
        p_next = next;
      }


      /**
       * Link to the previous application in the pipeline
       *
       * @param prev The previous pipeline application
       */
      void SetPrevious(PipelineApplication *prev) {
        p_previous = prev;
      }

      void BuildParamString();

      //!This returns the next enabled pipeline application or null
      PipelineApplication *Next() const {
        if(p_next == NULL) return p_next;
        if(!p_next->Enabled()) return p_next->Next();
        return p_next;
      }

      //!This returns the last enabled pipeline application or null
      PipelineApplication *Previous() const {
        if(p_previous == NULL) return p_previous;
        if(!p_previous->Enabled()) return p_previous->Previous();
        return p_previous;
      }

      //!This returns the previous enabled pipeline application that makes output or null
      PipelineApplication *PreviousOutputer() const {
        if(p_previous == NULL) return p_previous;
        if(!p_previous->Enabled()) return p_previous->Previous();
        if(p_previous->p_output.empty()) return p_previous->Previous();
        return p_previous;
      }

      bool SupportsVirtualBands();
      void SetVirtualBands(std::vector<QString> bands);
      
      /**
       * Enable/Disable Branch given the branch name
       * 
       * @author Sharmila Prasad (12/20/2010)
       * 
       * @param branch - branch name
       * @param flag   - true/false
       */
      void EnableBranch(QString branch, bool flag) {
        for(int i=0; i<(int)p_inBranches.size(); i++) {
          if (p_inBranches[i].contains(branch))
            p_enableBranch[i] = flag;
        }
      }
      
      /**
       * Check whether a branch is enabled given branch index
       * 
       * @author Sharmila Prasad (12/20/2010)
       * 
       * @param branch - index into the branch
       * 
       * @return bool - true/false
       */
      bool BranchEnabled(int branch){
        if (branch >= 0 && branch >= (int)p_enableBranch.size())
          return false;
        return p_enableBranch[branch] ;
      }
      
      /**
       * Set the continue flag status
       * 
       * @author Sharmila Prasad (2/9/2011)
       * 
       * @param pbFlag - true/false
       */
      void SetContinue(bool pbFlag){
        p_continue = pbFlag;
      };
      
      /**
       * Get the continue flag status
       * 
       * @author Sharmila Prasad (2/9/2011)
       * 
       * @return bool - return continue status
       */
      bool Continue(void) {
        return p_continue;
      };
      
    private:
      bool FutureOutputFileCreated();
      bool LastApplicationWithOutput();
      //! Returns the pipeline
      Pipeline *GetPipeline() {
        return p_pipeline;
      }

      //! Return true is this application does branch (one input branch, multiple output)
      bool Branches() {
        if(p_inBranches.size() >= p_outBranches.size()) return false;
        return true;
      }

      //! Returns true if this application does merge branches (multiple input branches, one output)
      bool Merges() {
        if(p_inBranches.size() == 1) return false;
        if(p_outBranches.size() == 1) return true;
        return false;
      }

      /**
       * String comparison helper, returns true if from starts with compare bool
       *
       * @param from Longer string ("abcdef")
       * @param compare String to compare against ("abc")
       */
      bool StringStartsWith(QString from, QString compare) {
        if(compare.size() > from.size()) return false;

        for(int index = 0; index < compare.size(); index++)
          if(from[index] != compare[index]) return false;

        return true;
      }

      QString CalculateInputFile(int branch);
      QString CalculateOutputFile(int branch);
      QString GetRealLastOutput(bool skipOne = false);
      PipelineParameter &GetInputParameter(int branch);

      int FindBranch(QString name, bool input = true);

      bool p_continue;//!< Continue the pipeline execution even if an error is encountered by this app
      bool p_enabled; //!< This application enabled?
      bool p_supportsVirtualBands; //!< This application supports virtual bands?
      QString p_name; //!< Name of this application
      std::vector<QString> p_outputs;     //!< Actual output files
      std::vector<QString> p_tempFiles;   //!< Actial temporary files
      std::vector<QString> p_paramString; //!< Built parameter strings
      std::vector<QString> p_inBranches;  //!< Input branches
      std::vector<QString> p_outBranches; //!< Output branches
      std::vector<bool>    p_enableBranch; //!< Branch enabled/disabled
      
      std::vector<PipelineParameter> p_output; //!< Output parameters
      QString p_outputMod; //!< Output file name modifier
      QString p_outputExtension; //!< Output file name extension
      std::vector<QString> p_virtualBands; //!< Virtual bands string to add (empty if none)

      std::vector<PipelineParameter> p_input; //!< Input parameters
      std::vector<PipelineParameter> p_params; //!< Regular parameters

      PipelineApplication *p_previous; //!< Previous pipeline application
      PipelineApplication *p_next; //!< Next pipeline application
      Pipeline *p_pipeline; //!< The pipeline
  };

  /**
   * This class represents a parameter of some type for the PipelineApplication.
   * This class simply helps to store multiple pieces of data and provide quick
   * access to them, and should never be used outside of the PipelineApplication
   * class.
   *
   * @author 2008-08-01 Steven Lambright
   *
   * @internal
   */
  class PipelineParameter {
    public:
      /**
       * Construct the parameter with only a parameter name; affects all branches and
       * is not a special value.
       *
       * @param paramName Parameter name
       */
      PipelineParameter(QString paramName) {
        p_paramName = paramName;
        p_special = (PipelineApplication::CustomParameterValue) - 1;
        p_branch = -1;
      }


      /**
       * Construct the parameter with a parameter name and value; affects all branches
       *  and is not special
       *
       * @param paramName Parameter name
       * @param value Parameter value
       */
      PipelineParameter(QString paramName, QString value) {
        p_paramName = paramName;
        p_paramValue = value;
        p_special = (PipelineApplication::CustomParameterValue) - 1;
        p_branch = -1;
      }


      /**
       * Construct the parameter with only a parameter name; affects only the
       * specified branch and is not a special value.
       *
       * @param branch Branch this parameter affects
       * @param paramName Parameter name
       */
      PipelineParameter(int branch, QString paramName) {
        p_branch = branch;
        p_paramName = paramName;
        p_special = (PipelineApplication::CustomParameterValue) - 1;
      }


      /**
       * Construct the parameter with a parameter name and value; affects only the
       * specified branch and is not a special value.
       *
       * @param branch Branch this parameter affects
       * @param paramName Parameter name
       * @param paramValue Special parameter value
       */
      PipelineParameter(int branch, QString paramName, QString paramValue) {
        p_branch = branch;
        p_paramValue = paramValue;
        p_paramName = paramName;
        p_special = (PipelineApplication::CustomParameterValue) - 1;
      }


      /**
       * Construct the parameter with a parameter name and special value; affects all
       * branches
       *
       * @param paramName Parameter name
       * @param special Special value
       */
      PipelineParameter(QString paramName, PipelineApplication::CustomParameterValue special) {
        p_paramName = paramName;
        p_special = special;
        p_branch = -1;
      }


      /**
       * Construct the parameter with a parameter name and special value; affects only
       * the specified branch
       *
       * @param branch Branch this parameter affects
       * @param paramName Parameter name
       * @param special Special parameter value
       */
      PipelineParameter(int branch, QString paramName, PipelineApplication::CustomParameterValue special) {
        p_paramName = paramName;
        p_special = special;
        p_branch = branch;
      }


      /**
       * Returns whether or not the specified branch is affected.
       *
       * @param branch Branch to test
       *
       * @return bool Whether or not the specified branch is affected.
       */
      bool AppliesToBranch(int branch) {
        return (p_branch == -1 || p_branch == branch);
      }

      //! Name of the parameter
      QString Name() {
        return p_paramName;
      }
      //! Non-special value of the parameter
      QString Value() {
        return p_paramValue;
      }
      //! True if the parameter value is special
      bool IsSpecial() {
        return (p_special != (PipelineApplication::CustomParameterValue) - 1);
      };
      //! Special value of the parameter
      PipelineApplication::CustomParameterValue Special() {
        return p_special;
      }
      //! True if branch-independant
      bool AffectsAllBranches() {
        return p_branch == -1;
      }

    private:
      int p_branch; //!< Branch this affects
      QString p_paramName; //!< Parameter name
      QString p_paramValue; //!< Parameter non-special value
      PipelineApplication::CustomParameterValue p_special; //!< Parameter special value
  };
};
#endif
