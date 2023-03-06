#ifndef IsisAml_h
#define IsisAml_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>

#include <QString>

#include <xercesc/sax2/SAX2XMLReader.hpp>

#include <nlohmann/json.hpp>

#include "IsisXMLApplication.h"
#include "IsisAmlData.h"
#include "Pvl.h"

class QString;

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
 *   @history 2002-12-10 Stuart Sides - modified GetFileName member to return
 *                                      the filename(s) with environment
 *                                      variables expanded.
 *   @history 2003-01-24 Stuart Sides - took out schema checking for the time
 *                                      being. It will need to be put back in
 *                                      after the user preferences object is
 *                                      finished.
 *   @history 2003-01-27 Stuart Sides - modified GetFileName.
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
 *   @history 2003-08-01 Stuart Sides - made get and put FileName work for cubes
 *                                      too.
 *   @history 2003-08-26 Jeff Anderson - added extension option to GetFileName
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
 *   @history 2006-10-17 Steven Lambright - Fixed problem with boolean
 *                                          inclusion/exclusion
 *   @history 2010-07-19 Jeannie Walldren - Added check whether user preferences
 *                          are set to allow file overwrite in Verify() method
 *   @history 2010-07-21 Sharmila Prasad - Modified for doxygen documentation
 *                                         Warning
 *   @history 2010-07-22 Steven Lambright - Array values for parameters are now
 *                                         written to the history file
 *   @history 2011-08-16 Sharmila Prasad - Added API to CreatePVL from a GUI Group and
 *                                         Get GUI Group index given the Group name
 *   @history 2012-11-20 Janet Barrett - Fixed the GetString method so that it doesn't
 *                                       stop searching for a matched string until an
 *                                       exact match is found or it has gone through the
 *                                       entire list. Fixes #554.
 *   @history 2016-08-28 Kelvin Rodriguez - Moved 'using namespace std' statements to be
 *                                         after #includes to squash implicit declaration
 *                                         warnings in clang. Part of porting to OSX 10.11.
 *   @history 2017-08-08 Adam Goins - Added an additional catch statement to display the 
 *                                    file name of an XML file that threw an error while parsing.
 */
class IsisAml : protected IsisAmlData {

// Public section
  public:

    IsisAml(const QString &xmlfile);

    ~IsisAml();


    // Methods for searching and retrieving application info
    // via the parameter name

    void PutAsString(const QString &paramName, const QString &value);
    void PutAsString(const QString &paramName, const std::vector<QString> &value);

    void PutFileName(const QString &paramName, const QString &value);
    void PutFileName(const QString &paramName, const std::vector<QString> &value);

    void PutCubeName(const QString &paramName, const QString &value);

    void PutDouble(const QString &paramName, const double &value);
    void PutDouble(const QString &paramName, const std::vector<double> &value);

    void PutInteger(const QString &paramName, const int &value);
    void PutInteger(const QString &paramName, const std::vector<int> &value);

    void PutBoolean(const QString &paramName, const bool &value);
    void PutBoolean(const QString &paramName, const std::vector<bool> &value);

    void PutString(const QString &paramName, const QString &value);
    void PutString(const QString &paramName, const std::vector<QString> &value);

    QString GetAsString(const QString &paramName) const;
    void GetAsString(const QString &paramName, std::vector<QString> &values) const;

    QString GetFileName(const QString &paramName, QString extension = "") const;
    void GetFileName(const QString &paramName, std::vector<QString> &values) const;

    QString GetCubeName(const QString &paramName, QString extension = "") const;

    QString GetString(const QString &paramName) const;
    void GetString(const QString &paramName, std::vector<QString> &values) const;

    int GetInteger(const QString &paramName) const;
    void GetInteger(const QString &paramName, std::vector<int> &values) const;

    bool GetBoolean(const QString &paramName) const;

    void GetBoolean(const QString &paramName, std::vector<bool> &values) const;

    double GetDouble(const QString &paramName) const;
    void GetDouble(const QString &paramName, std::vector<double> &values) const;

    bool WasEntered(const QString &paramName) const;
    void Clear(const QString &paramName);

    Isis::CubeAttributeInput &GetInputAttribute(const QString &paramName);
    Isis::CubeAttributeOutput &GetOutputAttribute(const QString &paramName);

    // Methods for retrieving application info not inside a group

    QString ProgramName() const;
    QString Brief() const;
    QString Description() const;

