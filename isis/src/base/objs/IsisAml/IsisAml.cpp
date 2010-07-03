/**                                                                       
 * @file                                                                  
 * $Revision: 1.4 $                                                             
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
using namespace std;

#include <sstream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

#include "iException.h"
#include "IsisAml.h"
#include "IsisXMLChTrans.h"
#include "Filename.h"
#include "iString.h"

/**
 * @internal
 *   @todo This namespace needs documentation.
 */
namespace XERCES = XERCES_CPP_NAMESPACE;

/**                                                                       
 * Constructs an IsisAml object and internalizes the XML data in the given file 
 * name.
 * 
 * @param xmlfile Indicates the pull path of the XML file to be parsed.
 */                                                                       
IsisAml::IsisAml (const std::string &xmlfile) {
  StartParser (xmlfile.c_str());
}

/**                                                                       
 * Destructs an IsisAml object.                                     
 */                                                                       
IsisAml::~IsisAml () {
}

/**                                                                       
 * Allows the insertion of a value for any parameter. No validity check is 
 * performed on the value passed in.                                                         
 *                                                                        
 * @param paramName The partial or full name of the parameter to be modified.
 * @param value The string representation of the value to be placed in the 
 * parameters value data member. For parameters of type integer, the string 
 * must be convertable to an integer. For parameters of type double, the string 
 * must be convertable to a double. For parameters of type boolean, the string 
 * must be one of: (TRUE, FALSE, YES, NO, or a partial match of any of these 
 * beginning with the first character). 
 * 
 * @throws iException (IsisUserError) The parameter already has a value in its 
 * "value" data member.Overwriting an existing value is not allowed. Use "Clear"
 * to erase all values in the value data member instead of overwriting an 
 * existing value.
 */                                                                       
void IsisAml::PutAsString (const std::string &paramName,
                           const std::string &value) {

  IsisParameterData *param = const_cast <IsisParameterData *> (ReturnParam (paramName));

  if (param->values.size () > 0) {
    string message = "A value for this parameter [" + paramName + "] has "
                     "already been entered.";
    throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
  }

  param->values.clear();
  param->values.push_back (value);

}

/**                                                                       
 * Allows the insertion of a value for any parameter. No validity check is 
 * performed on the value passed in.                                                         
 *                                                                        
 * @param paramName The partial or full name of the parameter to be modified.
 * @param value The string vector representation of the value to be placed in 
 * the parameters value data member. For parameters of type integer, the string 
 * must be convertable to an integer. For parameters of type double, the string 
 * must be convertable to a double. For parameters of type boolean, the string 
 * must be one of: (TRUE, FALSE, YES, NO, or a partial match of any of these 
 * beginning with the first character).                                                                         
 */                                                                                                                                            
void IsisAml::PutAsString (const std::string &paramName,
                           const std::vector<std::string> &value) {

  IsisParameterData *param = const_cast <IsisParameterData *> (ReturnParam (paramName));

  if (param->values.size () > 0) {
    string message = "A value for this parameter [" + paramName + "] has "
                     "already been entered.";
    throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
  }

  param->values.resize(value.size());
  param->values = value;

}


// Public: Sets the value member of a parameter of type string whos name
// starts with paramName

/**                                                                       
 * Allows the insertion of a value for any parameter. No validity check         
 * is                                                                          
 * performed on the value passed in.                                                         
 *                                                                        
 * @param paramName The partial or full name of the parameter to be modified.
 * @param value The string representation of the value to be placed in the 
 * parameters value data member. For parameters of type integer, the         
 * string                                                                    
 * must be convertable to an integer. For parameters of type double, the string 
 * must be convertable to a double. For parameters of type boolean, the string 
 * must be one of: (TRUE, FALSE, YES, NO, or a partial match of any of these 
 * beginning with the first character).    
 *                                        
 */                                                                      
