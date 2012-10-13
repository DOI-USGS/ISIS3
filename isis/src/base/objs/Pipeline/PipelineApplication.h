#ifndef PipelineApplication_h
#define PipelineApplication_h
/**
 * @file
 * $Revision: 1.5 $
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
#include "IException.h"
#include "IString.h"

using namespace std;

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
      PipelineApplication(IString appName, Pipeline *pipe);
      PipelineApplication(IString appName, PipelineApplication *previous);

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
      const IString &Name() const {
        return p_name;
      }
      //! Get the parameters for running this program; one element in the vector per run
      const vector<IString> &ParamString() const {
        return p_paramString;
      }
      //! Get the branches this program expects as input
      const vector<IString> &InputBranches() const {
        return p_inBranches;
      }
      //! Get the branches this program has as output
      const vector<IString> &OutputBranches() const {
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

      void SetInputParameter(const IString &inputParamName, bool supportsVirtualBands);
      void SetInputParameter(const IString &inputParamName, CustomParameterValue value, bool supportsVirtualBands);

      void SetOutputParameter(const IString &outputParamName, const IString &outNameModifier, const IString &outFileExtension = "cub");
      void SetOutputParameter(const IString &branch, const IString &outputParamName,
                              const IString &outNameModifier, const IString &outFileExtension);

      void AddBranch(const IString &modString, NameModifierType type);

      void AddParameter(const IString &inputParamName, const IString &appParamName);
      void AddParameter(const IString &branch, const IString &inputParamName, const IString &appParamName);

      void AddConstParameter(const IString &appParamName, const IString &appParamValue);
      void AddConstParameter(const IString &branch, const IString &appParamName, const IString &appParamValue);

      void AddParameter(const IString &appParamName, CustomParameterValue value);
      void AddParameter(const IString &branch, const IString &appParamName, CustomParameterValue value);

      //! This returns this application's output name modifier
      IString OutputNameModifier() {
        return (!p_outputMod.empty() || !Previous()) ? p_outputMod : Previous()->OutputNameModifier();
      }
      //! This returns this application's output file name's extension
      IString OutputExtension() {
        return (!p_outputExtension.empty() || !Previous()) ? p_outputExtension : Previous()->OutputExtension();
      }
      //! This returns this application's output files. Only valid after BuildParamString is called.
      vector<IString> &GetOutputs();

      vector<IString> TemporaryFiles();

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
      void SetVirtualBands(vector<IString> bands);
      
      /**
       * Enable/Disable Branch given the branch name
       * 
       * @author Sharmila Prasad (12/20/2010)
       * 
       * @param branch - branch name
       * @param flag   - true/false
       */
      void EnableBranch(IString branch, bool flag) {
        for(int i=0; i<(int)p_inBranches.size(); i++) {
          if (p_inBranches[i].rfind(branch) != string::npos)
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
      bool StringStartsWith(IString from, IString compare) {
        if(compare.size() > from.size()) return false;

        for(unsigned int index = 0; index < compare.size(); index++)
          if(from[index] != compare[index]) return false;

        return true;
      }

      IString CalculateInputFile(int branch);
      IString CalculateOutputFile(int branch);
      IString GetRealLastOutput(bool skipOne = false);
      PipelineParameter &GetInputParameter(int branch);

      int FindBranch(IString name, bool input = true);

      bool p_continue;//!< Continue the pipeline execution even if an error is encountered by this app
      bool p_enabled; //!< This application enabled?
      bool p_supportsVirtualBands; //!< This application supports virtual bands?
      IString p_name; //!< Name of this application
      vector<IString> p_outputs;     //!< Actual output files
      vector<IString> p_tempFiles;   //!< Actial temporary files
      vector<IString> p_paramString; //!< Built parameter strings
      vector<IString> p_inBranches;  //!< Input branches
      vector<IString> p_outBranches; //!< Output branches
      vector<bool>    p_enableBranch; //!< Branch enabled/disabled
      
      vector<PipelineParameter> p_output; //!< Output parameters
      IString p_outputMod; //!< Output file name modifier
      IString p_outputExtension; //!< Output file name extension
      vector<IString> p_virtualBands; //!< Virtual bands string to add (empty if none)

      vector<PipelineParameter> p_input; //!< Input parameters
      vector<PipelineParameter> p_params; //!< Regular parameters

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
      PipelineParameter(IString paramName) {
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
      PipelineParameter(IString paramName, IString value) {
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
      PipelineParameter(int branch, IString paramName) {
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
      PipelineParameter(int branch, IString paramName, IString paramValue) {
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
      PipelineParameter(IString paramName, PipelineApplication::CustomParameterValue special) {
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
      PipelineParameter(int branch, IString paramName, PipelineApplication::CustomParameterValue special) {
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
      IString Name() {
        return p_paramName;
      }
      //! Non-special value of the parameter
      IString Value() {
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
      IString p_paramName; //!< Parameter name
      IString p_paramValue; //!< Parameter non-special value
      PipelineApplication::CustomParameterValue p_special; //!< Parameter special value
  };
};
#endif
