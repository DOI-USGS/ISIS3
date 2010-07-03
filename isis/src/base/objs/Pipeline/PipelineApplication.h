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
#include "iString.h"

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
 */
  class PipelineApplication {
    public:
      PipelineApplication(iString appName, Pipeline *pipe);
      PipelineApplication(iString appName, PipelineApplication *previous);

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
      const iString &Name() const { return p_name; }
      //! Get the parameters for running this program; one element in the vector per run
      const vector<iString> &ParamString() const { return p_paramString; }
      //! Get the branches this program expects as input
      const vector<iString> &InputBranches() const { return p_inBranches; }
      //! Get the branches this program has as output
      const vector<iString> &OutputBranches() const { 
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
      void Enable() { p_enabled = true; };

      //! This method disables this program, causing it to be ignored 
      void Disable() { p_enabled = false; };

      //! Returns true if this program will be run
      const bool &Enabled() const { return p_enabled; }

      void SetInputParameter(const iString &inputParamName, bool supportsVirtualBands);
      void SetInputParameter(const iString &inputParamName, CustomParameterValue value, bool supportsVirtualBands);

      void SetOutputParameter(const iString &outputParamName, const iString &outNameModifier, const iString &outFileExtension = "cub");
      void SetOutputParameter(const iString &branch, const iString &outputParamName, 
                              const iString &outNameModifier, const iString &outFileExtension);

      void AddBranch(const iString &modString, NameModifierType type);

      void AddParameter(const iString &inputParamName, const iString &appParamName);
      void AddParameter(const iString &branch, const iString &inputParamName, const iString &appParamName);

      void AddConstParameter(const iString &appParamName, const iString &appParamValue);
      void AddConstParameter(const iString &branch, const iString &appParamName, const iString &appParamValue);

      void AddParameter(const iString &appParamName, CustomParameterValue value);
      void AddParameter(const iString &branch, const iString &appParamName, CustomParameterValue value);

      //! This returns this application's output name modifier
      iString OutputNameModifier() { return (!p_outputMod.empty() || !Previous())? p_outputMod : Previous()->OutputNameModifier(); }
      //! This returns this application's output file name's extension
      iString OutputExtension() { return (!p_outputExtension.empty() || !Previous())? p_outputExtension : Previous()->OutputExtension(); }

      //! This returns this application's output files. Only valid after BuildParamString is called.
      vector<iString> &GetOutputs();

      vector<iString> TemporaryFiles();

      /**
       * Link to the next application in the pipeline
       * 
       * @param next The next pipeline application
       */
      void SetNext(PipelineApplication *next) { p_next = next; }


      /**
       * Link to the previous application in the pipeline
       * 
       * @param prev The previous pipeline application
       */
      void SetPrevious(PipelineApplication *prev) { p_previous = prev; }

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
      void SetVirtualBands(vector<iString> bands);

    private:
      bool FutureOutputFileCreated();
      bool LastApplicationWithOutput();
      //! Returns the pipeline
      Pipeline *GetPipeline() { return p_pipeline; }

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
      bool StringStartsWith(iString from, iString compare) { 
        if(compare.size() > from.size()) return false;

        for(unsigned int index = 0; index < compare.size(); index++)
          if(from[index] != compare[index]) return false;

        return true;
      }

      iString CalculateInputFile(int branch);
      iString CalculateOutputFile(int branch);
      iString GetRealLastOutput(bool skipOne = false);
      PipelineParameter &GetInputParameter(int branch);

      int FindBranch(iString name, bool input = true);

      bool p_enabled; //!< This application enabled?
      bool p_supportsVirtualBands; //!< This application supports virtual bands?
      iString p_name; //!< Name of this application
      vector<iString> p_outputs; //!< Actual output files 
      vector<iString> p_tempFiles; //!< Actial temporary files
      vector<iString> p_paramString; //!< Built parameter strings
      vector<iString> p_inBranches; //!< Input branches
      vector<iString> p_outBranches; //!< Output branches
      
      vector<PipelineParameter> p_output; //!< Output parameters
      iString p_outputMod; //!< Output file name modifier
      iString p_outputExtension; //!< Output file name extension
      vector<iString> p_virtualBands; //!< Virtual bands string to add (empty if none)

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
   */
  class PipelineParameter {
    public:
      /**
       * Construct the parameter with only a parameter name; affects all branches and 
       * is not a special value. 
       * 
       * @param paramName Parameter name
       */
      PipelineParameter(iString paramName) {
        p_paramName = paramName;
        p_special = (PipelineApplication::CustomParameterValue)-1;
        p_branch = -1;
      }


      /**
       * Construct the parameter with a parameter name and value; affects all branches
       *  and is not special
       * 
       * @param paramName Parameter name
       * @param value Parameter value
       */
      PipelineParameter(iString paramName, iString value) {
        p_paramName = paramName;
        p_paramValue = value;
        p_special = (PipelineApplication::CustomParameterValue)-1;
        p_branch = -1;
      }


      /**
       * Construct the parameter with only a parameter name; affects only the 
       * specified branch and is not a special value. 
       * 
       * @param branch Branch this parameter affects
       * @param paramName Parameter name
       */
      PipelineParameter(int branch, iString paramName) {
        p_branch = branch;
        p_paramName = paramName;
        p_special = (PipelineApplication::CustomParameterValue)-1;
      }


      /**
       * Construct the parameter with a parameter name and value; affects only the 
       * specified branch and is not a special value. 
       * 
       * @param branch Branch this parameter affects
       * @param paramName Parameter name
       * @param paramValue Special parameter value
       */
      PipelineParameter(int branch, iString paramName, iString paramValue) {
        p_branch = branch;
        p_paramValue = paramValue;
        p_paramName = paramName;
        p_special = (PipelineApplication::CustomParameterValue)-1;
      }


      /**
       * Construct the parameter with a parameter name and special value; affects all 
       * branches 
       * 
       * @param paramName Parameter name
       * @param special Special value
       */
      PipelineParameter(iString paramName, PipelineApplication::CustomParameterValue special) {
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
      PipelineParameter(int branch, iString paramName, PipelineApplication::CustomParameterValue special) {
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
      bool AppliesToBranch(int branch) { return (p_branch == -1 || p_branch == branch); }

      //! Name of the parameter
      iString Name() { return p_paramName; }
      //! Non-special value of the parameter
      iString Value() { return p_paramValue; }
      //! True if the parameter value is special
      bool IsSpecial() { return (p_special != (PipelineApplication::CustomParameterValue)-1); };
      //! Special value of the parameter
      PipelineApplication::CustomParameterValue Special() { return p_special; }
      //! True if branch-independant
      bool AffectsAllBranches() { return p_branch == -1; } 

    private:
      int p_branch; //!< Branch this affects
      iString p_paramName; //!< Parameter name
      iString p_paramValue; //!< Parameter non-special value
      PipelineApplication::CustomParameterValue p_special; //!< Parameter special value
  };
};
#endif
