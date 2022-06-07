#ifndef IMPORT_UTILS_H
#define IMPORT_UTILS_H

#include <map>
#include <iostream>
#include <sstream>
#include <vector>

#include <QString>

#include "Buffer.h"
#include "EndianSwapper.h"
#include "Table.h"
#include "SpecialPixel.h"
#include "Stretch.h"

#include "CassiniImportUtils.h"

using namespace std;

std::map<std::string, int> processMap = {
  {"cassiniIssFixDnPostProcess", 1}
};

std::map<std::string, int> ancillaryProcessMap = {
  {"cassiniIssCreateLinePrefixTable", 1},
  {"cassiniIssFixLabel", 2}
};

namespace Isis {

  QString vectorToString(std::vector<double> v) {
    std::stringstream ss;
    ss << "(";

    size_t i = 0;
    for(; i < v.size(); i++)
    {
      if(i != 0)
        ss << ", ";
      ss << v[i];
    }
    ss << ")";
    return QString::fromStdString(ss.str());
  };

  void applyAncillaryProcess(Cube *cube,
                             QString processFunction,
                             PvlObject translation,
                             ProcessImport *process) {
    vector<vector<char *>> prefixData = process->DataPrefix();
    switch (ancillaryProcessMap[processFunction.toStdString()]) {
      case 1:
        return cassiniIssCreateLinePrefixTable(cube,
                                               prefixData.at(0),
                                               translation);
      case 2:
        return cassiniIssFixLabel(cube, translation, process);
    }

    throw IException(IException::Programmer, "Unable to find prefix/suffix function [" + processFunction.toStdString() + "]", _FILEINFO_);
  };

  void runProcess(QString ioFile, PvlObject process) {
    std::string functionString = QString(process["ProcessFunction"]).toStdString();
    switch (processMap[functionString]) {
      case 1:
        cassiniIssFixDnPostProcess(ioFile, process);
        return;
    }

    throw IException(IException::Programmer, "Unable to find functor [" + functionString + "]", _FILEINFO_);
  };
}

#endif