void IsisAml::PutString (const std::string &paramName, const std::string &value) {

  IsisParameterData *param = const_cast <IsisParameterData *> (ReturnParam (paramName));

  if (param->type != "string") {
    string message = "Parameter [" + paramName + "] is not a string.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  if (param->values.size () > 0) {
    string message = "A value for this parameter [" + paramName + "] has "
                     "already been saved (possibly by IsisGui). If you need to "
                     "change the value use \"Clear\" before the Put.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  param->values.clear();
  param->values.push_back (value);

  Verify (param);
}


// Public: Sets the value member of a parameter of type string whos name
// starts with paramName
/**                                                                       
 * Allows the insertion of a value for a parameter of type "string". A validity
 * check is performed on the value passed in, but all strings are allowed.                                                          
 *                                                                        
 * @param paramName The partial or full name of the parameter to be modified.
 * @param value The string value to be placed in the string's value data member.                                      
 */                                                                       
void IsisAml::PutString (const std::string &paramName,
                         const std::vector<std::string> &value) {

  IsisParameterData *param = const_cast <IsisParameterData *> (ReturnParam (paramName));

  if (param->type != "string") {
    string message = "Parameter [" + paramName + "] is not a string.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  if (param->values.size () > 0) {
    string message = "A value for this parameter [" + paramName + "] has "
                     "already been saved (possibly by IsisGui). If you need to "
                     "change the value use \"Clear\" before the Put.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  param->values.resize(value.size());
  param->values = value;

  Verify (param);
}


// Public: Sets the value member of a parameter of type filename whos name
// starts with paramName

/**                                                                       
 * Allows the insertion of a value for a parameter of type "filename". A
 * validity check is performed on the value passed in.                                                       
 *                                                                        
 * @param paramName The partial or full name of the parameter to be modified.
 * @param value The string representation of the value to be placed in the 
 * filename's value data member.
 */                                                                       
void IsisAml::PutFilename (const std::string &paramName,
                           const std::string &value) {

  IsisParameterData *param = const_cast <IsisParameterData *> (ReturnParam (paramName));

  if ((param->type != "filename") && (param->type != "cube")) {
    string message = "Parameter [" + paramName + "] is not a filename.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  if (param->values.size () > 0) {
    string message = "A value for this parameter [" + paramName + "] has "
                     "already been saved (possibly by IsisGui). If you need to "
                     "change the value use \"Clear\" before the Put.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  param->values.clear();
  param->values.push_back (value);

  Verify (param);
}


// Public: Sets the value member of a parameter of type filename whos name
// starts with paramName

/**                                                                       
 * Allows the insertion of a value for a parameter of type "filename". A 
 * validity check is performed on the value passed in.                                                     
 *                                                                        
 * @param paramName The partial or full name of the parameter to be modified.
 * @param value The string vector representation of the value to be placed in 
 * the filename's value data member.
 *                                                                        
 * @throws iException (IsisProgrammerError) The parameter is not of type 
 * "filename". 
 * @throws iException (IsisProgrammerError) The parameter already has a value 
 * in its "value" data member. Overwriting an existing value is not allowed. 
 * Use "Clear" to erase all values in the value data member instead of 
 * overwriting an existing value.
 */                                                                       
void IsisAml::PutFilename (const std::string &paramName,
                           const std::vector<std::string> &value) {

  IsisParameterData *param = const_cast <IsisParameterData *> (ReturnParam (paramName));

  if ((param->type != "filename") && (param->type != "cube")) {
    string message = "Parameter [" + paramName + "] is not a filename.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  if (param->values.size () > 0) {
    string message = "A value for this parameter [" + paramName + "] has "
                     "already been saved (possibly by IsisGui). If you need to "
                     "change the value use \"Clear\" before the Put.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  param->values.resize(value.size());
  param->values = value;

  Verify (param);
}


// Public: Sets the value member of a parameter of type integer whos name
// starts with paramName

/**                                                                       
 * Allows the insertion of a value for a parameter of type "integer". A 
 * validity check is performed on the value passed in.                                                          
 *                                                                        
 * @param paramName The partial or full name of the parameter to be modified.
 * @param value The integer value to be placed in the integer's value data 
 * member.
 *                                                                        
 * @throws iException (IsisProgrammerError) The parameter is not of type 
 * "int".                                      
 */                                                                       
void IsisAml::PutInteger (const std::string &paramName,
                          const int &value) {

  IsisParameterData *param = const_cast <IsisParameterData *> (ReturnParam (paramName));

  if (param->type != "integer") {
    string message = "Parameter [" + paramName + "] is not an integer.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  if (param->values.size () > 0) {
    string message = "A value for this parameter [" + paramName + "] has "
                     "already been saved (possibly by IsisGui). If you need to "
                     "change the value use \"Clear\" before the Put.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }
  
  param->values.clear();
  param->values.push_back (Isis::iString(value));

  Verify (param);
}


// Public: Sets the value member of a parameter of type integer whos name
// starts with paramName

/**                                                                       
 * Allows the insertion of a value for a parameter of type "integer". A 
 * validity check is performed on the value passed in.                                                          
 *                                                                        
 * @param paramName The partial or full name of the parameter to be modified.
 * @param value The integer vector to be placed in the integer's value data 
 * member.                    
 * 
 * @throws iException (IsisProgrammerError) The parameter already has a value 
 * in its "value" data member.Overwriting an existing value is not allowed. Use 
 * "Clear" to erase all values in the value data member instead of overwriting 
 * an existing value.
 * @throws iException (IsisProgrammerError) The parameter is not of type "int".
 */                                                                       
void IsisAml::PutInteger (const std::string &paramName,
                          const std::vector<int> &value) {

  IsisParameterData *param = const_cast <IsisParameterData *> (ReturnParam (paramName));

  if (param->type != "integer") {
    string message = "Parameter [" + paramName + "] is not an integer.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  if (param->values.size () > 0) {
    string message = "A value for this parameter [" + paramName + "] has "
                     "already been saved (possibly by IsisGui). If you need to "
                     "change the value use \"Clear\" before the Put.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  param->values.resize (value.size());
  for (unsigned int i = 0; i < value.size(); i++) {
    param->values[i] = Isis::iString(value[i]);
  }

  Verify (param);
}




// Public: Sets the value member of a parameter of type double whos name
// starts with paramName
/**                                                                       
 * Allows the insertion of a value for a parameter of type "double". A 
 * validity check is performed on the value passed in.                                                          
 *                                                                        
 * @param paramName The partial or full name of the parameter to be modified.
 * @param value The double value to be placed in the double's value data 
 * member.                    
 * 
 * @throws iException (IsisProgrammerError) The parameter already has a value 
 * in its "value" data member.Overwriting an existing value is not allowed. Use 
 * "Clear" to erase all values in the value data member instead of overwriting 
 * an existing value.
 * @throws iException (IsisProgrammerError) The parameter is not of type 
 * "double".
 */
void IsisAml::PutDouble (const std::string &paramName,
                         const double &value) {

  IsisParameterData *param = const_cast <IsisParameterData *> (ReturnParam (paramName));

  if (param->type != "double") {
    string message = "Parameter [" + paramName + "] is not a double.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  if (param->values.size () > 0) {
    string message = "A value for this parameter [" + paramName + "] has "
                     "already been saved (possibly by IsisGui). If you need to "
                     "change the value use \"Clear\" before the Put.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  param->values.clear();
  param->values.push_back (Isis::iString(value));

  Verify (param);
}


// Public: Sets the value member of a parameter of type double whos name
// starts with paramName
/**                                                                       
 * Allows the insertion of a value for a parameter of type "double". A 
 * validity check is performed on the value passed in.                                                          
 *                                                                        
 * @param paramName The partial or full name of the parameter to be modified.
 * @param value The double vector to be placed in the double's value data 
 * member.                    
 * 
 * @throws iException (IsisProgrammerError) The parameter already has a value 
 * in its "value" data member.Overwriting an existing value is not allowed. Use 
 * "Clear" to erase all values in the value data member instead of overwriting 
 * an existing value.
 * @throws iException (IsisProgrammerError) The parameter is not of type 
 * "double".
 */
void IsisAml::PutDouble (const std::string &paramName,
                         const std::vector<double> &value) {

  IsisParameterData *param = const_cast <IsisParameterData *> (ReturnParam (paramName));

  if (param->type != "double") {
    string message = "Parameter [" + paramName + "] is not a double.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  if (param->values.size () > 0) {
    string message = "A value for this parameter [" + paramName + "] has "
                     "already been saved (possibly by IsisGui). If you need to "
                     "change the value use \"Clear\" before the Put.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  param->values.resize (value.size());
  for (unsigned int i = 0; i < value.size(); i++) {
    param->values[i] = Isis::iString(value[i]);
  }

  Verify (param);
}



// Public: Sets the value member of a parameter of type boolean whos name
// starts with paramName
/**                                                                       
 * Allows the insertion of a value for a parameter of type "boolean". A 
 * validity check is performed on the value passed in.                                                          
 *                                                                        
 * @param paramName The partial or full name of the parameter to be modified.
 * @param value The boolean value to be placed in the boolean's value data 
 * member.                    
 * 
 * @throws iException (IsisProgrammerError) The parameter already has a value 
 * in its "value" data member.Overwriting an existing value is not allowed. Use 
 * "Clear" to erase all values in the value data member instead of overwriting 
 * an existing value.
 * @throws iException (IsisProgrammerError) The parameter is not of type 
 * "boolean".
 */
void IsisAml::PutBoolean (const std::string &paramName,
                          const bool &value) {

  IsisParameterData *param = const_cast <IsisParameterData *> (ReturnParam (paramName));

  if (param->type != "boolean") {
    string message = "Parameter [" + paramName + "] is not a boolean.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  if (param->values.size () > 0) {
    string message = "A value for this parameter [" + paramName + "] has "
                     "already been saved (possibly by IsisGui). If you need to "
                     "change the value use \"Clear\" before the Put.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  param->values.clear();
  if (value) {
    param->values.push_back ("YES");
  }
  else {
    param->values.push_back ("NO");
  }

  Verify (param);
}


// Public: Sets the value member of a parameter of type boolean whos name
// starts with paramName
/**                                                                       
 * Allows the insertion of a value for a parameter of type "boolean". A 
 * validity check is performed on the value passed in.                                                          
 *                                                                        
 * @param paramName The partial or full name of the parameter to be modified.
 * @param value The boolean vector to be placed in the boolean's value data 
 * member.                    
 * 
 * @throws iException (IsisProgrammerError) The parameter already has a value 
 * in its "value" data member.Overwriting an existing value is not allowed. Use 
 * "Clear" to erase all values in the value data member instead of overwriting 
 * an existing value.
 * @throws iException (IsisProgrammerError) The parameter is not of type 
 * "boolean".
 */
void IsisAml::PutBoolean (const std::string &paramName,
                          const std::vector<bool> &value) {

  IsisParameterData *param = const_cast <IsisParameterData *> (ReturnParam (paramName));

  if (param->type != "boolean") {
    string message = "Parameter [" + paramName + "] is not a boolean.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  if (param->values.size () > 0) {
    string message = "A value for this parameter [" + paramName + "] has "
                     "already been saved (possibly by IsisGui). If you need to "
                     "change the value use \"Clear\" before the Put.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  param->values.resize (value.size());
  for (unsigned int i = 0; i < value.size(); i++) {
    if (value[i]) {
      param->values.push_back ("YES");
    }
    else {
      param->values.push_back ("NO");
    }
  }

  Verify (param);
}


// Accessor methods for getting the value(s) of a parameter

// Public: Returns the first element of the value member of a parameter whos
// name starts with paramName as a string. Any type can be retrieve with this member.
/**                                                                       
 * Allows the retrieval of a value for a parameter of any type. The value will 
 * be returned as a string no matter what the parameter type is.                                                   
 *                                                                        
 * @param paramName The partial or full name of the parameter to be retrieved.                 
 *                                                                        
 * @return A string representation of the value for the specified parameter.
 * 
 * @throws iException (IsisProgrammerError) The parameter has no value.                                       
 */                                                                       
string IsisAml::GetAsString (const std::string &paramName) const {

  const IsisParameterData *param = ReturnParam (paramName);
  string value;
  if (param->values.size() == 0) {
    if (param->defaultValues.size() == 0) {
      string message = "Parameter [" + paramName + "] has no value.";
      throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
    }
    else {
      value = param->defaultValues[0];
    }
  }
  else {
    value = param->values[0];
  }

  return value;
}

// Public: Returns the value member of a parameter whos name starts with paramName
// as a vector<string>
/**                                                                       
 * Allows the retrieval of a value for a parameter of any type. The value will 
 * be returned as a string no matter what the parameter type is.                                                   
 *                                                                        
 * @param paramName The partial or full name of the parameter to be retrieved.
 * @param values The value member of the parameter whose name starts with 
 * paramName.                
 * 
 * @throws iException (IsisProgrammerError) The parameter has no value.                                       
 */ 
void IsisAml::GetAsString (const std::string &paramName,
                           std::vector<std::string> &values) const{

  const IsisParameterData *param = ReturnParam (paramName);

  values.clear();
  string value;
  if (param->values.size() == 0) {
    if (param->defaultValues.size() == 0) {
      string message = "Parameter [" + paramName + "] has no value.";
      throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
    }
    else {
      for (unsigned int i = 0; i<param->defaultValues.size(); i++)
        values.push_back (param->defaultValues[i]);
    }
  }
  else {
    for (unsigned int i = 0; i<param->values.size(); i++)
      values.push_back (param->values[i]);
  }

  return;
}


// Public: Returns the first element of the value member of a parameter whos
// name starts with paramName as a string/filename
/**                                                                       
 * Allows the retrieval of a value for a parameter of type "filename".                                                        
 * 
 * @param paramName The partial or full name of the parameter to be retrieved.                                                                        
 * @param extension A default extension to add if it does not already exist on 
 * the file name.  For example, "txt" will make /mydir/myfile into 
 * /mydir/myfile.txt              
 *                                                                        
 * @return The value of the parameter.                                       
 */                                                                       
string IsisAml::GetFilename (const std::string &paramName, std::string extension) const {

  const IsisParameterData *param = ReturnParam (paramName);

  if ((param->type != "filename") && (param->type != "cube")) {
    string message = "Parameter [" + paramName + "] is not a filename.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  string value;
  if (param->values.size() == 0) {
    if (param->defaultValues.size() == 0) {
      string message = "Parameter [" + paramName + "] has no value.";
      throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
    }
    else {
      value = param->defaultValues[0];
    }
  }
  else {
    value = param->values[0];
  }

  Isis::Filename name(value);
  if (extension != "") name.AddExtension(extension);
  value = name.Expanded();

  return value;
}


// Public: Returns the value member of a parameter whos name starts with paramName
// as a vector<string/filename>
/**                                                                       
 * Allows the retrieval of a value for a parameter of type "filename".                                                        
 * 
 * @param paramName The partial or full name of the parameter to be retrieved.                                                                        
 * @param values The value membet of the parameter whose name starts with 
 * paramName.                                             
 */
void IsisAml::GetFilename (const std::string &paramName,
                           std::vector<std::string> &values) const {

  const IsisParameterData *param = ReturnParam (paramName);

  if ((param->type != "filename") && (param->type != "cube")) {
    string message = "Parameter [" + paramName + "] is not a filename.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  values.clear();
  if (param->values.size() == 0) {
    if (param->defaultValues.size() == 0) {
      string message = "Parameter [" + paramName + "] has no value.";
      throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
    }
    else {
      for (unsigned int i=0; i<param->defaultValues.size(); i++) {
        Isis::Filename name(param->defaultValues[i]);
        values.push_back (name.Expanded());
      }
    }
  }
  else {
    for (unsigned int i=0; i<param->values.size(); i++) {
      Isis::Filename name(param->values[i]);
      values.push_back (name.Expanded());
    }
  }

  return;
}


// Public: Returns the first element of the value member of a parameter whos
// name starts with paramName as a string
/**                                                                       
 * Allows the retrieval of a value for a parameter of type "string".                                       
 * 
 * @param paramName The partial or full name of the parameter to be retrieved.                                  
 *                                                                        
 * @return The value of the parameter. 
 *
 * @throws iException::Programmer (IsisErrorUser) The parameter has no value.                                  
 */
string IsisAml::GetString (const std::string &paramName) const {

  const IsisParameterData *param = ReturnParam (paramName);
  Isis::iString value;

  if (param->type != "string") {
    string message = "Parameter [" + paramName + "] is not a string.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  if (param->values.size() == 0) {
    if (param->defaultValues.size() == 0) {
      string message = "Parameter [" + paramName + "] has no value.";
      throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
    }
    else {
      value = param->defaultValues[0];
    }
  }
  else {
    value = param->values[0];
    // If there is a list of legal values return the list option that matches
    // or begins with what was entered rather than exactly what was entered
    if (param->listOptions.size () > 0) {
      value = value.UpCase();
      int found = -1;
      for (unsigned int p=0; p<param->listOptions.size (); p++) {
        Isis::iString option = param->listOptions[p].value;
        option = option.UpCase();
        if (value == option) {
          return value;
        }
        else if (value.compare(0, min(value.size(), option.size()),
                               option ,0, min(value.size(), option.size())) == 0) {
          if (found != -1) {
            string message = "Value [" + value + "] for parameter [" +
                             paramName + "] is not unique.";
            throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
          }
          found = p;
        }
      }
      return param->listOptions[found].value;
    }

    // Just return what is in the value
    else {
      value = param->values[0];
    }
  }
  return value;
}


// Public: Returns the value member of a parameter whos name starts with paramName
// as a vector<string>
/**                                                                       
 * Allows the retrieval of a value for a parameter of type "string".  
 * 
 * @param paramName The partial or full name of the parameter to be retrieved.
 *                          
 * @param values The value member of a parameter whose name starts with 
 *                paramName.
 *
 * @throws iException::Programmer (IsisErrorUser) The parameter has no value.
 */
void IsisAml::GetString (const std::string &paramName,
                         std::vector<std::string> &values) const {

  const IsisParameterData *param = ReturnParam (paramName);

  if (param->type != "string") {
    string message = "Parameter [" + paramName + "] is not a string.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  values.clear();
  string value;
  if (param->values.size() == 0) {
    if (param->defaultValues.size() == 0) {
      string message = "Parameter [" + paramName + "] has no value.";
      throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
    }
    else {
      for (unsigned int i = 0; i<param->defaultValues.size(); i++)
        values.push_back (param->defaultValues[i]);
    }
  }
  else {
    for (unsigned int i = 0; i<param->values.size(); i++)
      values.push_back (param->values[i]);
  }

  return;
}



// Public: Returns the first element of the value member of a parameter whos
// name starts with paramName as an integer
/**                                                                       
 * Allows the retrieval of a value for a parameter of type "integer".          
 * 
 * @param paramName The partial or full name of the parameter to be retrieved.                             
 *                                                                        
 * @return The value of the parameter. 
 *
 * @throws iException::Programmer (IsisErrorUser) The parameter has no value.
 */
int IsisAml::GetInteger (const std::string &paramName) const {

  const IsisParameterData *param = ReturnParam (paramName);

  if (param->type != "integer") {
    string message = "Parameter [" + paramName + "] is not an integer.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  Isis::iString value;
  if (param->values.size() == 0) {
    if (param->defaultValues.size() == 0) {
      string message = "Parameter [" + paramName + "] has no value.";
      throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
    }
    else {
      value = param->defaultValues[0];
    }
  }
  else {
    value = param->values[0];
  }

  return value.ToInteger();
}


// Public: Returns the value member of a parameter whos name starts with paramName
// as a vector<int>
/**                                                                       
 * Allows the retrieval of a value for a parameter of type "integer".
 * 
 * @param paramName The partial or full name of the parameter to be retrieved.
 *
 * @param values The value member of a parameter whose name starts with 
 * paramName.                                                                                     
 *
 * @throws iException::Programmer (IsisErrorUser) The parameter has no value.
 */
void IsisAml::GetInteger (const std::string &paramName,
                          std::vector<int> &values) const {

  const IsisParameterData *param = ReturnParam (paramName);

  if (param->type != "integer") {
    string message = "Parameter [" + paramName + "] is not an integer.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  values.clear();
  Isis::iString value;
  if (param->values.size() == 0) {
    if (param->defaultValues.size() == 0) {
      string message = "Parameter [" + paramName + "] has no value.";
      throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
    }
    else {
      for (unsigned int i = 0; i<param->defaultValues.size(); i++)
        value = param->defaultValues[i];
        values.push_back (value.ToInteger());
    }
  }
  else {
    for (unsigned int i = 0; i<param->values.size(); i++)
      value = param->values[i];
      values.push_back (value.ToInteger());
  }

  return;
}



  // Public: Returns the first element of the value member of a parameter whos
  // name starts with paramName as a doubble
/**                                                                       
 * Allows the retrieval of a value for a parameter of type "double".
 * 
 * @param paramName The partial or full name of the parameter to be retrieved.                                 
 *                                                                        
 * @return The value of the parameter. 
 *
 * @throws iException::Programmer (IsisErrorUser) The parameter has no value.
 */
double IsisAml::GetDouble (const std::string &paramName) const {

  const IsisParameterData *param = ReturnParam (paramName);

  if (param->type != "double") {
    string message = "Parameter [" + paramName + "] is not a double.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  Isis::iString value;
  if (param->values.size() == 0) {
    if (param->defaultValues.size() == 0) {
      string message = "Parameter [" + paramName + "] has no value.";
      throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
    }
    else {
      value = param->defaultValues[0];
    }
  }
  else {
    value = param->values[0];
  }

  return value.ToDouble();
}

// Public: Returns the value member of a parameter whos name starts with paramName
// as a vector<doubble>
/**                                                                       
 * Allows the retrieval of a value for a parameter of type "double".
 * 
 * @param paramName The partial or full name of the parameter to be retrieved.
 *                                 
 * @param values The value member of a parameter whose name starts with 
 *                paramName. 
 *
 * @throws iException::Programmer (IsisErrorUser) The parameter has no value.
 */
void IsisAml::GetDouble (const std::string &paramName,
                         std::vector<double> &values) const {

  const IsisParameterData *param = ReturnParam (paramName);

  if (param->type != "double") {
    string message = "Parameter [" + paramName + "] is not a double.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  values.clear();
  Isis::iString value;
  if (param->values.size() == 0) {
    if (param->defaultValues.size() == 0) {
      string message = "Parameter [" + paramName + "] has no value.";
      throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
    }
    else {
      for (unsigned int i = 0; i<param->defaultValues.size(); i++)
        value = param->defaultValues[i];
        values.push_back (value.ToDouble());
    }
  }
  else {
    for (unsigned int i = 0; i<param->values.size(); i++)
      value = param->values[i];
      values.push_back (value.ToDouble());
  }

  return;
}


// Public: Returns the first element of the value member of a parameter whos
// name starts with paramName as a bool
/**                                                                       
 * Allows the retrieval of a value for a parameter of type "boolean".
 * 
 * @param paramName The partial or full name of the parameter to be retrieved.                                  
 *                                                                        
 * @return The value of the parameter. 
 *
 * @throws iException::Programmer (IsisErrorUser) The parameter has no value.
 */
bool IsisAml::GetBoolean (const std::string &paramName) const {

  const IsisParameterData *param = ReturnParam (paramName);

  if (param->type != "boolean") {
    string message = "Parameter [" + paramName + "] is not a boolean.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  Isis::iString value;
  if (param->values.size() == 0) {
    if (param->defaultValues.size() == 0) {
      string message = "Parameter [" + paramName + "] has no value.";
      throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
    }
    else {
      value = param->defaultValues[0];
    }
  }
  else {
    value = param->values[0];
  }

  return StringToBool (value);

}


// Public: Returns the value member of a parameter whos name starts with paramName
// as a vector<bool>
/**                                                                       
 * Allows the retrieval of a value for a parameter of type "boolean".            
 * 
 * @param paramName The partial or full name of the parameter to be retrieved. 
 *                                
 * @param values The member value of a parameter whose name starts with 
 * paramName.. 
 *
 * @throws iException::Programmer (IsisErrorUser) The parameter has no value. 
 */
void IsisAml::GetBoolean (const std::string &paramName,
                          std::vector<bool> &values) const {

  const IsisParameterData *param = ReturnParam (paramName);

  if (param->type != "boolean") {
    string message = "Parameter [" + paramName + "] is not a boolean.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  values.clear();
  vector <string> value;
  Isis::iString tmp;
  if (param->values.size() == 0) {
    if (param->defaultValues.size() == 0) {
      string message = "Parameter [" + paramName + "] has no value.";
      throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
    }
    else {
      for (unsigned int i = 0; i<param->defaultValues.size(); i++) {
        tmp = param->defaultValues[i];
        tmp.UpCase();
        value.push_back (tmp);
      }
    }
  }
  else {
    for (unsigned int i = 0; i<param->values.size(); i++) {
      tmp = param->values[i];
      tmp.UpCase();
      value.push_back (tmp);
    }
  }

  for (unsigned int i = 0; i<value.size(); i++) {
    values.push_back (StringToBool (value[i]));

  }

  return;
}
/**                                                                       
 * Returns the Program name.                                                                            
 *                                                                        
 * @return The name of the program.                                      
 */                                                                       
string IsisAml::ProgramName () const {
  string tmp = name;
  return tmp;
}


/**                                                                       
 * Returns the brief description of the program.         
 *                                                                        
 * @return The brief description.                                      
 */ 
string IsisAml::Brief () const {
  return brief;
}


/**                                                                       
 * Returns the full description of the program.  
 *                                                                        
 * @return The full description.                                      
 */ 
string IsisAml::Description () const {
  return description;
}

/**                                                                       
 * Returns the number of groups found in the XML.      
 *                                                                        
 * @return The number of groups.                                      
 */ 
int IsisAml::NumGroups () const {
  return groups.size();
}

/**                                                                       
 * Returns the group name of group[index]. 
 *
 * @param index The array index of the group.       
 *                                                                         
 * @return The group name.                                      
 */ 
string IsisAml::GroupName (const int &index) const {
  string s = groups[index].name;
  return s;
}

/**                                                                       
 * Returns the number of parameters in a group.                                                         
 * 
 * @param group The group to measure.
 *                                                                       
 * @return The number of parameters.                                      
 */ 
int IsisAml::NumParams (const int &group) const {
  return groups[group].parameters.size();
}

/**                                                                       
 * Returns the parameter name.
 *
 * @param group The group index where the parameter can be found.
 * @param param The index of the parameter to name.                                                     
 *                                                                        
 * @return The name of the parameter.                                      
 */ 
string IsisAml::ParamName (const int &group, const int &param) const {
   string s = groups[group].parameters[param].name;
  return s;
}

/**                                                                       
 * Returns the brief description of a parameter in a specified group.
 * 
 * @param group The group index
 * @param param The parameter index
 *                                                                        
 * @return The brief description.                                      
 */ 
string IsisAml::ParamBrief (const int &group, const int &param) const {
  string s = groups[group].parameters[param].brief;
 return s;
}

/** 
 * Returns the long description of a parameter in a specified group.
 * 
 * @param group The group index
 * @param param The parameter index
 * 
 * @return The long description
 */
string IsisAml::ParamDescription (const int &group, const int &param) const {
  string s = groups[group].parameters[param].description;
  return s;
}

/** 
 * Returns the minimum value of a parameter in a specified group.
 * 
 * @param group The group index
 * @param param The parameter index
 * 
 * @return The minimum
 */
string IsisAml::ParamMinimum (const int &group, const int &param) const {
  string s = groups[group].parameters[param].minimum;
  return s;
}

/** 
 * Returns the maximum value of a parameter in a specified group.
 * 
 * @param group The group index
 * @param param The parameter index
 * 
 * @return The maximum
 */
string IsisAml::ParamMaximum (const int &group, const int &param) const {
  string s = groups[group].parameters[param].maximum;
  return s;
}

/** 
 * Returns whether the minimum value is inclusive or not.
 * 
 * @param group The group index
 * @param param The parameter index
 * 
 * @return Whether minimum is inclusive or not
 */
string IsisAml::ParamMinimumInclusive (const int &group, const int &param) const {
  string s = groups[group].parameters[param].minimum_inclusive;
  return s;
}

/** 
 * Returns whether the maximum value is inclusive or not.
 * 
 * @param group The group index
 * @param param The parameter index
 * 
 * @return Whether maximum is inclusive or not
 */
string IsisAml::ParamMaximumInclusive (const int &group, const int &param) const {
  string s = groups[group].parameters[param].maximum_inclusive;
  return s;
}

/** 
 * Returns whether the selected parameter has a restriction on odd values or 
 * not.
 * 
 * @param group The group index
 * @param param The parameter index
 * 
 * @return Whether the parameter restricts odd values or not
 */
string IsisAml::ParamOdd (const int &group, const int &param) const {
  string s = groups[group].parameters[param].odd;
  return s;
}

/** 
 * Returns the number of values in the parameters greater than list.
 * 
 * @param group The group index
 * @param param The parameter index
 * 
 * @return Number of values in the parameters greater than list
 */
int IsisAml::ParamGreaterThanSize (const int &group, const int &param) const {
  return groups[group].parameters[param].greaterThan.size();
}

/** 
 * Returns the number of values in the parameters greater than or equal list.
 * 
 * @param group The group index
 * @param param The parameter index
 * 
 * @return Number of values in the parameters greater than or equal list
 */
int IsisAml::ParamGreaterThanOrEqualSize (const int &group, 
                                             const int &param) const {
  return groups[group].parameters[param].greaterThanOrEqual.size();
}

/** 
 * Returns the number of values in the parameters less than list.
 * 
 * @param group The group index
 * @param param The parameter index
 * 
 * @return Number of values in the parameters less than list
 */
int IsisAml::ParamLessThanSize (const int &group, const int &param) const {
  return groups[group].parameters[param].lessThan.size();
}

/** 
 * Returns the number of values in the parameters less than or equal list.
 * 
 * @param group The group index
 * @param param The parameter index
 * 
 * @return Number of values in the parameters less than or equal list
 */
int IsisAml::ParamLessThanOrEqualSize (const int &group, 
                                          const int &param) const {
  return groups[group].parameters[param].lessThanOrEqual.size();
}

/** 
 * Returns the number of values in the not equal list.
 * 
 * @param group The group index
 * @param param The parameter index
 * 
 * @return Number of values in the parameters not equal list
 */
int IsisAml::ParamNotEqualSize (const int &group, const int &param) const {
  return groups[group].parameters[param].notEqual.size();
}

/**
 * Returns the name of the specified greaterThan parameter
 * 
 * @param group The group index
 * @param param The parameter index
 * @param great The greaterThan index
 * 
 * @return The name of the greaterThan parameter
 */
string IsisAml::ParamGreaterThan (const int &group, const int &param,
                     const int &great) const {
  string s = groups[group].parameters[param].greaterThan[great];
  return s;
}

/**
 * Returns the name of the specified greaterThanOrEqual parameter
 * 
 * @param group The group index
 * @param param The parameter index
 * @param great The greaterThanOrEqual index
 * 
 * @return The name of the greaterThanOrEqual parameter
 */
string IsisAml::ParamGreaterThanOrEqual (const int &group, const int &param,
                     const int &great) const {
  string s = groups[group].parameters[param].greaterThanOrEqual[great];
  return s;
}

/**
 * Returns the name of the specified lessThan parameter
 * 
 * @param group The group index
 * @param param The parameter index
 * @param les The lessThan index
 * 
 * @return The name of the lessThan parameter
 */
string IsisAml::ParamLessThan (const int &group, const int &param,
                     const int &les) const {
  string s = groups[group].parameters[param].lessThan[les];
  return s;
}

/**
 * Returns the name of the specified lessThanOrEqual parameter
 * 
 * @param group The group index
 * @param param The parameter index
 * @param les The lessThanOrEqual index
 * 
 * @return The name of the lessThanOrEqual parameter
 */
string IsisAml::ParamLessThanOrEqual (const int &group, const int &param,
                     const int &les) const {
  string s = groups[group].parameters[param].lessThanOrEqual[les];
  return s;
}

/**
 * Returns the name of the specified notEqual parameter
 * 
 * @param group The group index
 * @param param The parameter index
 * @param notEq The notEqual index
 * 
 * @return The name of the notEqual parameter
 */
string IsisAml::ParamNotEqual (const int &group, const int &param,
                     const int &notEq) const {
  string s = groups[group].parameters[param].notEqual[notEq];
  return s;
}

/**
 * Returns the name of the specified excluded parameter
 * 
 * @param group The group index
 * @param param The parameter index
 * @param exclude The exclude index
 * 
 * @return The name of the excluded parameter
 */
string IsisAml::ParamExclude (const int &group, const int &param,
                     const int &exclude) const {
  string s = groups[group].parameters[param].exclude[exclude];
  return s;
}

/**
 * Returns the name of the specified included parameter
 * 
 * @param group The group index
 * @param param The parameter index
 * @param include The include index
 * 
 * @return The name of the included parameter
 */
string IsisAml::ParamInclude (const int &group, const int &param,
                     const int &include) const {
  string s = groups[group].parameters[param].include[include];
  return s;
}


/**                                                                       
 * Returns the parameter type of a parameter in a specified group.
 * 
 * @param group The group index
 * @param param The parameter index
 *                                                                        
 * @return The type of parameter.                                      
 */ 
string IsisAml::ParamType (const int &group, const int &param) const {
   string s = groups[group].parameters[param].type;
  return s;
}

/**                                                                       
 * Returns the default for a parameter in a specified group.
 * 
 * @param group The group index
 * @param param The parameter index
 *                                                                        
 * @return The default for the specified parameter.                                      
 */ 
string IsisAml::ParamDefault (const int &group, const int &param) const {
  string s;
  if (groups[group].parameters[param].defaultValues.size() == 0) {
    s = "";
  }
  else {
    s = groups[group].parameters[param].defaultValues[0];
  }
  return s;
}

/**                                                                       
 * Returns the internal default for a parameter in a specified group
 *
 *.@param group The group index
 * @param param The parameter index
 *                                                                        
 * @return The internal default for the specified parameter.                                      
 */ 
string IsisAml::ParamInternalDefault (const int &group, const int &param) const {
  string s;
  if (groups[group].parameters[param].internalDefault.size() == 0) {
    s = "";
  }
  else {
    s = groups[group].parameters[param].internalDefault;
  }
  return s;
}

/**                                                                       
 * Returns the parameter filter for a parameter in a specified group.
 * 
 * @param group The group index
 * @param param The parameter index
 *                                                                        
 * @return The paramter filter.                                      
 */ 
string IsisAml::ParamFilter (const int &group, const int &param) const {
  string s;
  if (groups[group].parameters[param].filter.size() == 0) {
    s = "";
  }
  else {
    s = groups[group].parameters[param].filter;
  }
  return s;
}

/**                                                                       
 * Returns the default path for a filename/cube parameter
 *
 * @param group The group index
 * @param param The parameter index                                                                            
 *                                                                        
 * @return The default path.                                      
 */ 
string IsisAml::ParamPath (const int &group, const int &param) const {
  string s;
  if (groups[group].parameters[param].path.size() == 0) {
    s = "";
  }
  else {
    s = groups[group].parameters[param].path;
  }
  return s;
}

/**                                                                       
 * Returns the file mode for a parameter in a specified group.
 * 
 * @param group The group index
 * @param param The parameter index
 *                                                                        
 * @return The file mode for the parameter.                                      
 */ 
string IsisAml::ParamFileMode (const int &group, const int &param) const {
  string s;
  if (groups[group].parameters[param].fileMode.size() == 0) {
    s = "";
  }
  else {
    s = groups[group].parameters[param].fileMode;
  }
  return s;
}

/**                                                                       
 * Returns the number of options in the specified parameter's list.
 * 
 * @param group The group index
 * @param param The parameter index
 *                                                                        
 * @return The number of options contained in the parameter's list. If the 
 * parameter does not contain a list, zero will be returned.                                      
 */
int IsisAml::ParamListSize (const int &group, const int &param) const {
  return groups[group].parameters[param].listOptions.size();
}


/**                                                                       
 * Returns the option value for a specific option to a parameter.
 * 
 * @param group The group index
 * @param param The parameter index
 * @param option The option number within the parameters list.
 *                                                                        
 * @return The value of the option.                                      
 */
string IsisAml::ParamListValue (const int &group, const int &param,
                                const int &option) const {
  string s = groups[group].parameters[param].listOptions[option].value;
  return s;
}

/**                                                                       
 * Returns the brief description for a specific option to a parameter.
 * 
 * @param group The group index
 * @param param The parameter index
 * @param option The option number within the parameters list.
 *                                                                        
 * @return The brief description of the option.                                      
 */
string IsisAml::ParamListBrief (const int &group, const int &param,
                                const int &option) const {
  string s = groups[group].parameters[param].listOptions[option].brief;
  return s;
}

/**                                                                       
 * Returns the full description for a specific option to a parameter.
 * 
 * @param group The group index
 * @param param The parameter index
 * @param option The option number within the parameters list.
 *                                                                        
 * @return The full description of the option.                                      
 */
string IsisAml::ParamListDescription (const int &group, const int &param,
                                      const int &option) const {
  string s = groups[group].parameters[param].listOptions[option].description;
  return s;
}

/**                                                                       
 * Returns the number of items in a parameters list exclude section.
 * 
 * @param group The group index
 * @param param The parameter index
 * @param option The option number within the parameters list.
 *                                                                        
 * @return The number of items in the parameters list exclude section.                                      
 */
int IsisAml::ParamListExcludeSize (const int &group, const int &param,
                                   const int &option) const {
  return groups[group].parameters[param].listOptions[option].exclude.size();
}

/**                                                                       
 * Returns the parameter name to be excluded if this option is selected.
 * 
 * @param group The group index
 * @param param The parameter index
 * @param option The option number within the parameters list.
 * @param exclude The exclusion number within the parameters list.
 *                                                                        
 * @return The parameter name to be excluded if this option is selected.                                      
 */
string IsisAml::ParamListExclude (const int &group, const int &param,
                                  const int &option, const int &exclude) const {
  string s = groups[group].parameters[param].listOptions[option].exclude[exclude];
  return s;
}

/**                                                                       
 * Returns the number of items in a parameters list include section.
 * 
 * @param group The group index
 * @param param The parameter index
 * @param option The option number within the parameters list.
 *                                                                        
 * @return The number of items in the parameters list include section.                                      
 */
int IsisAml::ParamListIncludeSize (const int &group, const int &param,
                                   const int &option) const {
  return groups[group].parameters[param].listOptions[option].include.size();
}

/**                                                                       
 * Returns the parameter name to be included if this option is selected.
 * 
 * @param group The group index
 * @param param The parameter index
 * @param option The option number within the parameters list.
 * @param include The inclusion number within the parameters list.
 *                                                                        
 * @return The parameter name to be included if this option is selected.                                      
 */
string IsisAml::ParamListInclude (const int &group, const int &param,
                                  const int &option, const int &include) const {
  string s = groups[group].parameters[param].listOptions[option].include[include];
  return s;
}

/**
 * Returns the number of parameters excluded in this parameter's exclusions
 * 
 * @param group The group index
 * @param param The parameter index
 * 
 * @return The number of items in the parameters exclude list
 */
int IsisAml::ParamExcludeSize (const int &group, const int &param) const {
  return groups[group].parameters[param].exclude.size();
}

/**
 * Returns the number of parameters included in this parameter's inclusions
 * 
 * @param group The group index
 * @param param The parameter index
 * 
 * @return The number of items in the parameters include list
 */
int IsisAml::ParamIncludeSize (const int &group, const int &param) const {
  return groups[group].parameters[param].include.size();
}

/**
 * Returns the default pixel type from the XML
 * 
 * @param group The group index
 * @param param The parameter index
 * 
 * @return The default pixel type
 */
string IsisAml::PixelType (const int &group, const int &param) const {
  return groups[group].parameters[param].pixelType;
}

/**
 * Returns the number of helpers the parameter has 
 *  
 * @param group The group index
 * @param param The parameter index
 * 
 * @return int The number of helpers the parameter has
 */
int IsisAml::HelpersSize (const int &group, const int &param) const {
  return groups[group].parameters[param].helpers.size();
}

/**
 * Returns the name of the helper button
 *   
 * @param group The group index
 * @param param The parameter index
 * @param helper The helper index
 * 
 * @return string The name of the helper
 */
string IsisAml::HelperButtonName (const int &group, const int &param, 
                               const int &helper) const {
  return groups[group].parameters[param].helpers[helper].name;
}

/**
 * Returns the name of the helper function 
 *  
 * @param group The group index
 * @param param The parameter index
 * @param helper The helper index
 * 
 * @return string The name of the helper function
 */
string IsisAml::HelperFunction (const int &group, const int &param, 
                             const int &helper) const {
  return groups[group].parameters[param].helpers[helper].function;
}

/**
 * Returns the brief description of the helper button 
 *  
 * @param group The group index
 * @param param The parameter index
 * @param helper The helper index
 * 
 * @return string The brief description of the helper button
 */
string IsisAml::HelperBrief (const int &group, const int &param, 
                             const int &helper) const {
  return groups[group].parameters[param].helpers[helper].brief;
}

/**
 * Returns the long description of the helper button 
 *  
 * @param group The group index
 * @param param The parameter index
 * @param helper The helper index
 * 
 * @return string The long description of the helper button
 */
string IsisAml::HelperDescription (const int &group, const int &param, 
                                   const int &helper) const {
  return groups[group].parameters[param].helpers[helper].description;
}

/**
 * Returns the name of the icon for the helper button
 *  
 * @param group The group index
 * @param param The parameter index
 * @param helper The helper index
 * 
 * @return string The name of the helper icon
 */
string IsisAml::HelperIcon (const int &group, const int &param, 
                         const int &helper) const {
  return groups[group].parameters[param].helpers[helper].icon;
}

/**
 * Returns a true if the parameter has a value, and false if it does not
 * 
 * @param paramName The name of the parameter to check if it was entered 
 * 
 * @return True if the parameter was entered, and false if it was not
 */
bool IsisAml::WasEntered (const std::string &paramName) const {

  const IsisParameterData *param = ReturnParam (paramName);

  if (param->values.size() == 0) {
    return false;
  }

  return true;
}

/**
 * Clears the value(s) in the named parameter
 * 
 * @param paramName The name of the parameter to clear
 */
void IsisAml::Clear (const std::string &paramName) {

  IsisParameterData *param = const_cast <IsisParameterData *> (ReturnParam (paramName));
  param->values.clear();

  param->outCubeAtt.Reset();
  param->outCubeAtt.Set("+"+param->pixelType);
  param->inCubeAtt.Reset();

  return;
}


/**
 * Gets the attributes for an input cube
 * 
 * @param paramName The name of the parameter to get the attributes for
 * 
 * @return CubeAttributeInput
 * 
 * @throws iException::Programmer (Parameter is not a cube)
 * @throws iException::Programmer (Parameter does nto contain an input file)
 */
Isis::CubeAttributeInput& IsisAml::GetInputAttribute (const std::string &paramName) {

  IsisParameterData *param = const_cast <IsisParameterData *> (ReturnParam (paramName));

  if (param->type != "cube") {
    string message = "Parameter [" + paramName + "] is not a cube.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  string value;
  if (param->values.size() == 0) {
    if (param->defaultValues.size() == 0) {
      value.clear();
//      string message = "Parameter [" + paramName + "] has no value.";
//      throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
    }
    else {
      value = param->defaultValues[0];
    }
  }
  else {
    value = param->values[0];
  }
  if (param->fileMode == "input") {
    param->inCubeAtt.Reset();
    param->inCubeAtt.Set(value);
  }
  else {
    string message = "Parameter [" + paramName + "] does not contain an [input] file";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }
  return param->inCubeAtt;
}

/**
 * Gets the attributes for an output cube
 * 
 * @param paramName The name of the parameter to get the attributes for
 * 
 * @return CubeAttributeOutput
 * 
 * @throws iException::Programmer (Parameter is not a cube)
 * @throws iException::Programmer (Parameter does nto contain an output file)
 */
Isis::CubeAttributeOutput& IsisAml::GetOutputAttribute (const std::string &paramName) {

  IsisParameterData *param = const_cast <IsisParameterData *> (ReturnParam (paramName));

  if (param->type != "cube") {
    string message = "Parameter [" + paramName + "] is not a cube.";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }

  string value;
  if (param->values.size() == 0) {
    if (param->defaultValues.size() == 0) {
      value.clear();
//      string message = "Parameter [" + paramName + "] has no value.";
//      throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
    }
    else {
      value = param->defaultValues[0];
    }
  }
  else {
    value = param->values[0];
  }
  if (param->fileMode == "output") {
    param->outCubeAtt.Reset();
    param->outCubeAtt.Set("+"+param->pixelType);
    param->outCubeAtt.Set(value);
  }
  else {
    string message = "Parameter [" + paramName + "] does not contain an [output] file";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }
  return param->outCubeAtt;
}

/**
 * Returns a pointer to a parameter whos name starts with paramName
 * 
 * @param paramName The name of the parameter to return
 * 
 * @return IsisParameterData
 * 
 * @throws iException::User (Parameter is not unique)
 * @throws iException::User (Unknown Parameter)
 */
const IsisParameterData *IsisAml::ReturnParam (const std::string &paramName) const {
  Isis::iString pn = paramName;
  pn.UpCase();
  int found = 0;
  bool exact = false;
  const IsisParameterData *param = NULL;
  Isis::iString cur_pn;

  for (unsigned int g=0; g<groups.size(); g++) {
    for (unsigned int p=0; p<groups[g].parameters.size(); p++) {
      cur_pn = groups[g].parameters[p].name;
      cur_pn.UpCase();
      if (cur_pn.find(pn) == 0) {
        if (cur_pn == pn) {
          if (exact) {
            string message = "Parameter [" + paramName + "] is not unique.";
            throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
          }
          else {
            exact = true;
            found = 0;
            param = &(groups[g].parameters[p]);
          }
        }
        else if (!exact) {
          found++;
          param = &(groups[g].parameters[p]);
        }
      }
    }
  }
  if (param == NULL) {
    string message = "Unknown parameter [" + paramName + "].";
    throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
  }
  else if ((found > 1) && (!exact)) {
      string message = "Parameter [" + paramName + "] is not unique.";
      throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
  }
  return param;
}

/**
 * Throws an Isis::iExceptionXxxxxxxx if the parameter value(s) is invalid
 * 
 * @param param The parameter data
 * 
 * @throws iException::User 
 */
void IsisAml::Verify (const IsisParameterData *param) {

  // Check to make sure the value string can be converted to the correct type
  for (unsigned int i=0; i<param->values.size (); i++) {
    if (param->type == "integer") {
      try {
        Isis::iString value (param->values[i]);
        value.ToInteger();
      }
      catch (Isis::iException &e) {
        string message = "Unable to convert [" + param->values[i] + "] to an integer,"
                         " parameter [" + param->name + "].";
        throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
      }
    }
    else if (param->type == "double") {
      try {
        Isis::iString value (param->values[i]);
        value.ToDouble();
      }
      catch (Isis::iException &e) {
        string message = "Unable to convert [" + param->values[i] + "] to a double,"
                         " parameter [" + param->name + "].";
        throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
      }
    }
    else if (param->type == "boolean") {
      Isis::iString v = param->values[i];
      v.UpCase();

      try {
        StringToBool (v);
      }
      catch (Isis::iException &e) {
        string message = "Illegal value for [" + param->name + "], [" + param->values[i] + "].";
        throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
      }
    }
    else if (param->type == "filename") {
      // Put something here once we figure out what to do with filenames
    }
  }

  // Check the default values if there were no values
  if (param->values.size () == 0) {
    for (unsigned int i=0; i<param->defaultValues.size (); i++) {
      // Check to make sure the DefaultValue string can be converted to the
      // correct type
      if (param->type == "integer") {
        try {
          Isis::iString value (param->defaultValues[i]);
          value.ToInteger();
        }
        catch (Isis::iException &e) {
          string message = "Unable to convert default [" + param->defaultValues[i] +
                           "] to an integer, parameter [" + param->name + "].";
          throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
        }
      }
      else if (param->type == "double") {
        try {
          Isis::iString value (param->defaultValues[i]);
          value.ToDouble();
        }
        catch (Isis::iException &e) {
          string message = "Unable to convert default [" + param->defaultValues[i] +
                           "] to a double, parameter [" + param->name + "].";
          throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
        }
      }
      else if (param->type == "boolean") {
        Isis::iString v = param->defaultValues[i];
        v.UpCase();

        try {
          StringToBool (v);
        }
        catch (Isis::iException &e) {
          string message = "Illegal default value for [" + param->name + "], [" 
            + param->defaultValues[i] + "].";
          throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
        }
      }
      else if (param->type == "filename") {
        // Put something here once we figure out what to do with filenames
      }
    }
  }

  // Check the values against the values list if there is one
  if (param->listOptions.size () > 0) {
    for (unsigned int i=0; i<param->values.size (); i++) {
      Isis::iString value = param->values[i];
      value = value.UpCase();
      int partial = 0;
      bool exact = false;
      for (unsigned int p=0; p<param->listOptions.size (); p++) {
        Isis::iString option = param->listOptions[p].value;
        option = option.UpCase();
        // Check to see if the value matches the list option exactly
        if (value == option) {
          // If we already have one exact match then there is an error
          if (exact) {
            string message = "Duplicate list options [" +
                             param->listOptions[p].value +
                             "] in parameter [" + param->name + "].";
            throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
          }
          exact = true;
        }
        // Check for a partial match
        // Compare the shorter of the two (values[i]) to a substring of the
        // longer string (listOptions[p].value)
        else if (option.compare(0, min(value.size(),option.size()),
                                value, 0, min(value.size(),option.size())) == 0) {
          partial++;
        }
      }
      if (!exact && partial == 0) {
        string message = "Value of [" + param->name + "] must be one of [" +
                         param->listOptions[0].value;
        for (unsigned int p=1; p<param->listOptions.size (); p++) {
          message += ", " + param->listOptions[p].value;
        }
        message += "].";
        throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
      }
      else if (!exact && partial > 1) {
        string msg = "Value of [" + param->name +
                     "] does not match a list option uniquely.";
        throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
      }
    }
  }

  // Check the values against the minimum
  if (param->minimum.length () > 0) {
    Isis::iString incl = param->minimum_inclusive;
    for (unsigned int i=0; i<param->values.size (); i++) {
      if (param->type == "integer") {
        Isis::iString value (param->values[i]);
        int temp = value.ToInteger();
        value = param->minimum;
        int min = value.ToInteger();
        if (StringToBool(incl) && (temp < min)) {
          string message = "Parameter [" + param->name + 
            "] must be greater than or equal to [" + param->minimum + "].";
          throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
        }
        else if (!StringToBool(incl) && (temp <= min)) {
          string message = "Parameter [" + param->name + 
            "] must be greater than [" + param->minimum + "].";
          throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
        }
      }
      else if (param->type == "double") {
        Isis::iString value (param->values[i]);
        double temp = value.ToDouble();
        value = param->minimum;
        double min = value.ToDouble();
        if (StringToBool(incl) && (temp < min)) {
          string message = "Parameter [" + param->name + 
            "] must be greater than or equal to [" + param->minimum + "].";
          throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
        }
        else if (!StringToBool(incl) && (temp <= min)) {
          string message = "Parameter [" + param->name + 
            "] must be greater than [" + param->minimum + "].";
          throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
        }
      }
    }
    if (param->values.size () == 0) {
      for (unsigned int i=0; i<param->defaultValues.size (); i++) {
        if (param->type == "integer") {
          Isis::iString value (param->defaultValues[i]);
          int temp = value.ToInteger();
          value = param->minimum;
          int min = value.ToInteger();
          if (StringToBool(incl) && (temp < min)) {
            string message = "Parameter [" + param->name + 
              "] must be greater than or equal to [" + param->minimum + "].";
            throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
          }
          else if (!StringToBool(incl) && (temp <= min)) {
            string message = "Parameter [" + param->name + 
              "] must be greater than [" + param->minimum + "].";
            throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
          }
        }
        else if (param->type == "double") {
          Isis::iString value (param->defaultValues[i]);
          double temp = value.ToDouble();
          value = param->minimum;
          double min = value.ToDouble();
          if (StringToBool(incl) && (temp < min)) {
            string message = "Parameter [" + param->name + 
              "] must be greater than or equal to [" + param->minimum + "].";
            throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
          }
          else if (!StringToBool(incl) && (temp <= min)) {
            string message = "Parameter [" + param->name + 
              "] must be greater than [" + param->minimum + "].";
            throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
          }
        }
      }
    }
  }

  // Check the values against the maximum
  if (param->maximum.length () > 0) {
    Isis::iString incl = param->maximum_inclusive;
    incl.DownCase();
    for (unsigned int i=0; i<param->values.size (); i++) {
      if (param->type == "integer") {
        Isis::iString value (param->values[i]);
        int temp = value.ToInteger();
        value = param->maximum;
        int max = value.ToInteger();
        if (StringToBool(incl) && (temp > max)) {
          string message = "Parameter [" + param->name + 
            "] must be less than or equal to [" + param->maximum + "].";
          throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
        }
        else if (!StringToBool(incl) && (temp >= max)) {
          string message = "Parameter [" + param->name + 
            "] must be less than [" + param->maximum + "].";
          throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
        }
      }
      else if (param->type == "double") {
        Isis::iString value (param->values[i]);
        double temp = value.ToDouble();
        value = param->maximum;
        double max = value.ToDouble();
        if (StringToBool(incl) && (temp > max)) {
          string message = "Parameter [" + param->name + 
            "] must be less than or equal to [" + param->maximum + "].";
          throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
        }
        else if (!StringToBool(incl) && (temp >= max)) {
          string message = "Parameter [" + param->name + 
            "] must be less than [" + param->maximum + "].";
          throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
        }
      }
    }
    if (param->values.size () == 0) {
      for (unsigned int i=0; i<param->defaultValues.size (); i++) {
        if (param->type == "integer") {
          Isis::iString value (param->defaultValues[i]);
          int temp = value.ToInteger();
          value = param->maximum;
          int max = value.ToInteger();
          if (StringToBool(incl) && (temp > max)) {
            string message = "Parameter [" + param->name + 
              "] must be less than or equal to [" + param->maximum + "].";
            throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
          }
          else if (!StringToBool(incl) && (temp >= max)) {
            string message = "Parameter [" + param->name + 
              "] must be less than [" + param->maximum + "].";
            throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
          }
        }
        else if (param->type == "double") {
          Isis::iString value (param->defaultValues[i]);
          double temp = value.ToDouble();
          value = param->maximum;
          double max = value.ToDouble();
          if (StringToBool(incl) && (temp > max)) {
            string message = "Parameter [" + param->name + 
              "] must be less than or equal to [" + param->maximum + "].";
            throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
          }
          else if (!StringToBool(incl) && (temp >= max)) {
            string message = "Parameter [" + param->name + 
              "] must be less than [" + param->maximum + "].";
            throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
          }
        }
      }
    }
  }

  // Check the value for an odd test
  Isis::iString odd = param->odd;
  odd.DownCase();

  if ((odd != "") || StringToBool(odd)) {
    if (param->type != "integer") {
      string message = "Parameter [" + param->name + 
        "] must be of type integer to have an [odd] test.";
      throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
    }
    else {
      for (unsigned int i=0; i<param->values.size (); i++) {
        Isis::iString value (param->values[i]);
        if ((value.ToInteger() % 2) != 1) {
          string message = "Value for [" + param->name + "] must be odd.";
          throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
        }
      }
    }
  }
  return;
}


//! Verify all parameters
void IsisAml::VerifyAll () {
  for (unsigned int g=0; g<groups.size(); g++) {
    for (unsigned int p=0; p<groups[g].parameters.size(); p++) {
      IsisParameterData *param = &(this->groups[g].parameters[p]);

      Verify (param);

      // Check the values for inclusive clauses
      for (unsigned int item=0; item<param->include.size(); item++) {
        // If this parameter is a boolean and it is true/yes
        // all included parameters must have some type of value
        if (param->type == "boolean") {
          if (((param->values.size() > 0) && StringToBool(param->values[0])) ||
              ((param->values.size() == 0) && (param->defaultValues.size() > 0) 
               && StringToBool(param->defaultValues[0]))) {

            const IsisParameterData *param2 = ReturnParam (param->include[item]);
            if ((param2->values.size()) == 0 &&
                (param2->defaultValues.size() == 0) &&
                (param2->internalDefault.size() == 0)) {
              string message = "Parameter [" + param2->name +
                               "] must be used if parameter [" +
                               param->name + "] equates to true.";
              throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
            }
          }
        }
        // This parameter is NOT a boolean but the other one might be
        else {
          // If the other parameter is a boolean and it is true/yes
          // then this parameter must have some type of value
          const IsisParameterData *param2 = ReturnParam (param->include[item]);
          if (param2->type == "boolean") {
            if (((param2->values.size() > 0) && StringToBool(param2->values[0])) ||
                ((param2->values.size() == 0) && (param2->defaultValues.size() > 0) &&
                StringToBool(param2->defaultValues[0]))) {
              if ((param->values.size()) == 0 &&
                  (param->defaultValues.size() == 0) &&
                  (param->internalDefault.size() == 0)) {
                string message = "Parameter [" + param2->name +
                                 "] must be used if parameter [" +
                                 param->name + "] is used.";
                throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
              }
            }
          }
          // Neithter parameter is a boolean so
          // If this one has a value the other parameter must have some type of value
          else {
            if (param->values.size() > 0 &&
                param2->values.size() == 0 &&
                param2->defaultValues.size() == 0 &&
                param2->internalDefault.size() == 0) {
              string message = "Parameter [" + param2->name +
                               "] must be used if parameter [" +
                               param->name + "] is used.";
              throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
            }
          }
        }
      }
      // Check the values for exclusive clauses
      for (unsigned int item=0; item<param->exclude.size(); item++) {
        // If this parameter is a boolean that is true/yes
        // the other parameter should not have a value
        if (param->type == "boolean") {
          if (((param->values.size() > 0) && StringToBool(param->values[0])) ||
              ((param->values.size() == 0) && (param->defaultValues.size() > 0) &&
              StringToBool(param->defaultValues[0]))) {

            const IsisParameterData *param2 = ReturnParam (param->exclude[item]);
            if (param2->values.size() > 0) {
              string message = "Parameter [" + param2->name +
                               "] must NOT be used if parameter [" +
                               param->name + "] equates to true.";
              throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
            }
          }
        }
        // This parameter is NOT a boolean but the other one might be
        else {
          const IsisParameterData *param2 = ReturnParam (param->exclude[item]);
          if (param2->type == "boolean") {
            if (((param2->values.size() > 0) && StringToBool(param2->values[0])) ||
                ((param2->values.size() == 0) && (param2->defaultValues.size() > 0) &&
                StringToBool(param2->defaultValues[0]))) {
              if (param->values.size() > 0) {
                string message = "Parameter [" + param2->name +
                                 "] must be used if parameter [" +
                                 param->name + "] is used.";
                throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
              }
            }
          }
          // Neither parameter is a boolean
          else {
            if (param->values.size() > 0 && param2->values.size() > 0) {
              string message = "Parameter [" + param2->name +
                               "] must NOT be used if parameter [" +
                               param->name + "] is used.";
              throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
            }
          }
        }
      }

      // Check the values for greaterThan clauses
      if (param->values.size() > 0) {
        for (unsigned int item=0; item<param->greaterThan.size(); item++) {
          const IsisParameterData *param2 = ReturnParam (param->greaterThan[item]);
          if (param2->values.size() != 0) {
            double double1, double2;
            if (param->type == "integer") {
              double1 = (double) GetInteger (param->name);
            }
            else if (param->type == "double") {
              double1 = GetDouble (param->name);
            }
            else {
              string msg = "Parameter is not INTEGER or DOUBLE type [" +
                           param->name + "]";
              throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
            }

            if (param2->type == "integer") {
              double2 = GetInteger (param2->name);
            }
            else if (param2->type == "double") {
              double2 = GetDouble (param2->name);
            }
            else {
              string msg = "Parameter is not INTEGER or DOUBLE type [" +
                           param->name + "]";
              throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
            }

            if (double2 >= double1) {
              string message = "Parameter [" + param->name +
                               "] must be greater than parameter [" +
                               param2->name + "].";
              throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
            }
          }
        }
      }

      // Check the values for greaterThanOrEqual clauses
      if (param->values.size() > 0) {
        for (unsigned int item=0; item<param->greaterThanOrEqual.size(); item++) {
          const IsisParameterData *param2 =
                                  ReturnParam(param->greaterThanOrEqual[item]);
          if (param2->values.size() != 0) {
            double double1, double2;
            if (param->type == "integer") {
              double1 = (double) GetInteger (param->name);
            }
            else if (param->type == "double") {
              double1 = GetDouble (param->name);
            }
            else {
              string msg = "Parameter is not INTEGER or DOUBLE type [" +
                           param->name + "]";
              throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
            }

            if (param2->type == "integer") {
              double2 = GetInteger (param2->name);
            }
            else if (param2->type == "double") {
              double2 = GetDouble (param2->name);
            }
            else {
              string msg = "Parameter is not INTEGER or DOUBLE type [" +
                           param->name + "]";
              throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
            }

            if (double2 > double1) {
              string message = "Parameter [" + param->name +
                               "] must be greater than or equal to parameter [" +
                               param2->name + "].";
              throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
            }
          }
        }
      }
      // Check the values for lessThan clauses
      if (param->values.size() > 0) {
        for (unsigned int item=0; item<param->lessThan.size(); item++) {
          const IsisParameterData *param2 = ReturnParam (param->lessThan[item]);
          if (param2->values.size() != 0) {
            double double1, double2;
            if (param->type == "integer") {
              double1 = (double) GetInteger (param->name);
            }
            else if (param->type == "double") {
              double1 = GetDouble (param->name);
            }
            else {
              string msg = "Parameter is not INTEGER or DOUBLE type [" +
                           param->name + "]";
              throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
            }

            if (param2->type == "integer") {
              double2 = GetInteger (param2->name);
            }
            else if (param2->type == "double") {
              double2 = GetDouble (param2->name);
            }
            else {
              string msg = "Parameter is not INTEGER or DOUBLE type [" +
                           param->name + "]";
              throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
            }

            if (double2 <= double1) {
              string message = "Parameter [" + param->name +
                               "] must be less than parameter [" +
                               param2->name + "].";
              throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
            }
          }
        }
      }

      // Check the values for lessThanOrEqual clauses
      if (param->values.size() > 0) {
        for (unsigned int item=0; item<param->lessThanOrEqual.size(); item++) {
          const IsisParameterData *param2 =
                                  ReturnParam(param->lessThanOrEqual[item]);
          if (param2->values.size() != 0) {
            double double1, double2;
            if (param->type == "integer") {
              double1 = (double) GetInteger (param->name);
            }
            else if (param->type == "double") {
              double1 = GetDouble (param->name);
            }
            else {
              string msg = "Parameter is not INTEGER or DOUBLE type [" +
                           param->name + "]";
              throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
            }

            if (param2->type == "integer") {
              double2 = GetInteger (param2->name);
            }
            else if (param2->type == "double") {
              double2 = GetDouble (param2->name);
            }
            else {
              string msg = "Parameter is not INTEGER or DOUBLE type [" +
                           param->name + "]";
              throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
            }

            if (double2 < double1) {
              string message = "Parameter [" + param->name +
                               "] must be less than or equal to parameter [" +
                               param2->name + "].";
              throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
            }
          }
        }
      }

      // Check the values for notEqual clauses
      if (param->values.size() > 0) {
        for (unsigned int item=0; item<param->notEqual.size(); item++) {
          const IsisParameterData *param2 = ReturnParam (param->notEqual[item]);
          if (param2->values.size() != 0) {
            double double1, double2;
            if (param->type == "integer") {
              double1 = (double) GetInteger (param->name);
            }
            else if (param->type == "double") {
              double1 = GetDouble (param->name);
            }
            else {
              string msg = "Parameter is not INTEGER or DOUBLE type [" +
                           param->name + "]";
              throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
            }

            if (param2->type == "integer") {
              double2 = GetInteger (param2->name);
            }
            else if (param2->type == "double") {
              double2 = GetDouble (param2->name);
            }
            else {
              string msg = "Parameter is not INTEGER or DOUBLE type [" +
                           param->name + "]";
              throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
            }

            if (double2 == double1) {
              string message = "Parameter [" + param->name +
                               "] must NOT be equal to parameter [" +
                               param2->name + "].";
              throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
            }
          }
        }
      }

      // If this parameter has a value, and a list/option/exclude, make sure
      // the excluded parameter has NO value
      if (((param->values.size() > 0) || (param->defaultValues.size())) > 0) {
        for (unsigned int o2=0; o2<param->listOptions.size(); o2++) {
          Isis::iString value, option;
          if (param->type == "string") {
            value = GetString (param->name);
            value = value.UpCase ();
            option = param->listOptions[o2].value;
            option = option.UpCase ();
          }
          else if (param->type == "integer") {
            value = GetAsString (param->name);
            value = value.Trim ("\n\r\t\f\v\b");
            option = param->listOptions[o2].value;
            option = option.Trim ("\n\r\t\f\v\b");
          }
          if (value == option) {
            for (unsigned int e2=0;e2<param->listOptions[o2].exclude.size();e2++) {
              const IsisParameterData *param2 =
                  ReturnParam (param->listOptions[o2].exclude[e2]);
              if (param2->values.size() > 0) {
                string message = "Parameter [" + param2->name +
                                 "] can not be entered if parameter [" +
                                 param->name + "] is equal to [" +
                                 value + "]";
                throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
              }
            }
          }
        }
      }

      // If this parameter has a value, and a list/option/include, make sure
      // the included parameter has a value
      if (((param->values.size() > 0) || (param->defaultValues.size())) > 0) {
        for (unsigned int o2=0; o2<param->listOptions.size(); o2++) {
          Isis::iString value, option;
          if (param->type == "string") {
            value = GetString (param->name);
            value = value.UpCase ();
            option = param->listOptions[o2].value;
            option = option.UpCase ();
          }
          else if (param->type == "integer") {
            value = GetAsString (param->name);
            value = value.Trim ("\n\r\t\f\v\b");
            option = param->listOptions[o2].value;
            option = option.Trim ("\n\r\t\f\v\b");
          }
          if (value == option) {
            for (unsigned int e2=0;e2<param->listOptions[o2].include.size();e2++) {
              const IsisParameterData *param2 =
                  ReturnParam (param->listOptions[o2].include[e2]);
              if ((param2->values.size() == 0) &&
                  (param2->defaultValues.size() == 0)) {
                string message = "Parameter [" + param2->name +
                                 "] must be entered if parameter [" +
                                 param->name + "] is equal to [" +
                                 value + "]";
                throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
              }
            }
          }
        }
      }

      // If this parameter has no value, default value or internal default
      // value then it must be excluded by some other parameter with an
      // exclude or list/option/exclude
      //   OR
      // it must include a boolean which is false
      if ((param->values.size() == 0) && (param->defaultValues.size() == 0) &&
          (param->internalDefault.size() == 0)) {
        bool excluded = false;
        // See if another parameter has a list option excluding this parameter
        for (unsigned int g2=0; g2<groups.size(); g2++) {
          for (unsigned int p2=0; p2<groups[g2].parameters.size(); p2++) {
            for (unsigned int o2=0;
                 o2<groups[g2].parameters[p2].listOptions.size(); o2++) {
              for (unsigned int e2=0;
                   e2<groups[g2].parameters[p2].listOptions[o2].exclude.size();
                   e2++) {
                string excl =
                    this->groups[g2].parameters[p2].listOptions[o2].exclude[e2];
                if (excl == param->name) {
                  excluded = true;
                  break;
                }
              }
            }

            if(groups[g2].parameters[p2].type == "boolean") {
              const IsisParameterData *param2 = &groups[g2].parameters[p2];
              if (((param2->values.size() > 0) && !StringToBool(param2->values[0])) ||
                  ((param2->values.size() == 0) && (param2->defaultValues.size() > 0) &&
                  !StringToBool(param2->defaultValues[0]))) {
                for(unsigned int e2=0; e2<groups[g2].parameters[p2].include.size();
                     e2++) {
                  string incl =
                      this->groups[g2].parameters[p2].include[e2];
                  if (incl == param->name) {
                    excluded = true;
                  }
                }
              }
              else if (((param2->values.size() > 0) && StringToBool(param2->values[0])) ||
                  ((param2->values.size() == 0) && (param2->defaultValues.size() > 0) &&
                  StringToBool(param2->defaultValues[0]))) {
                for(unsigned int e2=0; e2<groups[g2].parameters[p2].exclude.size();
                     e2++) {
                  string excl =
                      this->groups[g2].parameters[p2].exclude[e2];
                  if (excl == param->name) {
                    excluded = true;
                  }
                }
              }
            }
          }
        }

        // See if this parameter excludes any other (this implies the other
        // one also excludes this one too
        for (unsigned int item=0; item<param->exclude.size(); item++) {
          const IsisParameterData *param2 = ReturnParam(param->exclude[item]);
          if ((param2->values.size() != 0) ||
              (param2->defaultValues.size() != 0) ||
              (param2->internalDefault.size() != 0)) {
            if (param2->type != "boolean") {
              excluded = true;
            }
            else {
              if (((param2->values.size() > 0) && !StringToBool(param2->values[0])) ||
                  ((param2->values.size() == 0) && (param2->defaultValues.size() > 0) &&
                  !StringToBool(param2->defaultValues[0]))) {
                excluded = true;
              }
            }
          }
        }

        // See if this parameter includes a boolean that is false
        // then it doesn't need a value
        for (unsigned int item=0; item<param->include.size(); item++) {
          const IsisParameterData *param2 = ReturnParam(param->include[item]);
          if (param2->type == "boolean") {
            if (((param2->values.size() > 0) && !StringToBool(param2->values[0])) ||
                ((param2->values.size() == 0) && (param2->defaultValues.size() > 0) &&
                !StringToBool(param2->defaultValues[0]))) {
              excluded = true;
            }
          }
        }

        if (!excluded) {
          string message = "Parameter [" + param->name + "] must be entered.";
          throw Isis::iException::Message(Isis::iException::User,message, _FILEINFO_);
        }
      }
    }
  }
}

/**
 * Returns a boolean value based on the string contents
 * 
 * @param value The value to convert to a boolean
 * 
 * @return boolean value based on string contents
 * 
 * @throws iException::Programmer (Invalid boolean value)
 */
bool IsisAml::StringToBool (Isis::iString value) const {

  value.UpCase ();
  if (value == "") {
    return false;
  }
  else if (!value.compare ("NO")) {
    return false;
  }
  else if (!value.compare ("FALSE")) {
    return false;
  }
  else if (!value.compare ("F")) {
    return false;
  }
  else if (!value.compare ("N")) {
    return false;
  }
  else if (!value.compare ("YES")) {
    return true;
  }
  else if (!value.compare ("TRUE")) {
    return true;
  }
  else if (!value.compare ("Y")) {
    return true;
  }
  else if (!value.compare ("T")) {
    return true;
  }
  else {
    string message = "Invalid boolean value [" + value + "].";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }
  return false;
}


/**
 * Creates a string which could be used as a command line
 * 
 * @param cont Pvl to put command line information in
 */
void IsisAml::CommandLine (Isis::Pvl &cont) const {
  Isis::PvlGroup group("UserParameters");

  // Add appropriate keywords
  for (unsigned int g=0; g<groups.size(); g++) {
    for (unsigned int p=0; p<groups[g].parameters.size(); p++) {
      const IsisParameterData *param = ReturnParam (ParamName(g, p));
      // If this param has a value add it to the command line
      if (param->values.size() > 0) {
        group += Isis::PvlKeyword(param->name, param->values[0]);
      }

      // Or if it has a default value add it to the command line
      else if (param->defaultValues.size() > 0) {
        group += Isis::PvlKeyword(param->name, param->defaultValues[0]);
      }
    }
  }
  // Remove excluded keywords
  for (unsigned int g=0; g<groups.size(); g++) {
    for (unsigned int p=0; p<groups[g].parameters.size(); p++) {
      const IsisParameterData *param = ReturnParam (ParamName(g, p));

      if (((param->values.size() > 0) || (param->defaultValues.size())) > 0) {
        for (unsigned int o2=0; o2<param->listOptions.size(); o2++) {
          Isis::iString value, option;
          if (param->type == "string") {
            value = GetAsString (param->name);
            value = value.UpCase ();
            option = param->listOptions[o2].value;
            option = option.UpCase ();
          }
          else if (param->type == "integer") {
            value = GetAsString (param->name);
            value = value.Trim ("\n\r\t\f\v\b");
            option = param->listOptions[o2].value;
            option = option.Trim ("\n\r\t\f\v\b");
          }
          if (value == option) {
            for (unsigned int e2=0;e2<param->listOptions[o2].exclude.size();e2++) {
              const IsisParameterData *param2 =
                  ReturnParam (param->listOptions[o2].exclude[e2]);
              if (group.HasKeyword(param2->name)) {
                group.DeleteKeyword(param2->name);
              }
            }
          }
        }
      }
    }
  }

  cont.Clear();
  cont.AddGroup(group);
  return;
}


/**
 * Returns the application version date
 * 
 * @return The application version date
 */
string IsisAml::Version () const {
  string st = "000-00-00";
  for (unsigned int i=0; i<changes.size(); i++) {
    if (changes[i].date > st ) st = changes[i].date;
  }
  return st;
}


/**
 * Starts parsing an application xml file
 * 
 * @param xmlfile The xml file to parse
 * 
 * @throws iException::Programmer (Error during XML parser initialization)
 */
void IsisAml::StartParser (const char *xmlfile) {

  // Initialize the XML system
  try {
    XERCES::XMLPlatformUtils::Initialize();
  }

  catch (const XERCES::XMLException& toCatch) {
    string message = "Error during XML parser initialization" +
                     (string)XERCES::XMLString::transcode(toCatch.getMessage());
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
    return;
  }

  //
  //  Create a SAX parser object. Then, set it to validate for an IsisAml file
  //
  parser = XERCES::XMLReaderFactory::createXMLReader();

//  SAX2XMLReader::ValSchemes valScheme = SAX2XMLReader::Val_Auto;
  XERCES::SAX2XMLReader::ValSchemes valScheme = XERCES::SAX2XMLReader::Val_Never;
  if (valScheme == XERCES::SAX2XMLReader::Val_Auto) {
    parser->setFeature(XERCES::XMLString::transcode("http://xml.org/sax/features/validation"), true);
    parser->setFeature(XERCES::XMLString::transcode("http://apache.org/xml/features/validation/dynamic"), true);
  }
  else if (valScheme == XERCES::SAX2XMLReader::Val_Never) {
    parser->setFeature(XERCES::XMLString::transcode("http://xml.org/sax/features/validation"), false);
  }

  else if (valScheme == XERCES::SAX2XMLReader::Val_Always) {
    parser->setFeature(XERCES::XMLString::transcode("http://xml.org/sax/features/validation"), true);
    parser->setFeature(XERCES::XMLString::transcode("http://apache.org/xml/features/validation/dynamic"), false);
  }

//  bool doSchema = true;
  bool doSchema = false;
  parser->setFeature(XERCES::XMLString::transcode("http://apache.org/xml/features/validation/schema"), doSchema);

  bool schemaFullChecking = false;
  parser->setFeature(XERCES::XMLString::transcode("http://apache.org/xml/features/validation/schema-full-checking"), schemaFullChecking);

  //  Create the handler object for an application
  //  Then parse the file
  char* encodingName = "LATIN1";
  bool expandNamespaces = false ;

  try {
    IsisAmlData *mydata = this;
    appHandler = new IsisXMLApplication (encodingName, expandNamespaces, parser, mydata);
    parser->parse(xmlfile);
  }
  catch (const XERCES::XMLException& toCatch) {
    string message = "Error in application XML file: " +
                     (string)XERCES::XMLString::transcode(toCatch.getMessage());
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
    XERCES::XMLPlatformUtils::Terminate();
    return;
  }

  //  Delete the parser
  delete parser;
  XERCES::XMLPlatformUtils::Terminate();
  delete appHandler;
  return;
}

