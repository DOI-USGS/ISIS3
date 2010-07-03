/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $                                                             
 * $Date: 2007/10/17 22:45:12 $                                                                 
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
#ifndef IsisAml_h
#define IsisAml_h

#include <string>
#include <vector>

#include <xercesc/sax2/SAX2XMLReader.hpp>

#include "IsisXMLApplication.h"
#include "IsisAmlData.h"
#include "Pvl.h"
#include "iString.h"

/**                                                                       
 * @brief Application program XML file parameter manager.                
 *                                                                        
 * This class is used to manage the data in an application XML file pertaining 
 * to the program parameters. The data is stored, accessed, and modified 
 * through this class and its helper classes.           
 *                                                                        
 * @ingroup ApplicationInterface                                                  
 *                                                                        
 * @author 2002-05-30 Stuart Sides                                         
 *                                                                                                                                                
 * @internal                                                              
 *   @todo This class needs an example.                                                               
 *   @history 2002-10-31 Stuart Sides - Added check to make sure all parameters 
 *                                      have some type of value or are excluded 
 *                                      by some other parameter.
 *   @history 2002-11-05 Stuart Sides - Modified GetString member to return the
 *                                      matching list member (if a list
 *                                      exists) instead of the value itself.
 *   @history 2002-12-10 Stuart Sides - modified GetFilename member to return 
 *                                      the filename(s) with environment 
 *                                      variables expanded.
 *   @history 2003-01-24 Stuart Sides - took out schema checking for the time 
 *                                      being. It will need to be put back in 
 *                                      after the user preferences object is 
 *                                      finished.
 *   @history 2003-01-27 Stuart Sides - modified GetFilename.
 *   @history 2003-02-07 Stuart Sides - fixed VerifyAll to better check 
 *                                      list/option/includes,excludes
 *   @history 2003-02-13 Stuart Sides - allowed list/option/included parameters 
 *                                      to use the default value instead of 
 *                                      just the value. 
 *   @history 2003-02-20 Stuart Sides - fixed problem where excludes/includes 
 *                                      were throwing an error when they should 
 *                                      not have.  
 *   @history 2003-03-12 Stuart Sides - added member to construct a command line 
 *                                      from the internalized data and return 
 *                                      it as a string (CommandLine).
 *   @history 2003-03-12 Stuart Sides - added member to construct a command line 
 *                                      from the internalized data and return it 
 *                                      as a string (CommandLine).
 *   @history 2003-03-13 Stuart Sides - modified member CommandLine to take a 
 *                                      Label reference instead of returning a 
 *                                      Label.
 *   @history 2003-03-13 Stuart Sides - socumented new member CommandLine.
 *   @history 2003-04-03 Stuart Sides - fixed bug where integer parameters that 
 *                                      need to be odd were not being checked.
 *   @history 2003-05-16 Stuart Sides - modified schema from astrogeology... 
 *                                      isis.astrogeology...
 *   @history 2003-05-30 Stuart Sides - fixed uninitialized variable compiler 
 *                                      error when -O1 flag was added.
 *   @history 2003-06-04 Stuart Sides - fixed a problem where the GetString 
 *                                      member was not returning the full list 
 *                                      option value.
 *   @history 2003-06-05 Stuart Sides - added the member "Version" to the class. 
 *                                      It will return the date of the most 
 *                                      recent change to the application. This 
 *                                      involved parsing the history/change 
 *                                      entries, which was being ignored 
 *                                      previously.
 *   @history 2003-07-02 Stuart Sides - added the new parameter type "cube"
 *   @history 2003-07-03 Stuart Sides - added the members "Brief and 
 *                                      Description" to return the application 
 *                                      brief and full description.
 *   @history 2003-08-01 Stuart Sides - made get and put Filename work for cubes 
 *                                      too.
 *   @history 2003-08-26 Jeff Anderson - added extension option to GetFilename 
 *                                       method.
 *   @history 2003-08-27 Stuart Sides - fixed bug where not all parameters would 
 *                                      get internalized if there where html 
 *                                      tags inside a description of a parameter
 *   @history 2003-12-03 Jeff Anderson - modified CommandLine method to output 
 *                                       all parameters except those in option 
 *                                       list excludes.
 *   @history 2004-01-21 Stuart Sides - added members for accessing a parameters 
 *                                      exclude(s).
 *   @history 2004-02-26 Stuart Sides - added parsing and get members for the 
 *                                      default path in the XML
 *   @history 2004-03-03 Stuart Sides - added member function PixelType so 
 *                                      IsisGui can get the pixel type of a cube
 *   @history 2004-09-01 Stuart Sides - modified so the parameter names don't 
 *                                      get capitalized as they are read from 
 *                                      the applications xml file. This caused 
 *                                      other modifications to the ReturnParam 
 *                                      member, so it would find the requested 
 *                                      parameter in a case insensitive way.
 *   @history 2005-10-03 Elizabeth Miller - changed @ingroup tag
 *   @history 2005-12-13 Stuart Sides - Took out an "exit(1)" in the GetString
 *                                      method. Couldn't find a reason for it
 *                                      to be there.
 *   @history 2005-12-28 Elizabeth Miller - Added extra methods and private vars
 *                                      to retrieve info needed for gui and 
 *                                      command line help
 *   @history 2006-02-13 Elizabeth Miller - Added GuiHelper Capabilities
 *   @history 2006-10-17 Steven Lambright - Fixed problem with boolean inclusion/exclusion
 */                                                                       
