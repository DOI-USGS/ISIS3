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

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class IsisListOptionData {
  public:
    QString value;
    QString brief;
    QString description;
    std::vector<QString> exclude;
    std::vector<QString> include;

    IsisListOptionData();
    ~IsisListOptionData();
};

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class IsisHelperData {
  public:
    QString name;
    QString icon;
    QString brief;
    QString description;
    QString function;

    IsisHelperData();
    ~IsisHelperData();
};

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class IsisParameterData {
  public:
    std::vector <QString> values;
    QString name;
    QString brief;
    QString description;
    QString type;
    std::vector <QString> defaultValues;
    QString internalDefault;
    QString count;
    std::vector <IsisListOptionData> listOptions;
    QString minimum_inclusive;
    QString minimum;
    QString maximum_inclusive;
    QString maximum;
    std::vector<QString> greaterThan;
    std::vector<QString> greaterThanOrEqual;
    std::vector<QString> lessThan;
    std::vector<QString> lessThanOrEqual;
    std::vector<QString> notEqual;
    std::vector<QString> exclude;
    std::vector<QString> include;
    QString odd;
    QString filter;
    QString path;
    QString fileMode;
    Isis::CubeAttributeOutput outCubeAtt;
    Isis::CubeAttributeInput inCubeAtt;
    QString pixelType;
    std::vector<IsisHelperData> helpers;

    IsisParameterData();
    ~IsisParameterData();
};


/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class IsisGroupData {
  public:
    QString name;
    std::vector<IsisParameterData> parameters;

    IsisGroupData();
    ~IsisGroupData();
};

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class IsisChangeData {
  public:
    QString name;
    QString date;
    QString description;

    IsisChangeData();
    ~IsisChangeData();
};


/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class IsisAmlData {
  public:
    QString name;
    QString brief;
    QString description;
    std::vector<IsisGroupData> groups;
    std::vector<QString> categorys;
    std::vector<IsisChangeData> changes;

    IsisAmlData();
    ~IsisAmlData();

};

#endif