    // Methods for searching and retrieving application info
    // via the index into groups and parameters

    int  NumGroups() const;

    QString GroupName(const int &group) const;

    //! Given the group name get its index in group array
    int GroupIndex(const QString & grpName) const;
  
    nlohmann::json GetParams();

    int NumParams(const int &) const;

    QString ParamName(const int &group, const int &param) const;

    QString ParamType(const int &group, const int &param) const;

    QString ParamBrief(const int &group, const int &param) const;

    QString ParamDescription(const int &group, const int &param) const;

    QString ParamMinimum(const int &group, const int &param) const;

    QString ParamMinimumInclusive(const int &group, const int &param) const;

    QString ParamMaximum(const int &group, const int &param) const;

    QString ParamMaximumInclusive(const int &group, const int &param) const;

    QString ParamOdd(const int &group, const int &param) const;

    int ParamGreaterThanSize(const int &group, const int &param) const;

    int ParamGreaterThanOrEqualSize(const int &group, const int &param) const;

    int ParamLessThanSize(const int &group, const int &param) const;

    int ParamLessThanOrEqualSize(const int &group, const int &param) const;

    int ParamNotEqualSize(const int &group, const int &param) const;

    QString ParamGreaterThan(const int &group, const int &param,
                                 const int &great) const;

    QString ParamGreaterThanOrEqual(const int &group, const int &param,
                                        const int &great) const;

    QString ParamLessThan(const int &group, const int &param,
                              const int &great) const;

    QString ParamLessThanOrEqual(const int &group, const int &param,
                                     const int &les) const;

    QString ParamNotEqual(const int &group, const int &param,
                              const int &notEq) const;

    int ParamListSize(const int &group, const int &param) const;

    QString ParamListValue(const int &group, const int &param,
                               const int &option) const;

    QString ParamListBrief(const int &group, const int &param,
                               const int &option) const;

    QString ParamListDescription(const int &group, const int &param,
                                     const int &option) const;

    int ParamListExcludeSize(const int &group, const int &param,
                             const int &option) const;

    QString ParamListExclude(const int &group, const int &param,
                                 const int &option, const int &exclude) const;

    int ParamListIncludeSize(const int &group, const int &param,
                             const int &option) const;

    QString ParamListInclude(const int &group, const int &param,
                                 const int &option, const int &include) const;

    QString ParamDefault(const int &group, const int &param) const;

    QString ParamInternalDefault(const int &group, const int &param) const;

    QString ParamFilter(const int &group, const int &param) const;

    QString ParamPath(const int &group, const int &param) const;

    QString ParamFileMode(const int &group, const int &param) const;

    int ParamExcludeSize(const int &group, const int &param) const;

    QString ParamExclude(const int &group, const int &param,
                             const int &exclude) const;
    int ParamIncludeSize(const int &group, const int &param) const;

    QString ParamInclude(const int &group, const int &param,
                             const int &include) const;

    QString PixelType(const int &group, const int &param) const;

    int HelpersSize(const int &group, const int &param) const;

    QString HelperButtonName(const int &group, const int &param,
                                 const int &helper) const;

    QString HelperFunction(const int &group, const int &param,
                               const int &helper) const;

    QString HelperBrief(const int &group, const int &param,
                            const int &helper) const;

    QString HelperDescription(const int &group, const int &param,
                                  const int &helper) const;

    QString HelperIcon(const int &group, const int &param,
                           const int &helper) const;

    //! Verify whether Parameter name is in the Include list
    //! Used in creation of DefFile
    bool IsParamInPvlInclude(QString & paramName, std::vector<QString> & exclude);

    //! Create Pvl with the parameters in a user defined group given the Pvl object and group name
    void CreatePVL(Isis::Pvl &pvlDef , QString guiGrpName, QString pvlObjName,
                   QString pvlGrpName, std::vector<QString> & exclude);

    // Test all parameters for valid values and conditions
    void VerifyAll();

    bool StringToBool(QString value) const;

    void CommandLine(Isis::Pvl &lab) const;

    QString Version() const;


  protected:
    const IsisParameterData *ReturnParam(const QString &paramName) const;


  private:
    //! The XML file parser.
    XERCES::SAX2XMLReader *parser;
    //! The application handler.
    IsisXMLApplication *appHandler;

    // Member functions
    void StartParser(const char *xmlfile);


    void Verify(const IsisParameterData *param);

    void CheckFileNamePreference(QString filename, QString paramname);
};


#endif
