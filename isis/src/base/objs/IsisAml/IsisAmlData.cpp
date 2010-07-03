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

using namespace std;

#include "iException.h"
#include "IsisAmlData.h"

// Constructors
  IsisAmlData::IsisAmlData () {
    name.erase();
    brief.erase();
    description.erase();
    groups.clear();
    categorys.clear();
  }

  IsisAmlData::~IsisAmlData () {
  }


// Helper classes
  IsisHelperData::IsisHelperData () {
    name.erase();
    icon.erase();
    brief.erase();
    description.erase();
    function.erase();
  }

  IsisHelperData::~IsisHelperData () {
  }


  IsisGroupData::IsisGroupData () {
    name.erase();
  }

  IsisGroupData::~IsisGroupData () {
  }


  IsisParameterData::IsisParameterData () {
    values.clear();
    name.erase();
    brief.erase();
    description.erase();
    type.erase();
    internalDefault.erase();
    count.erase();
    defaultValues.clear();
    exclude.clear();
    include.clear();
    filter.erase();
    greaterThan.clear();
    greaterThanOrEqual.clear();
    lessThan.clear();
    lessThanOrEqual.clear();
    internalDefault.erase();
    notEqual.clear();
    maximum.erase();
    maximum_inclusive.erase();
    minimum.erase();
    minimum_inclusive.erase();
    odd.erase();
    helpers.clear();
  }

  IsisParameterData::~IsisParameterData () {
  }


  IsisListOptionData::IsisListOptionData () {
    value.erase();
    brief.erase();
    description.erase();
    exclude.clear();
  }

  IsisListOptionData::~IsisListOptionData () {
  }


  IsisChangeData::IsisChangeData () {
    name.erase();
    date.erase();
    description.erase();
  }

  IsisChangeData::~IsisChangeData () {
  }