class IsisAml : protected IsisAmlData {

// Public section
public:

  IsisAml (const std::string &xmlfile);

  ~IsisAml ();


  // Methods for searching and retrieving application info
  // via the parameter name

  void PutAsString (const std::string &paramName, const std::string &value);
  void PutAsString (const std::string &paramName, const std::vector<std::string> &value);

  void PutFilename (const std::string &paramName, const std::string &value);
  void PutFilename (const std::string &paramName, const std::vector<std::string> &value);

  void PutDouble (const std::string &paramName, const double &value);
  void PutDouble (const std::string &paramName, const std::vector<double> &value);

  void PutInteger (const std::string &paramName, const int &value);
  void PutInteger (const std::string &paramName, const std::vector<int> &value);

  void PutBoolean (const std::string &paramName, const bool &value);
  void PutBoolean (const std::string &paramName, const std::vector<bool> &value);

  void PutString (const std::string &paramName, const std::string &value);
  void PutString (const std::string &paramName, const std::vector<std::string> &value);

  std::string GetAsString (const std::string &paramName) const;
  void GetAsString (const std::string &paramName, std::vector<std::string> &values) const;

  std::string GetFilename (const std::string &paramName, std::string extension="") const;
  void GetFilename (const std::string &paramName, std::vector<std::string> &values) const;

  std::string GetString (const std::string &paramName) const;
  void GetString (const std::string &paramName, std::vector<std::string> &values) const;

  int GetInteger (const std::string &paramName) const;
  void GetInteger (const std::string &paramName, std::vector<int> &values) const;

  bool GetBoolean (const std::string &paramName) const;

  void GetBoolean (const std::string &paramName, std::vector<bool> &values) const;

  double GetDouble (const std::string &paramName) const;
  void GetDouble (const std::string &paramName, std::vector<double> &values) const;

  bool WasEntered (const std::string &paramName) const;
  void Clear (const std::string &paramName);

  Isis::CubeAttributeInput& GetInputAttribute (const std::string &paramName);
  Isis::CubeAttributeOutput& GetOutputAttribute (const std::string &paramName);

  // Methods for retrieving application info not inside a group

  std::string ProgramName () const;
  std::string Brief () const;
  std::string Description () const;

  // Methods for searching and retrieving application info
  // via the index into groups and parameters

