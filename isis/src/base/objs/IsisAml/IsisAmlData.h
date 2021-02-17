#ifndef IsisAmlData_h
#define IsisAmlData_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
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
