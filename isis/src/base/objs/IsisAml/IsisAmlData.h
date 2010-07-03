/**                                                                       
 * @file                                                                  
 * $Revision: 1.1.1.1 $                                                             
 * $Date: 2006/10/31 23:18:07 $                                                                 
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

#ifndef IsisAmlData_h
#define IsisAmlData_h

#include <string>
#include <vector>

#include "CubeAttribute.h"

class IsisListOptionData {
  public:
    std::string value;
    std::string brief;
    std::string description;
    std::vector<std::string> exclude;
    std::vector<std::string> include;

  IsisListOptionData ();
  ~IsisListOptionData ();
};

class IsisHelperData {
  public:
    std::string name;
    std::string icon;
    std::string brief;
    std::string description;
    std::string function;

  IsisHelperData();
  ~IsisHelperData();
};

class IsisParameterData {
  public:
    std::vector <std::string> values;
    std::string name;
    std::string brief;
    std::string description;
    std::string type;
    std::vector <std::string> defaultValues;
    std::string internalDefault;
    std::string count;
    std::vector <IsisListOptionData> listOptions;
    std::string minimum_inclusive;
    std::string minimum;
    std::string maximum_inclusive;
    std::string maximum;
    std::vector<std::string> greaterThan;
    std::vector<std::string> greaterThanOrEqual;
    std::vector<std::string> lessThan;
    std::vector<std::string> lessThanOrEqual;
    std::vector<std::string> notEqual;
    std::vector<std::string> exclude;
    std::vector<std::string> include;
    std::string odd;
    std::string filter;
    std::string path;
    std::string fileMode;
    Isis::CubeAttributeOutput outCubeAtt;
    Isis::CubeAttributeInput inCubeAtt;
    std::string pixelType;
    std::vector<IsisHelperData> helpers;

    IsisParameterData ();
    ~IsisParameterData ();
};


class IsisGroupData {
  public:
    std::string name;
    std::vector<IsisParameterData> parameters;

    IsisGroupData ();
    ~IsisGroupData ();
};

class IsisChangeData {
  public:
    std::string name;
    std::string date;
    std::string description;

    IsisChangeData ();
    ~IsisChangeData ();
};


class IsisAmlData {
  public:
    std::string name;
    std::string brief;
    std::string description;
    std::vector<IsisGroupData> groups;
    std::vector<std::string> categorys;
    std::vector<IsisChangeData> changes;

    IsisAmlData ();
    ~IsisAmlData ();

};

#endif