  int  NumGroups () const;

  std::string GroupName (const int &group) const;

  int NumParams (const int &) const;

  std::string ParamName (const int &group, const int &param) const;

  std::string ParamType (const int &group, const int &param) const;

  std::string ParamBrief (const int &group, const int &param) const;

  std::string ParamDescription (const int &group, const int &param) const;

  std::string ParamMinimum (const int &group, const int &param) const;

  std::string ParamMinimumInclusive (const int &group, const int &param) const;

  std::string ParamMaximum (const int &group, const int &param) const;

  std::string ParamMaximumInclusive (const int &group, const int &param) const;

  std::string ParamOdd (const int &group, const int &param) const;

  int ParamGreaterThanSize (const int &group, const int &param) const;

  int ParamGreaterThanOrEqualSize (const int &group, const int &param) const;

  int ParamLessThanSize (const int &group, const int &param) const;

  int ParamLessThanOrEqualSize (const int &group, const int &param) const;

  int ParamNotEqualSize (const int &group, const int &param) const;

  std::string ParamGreaterThan (const int &group, const int &param, 
                                const int &great) const;

  std::string ParamGreaterThanOrEqual (const int &group, const int &param, 
                                const int &great) const;

  std::string ParamLessThan (const int &group, const int &param, 
                                const int &great) const;

  std::string ParamLessThanOrEqual (const int &group, const int &param, 
                                const int &les) const;

  std::string ParamNotEqual (const int &group, const int &param, 
                                const int &notEq) const;

  int ParamListSize (const int &group, const int &param) const;

  std::string ParamListValue (const int &group, const int &param,
                         const int &option) const;

  std::string ParamListBrief (const int &group, const int &param,
                         const int &option) const;

  std::string ParamListDescription (const int &group, const int &param,
                               const int &option) const;

  int ParamListExcludeSize (const int &group, const int &param,
                            const int &option) const;

  std::string ParamListExclude (const int &group, const int &param,
                           const int &option, const int &exclude) const;

  int ParamListIncludeSize (const int &group, const int &param,
                            const int &option) const;

  std::string ParamListInclude (const int &group, const int &param,
                           const int &option, const int &include) const;

  std::string ParamDefault (const int &group, const int &param) const;

  std::string ParamInternalDefault (const int &group, const int &param) const;

  std::string ParamFilter (const int &group, const int &param) const;
  
  std::string ParamPath (const int &group, const int &param) const;

  std::string ParamFileMode (const int &group, const int &param) const;

  int ParamExcludeSize (const int &group, const int &param) const;

  std::string ParamExclude (const int &group, const int &param,
                       const int &exclude) const;
  int ParamIncludeSize (const int &group, const int &param) const;

  std::string ParamInclude (const int &group, const int &param,
                       const int &include) const;

  std::string PixelType (const int &group, const int &param) const;

  int HelpersSize(const int &group, const int &param) const;

  std::string HelperButtonName(const int &group, const int &param, 
                               const int &helper) const;

  std::string HelperFunction(const int &group, const int &param, 
                             const int &helper) const;

  std::string HelperBrief(const int &group, const int &param, 
                          const int &helper) const;

  std::string HelperDescription(const int &group, const int &param, 
                          const int &helper) const;

  std::string HelperIcon(const int &group, const int &param,  
                         const int &helper) const;

  // Test all parameters for valid values and conditions
  void VerifyAll ();

  bool StringToBool (Isis::iString value) const;

  void CommandLine (Isis::Pvl &lab) const;

  std::string Version () const;


// Private section
private:

  //! The XML file parser.
  XERCES::SAX2XMLReader *parser;
  //! The application handler.
  IsisXMLApplication *appHandler;

  // Member functions
  void StartParser (const char *xmlfile);

  const IsisParameterData *ReturnParam (const std::string &paramName) const;

  void Verify (const IsisParameterData *param);
};


#endif
