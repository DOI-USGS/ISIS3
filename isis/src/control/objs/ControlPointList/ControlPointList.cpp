#include "ControlPointList.h"

#include <QList>

#include "IException.h"
#include "FileList.h"
#include "FileName.h"
#include "iString.h"

namespace Isis {
  /**
   * Creates a ControlPointList from a list of control point ids'
   *
   * @param psListFile The file withe list of control point ids'
   */
  ControlPointList::ControlPointList(const FileName &psListFile) {
    try {
      QList<QString> qList;
      FileList list(psListFile);
      int size = list.size();
      for(int i = 0; i < size; i++) {
        qList.insert(i, list[i].toString().ToQt());
        mbFound.push_back(false);
      }
      mqCpList = QStringList(qList);

      //sort the list for faster searches - internally uses qsort()
      mqCpList.sort();
    }
    catch(IException &e) {
      std::string msg = "Can't open or invalid file list [" +
          psListFile.expanded() + "]";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }
  }

  /**
   * Destructor
   */
  ControlPointList::~ControlPointList() {
  }


  /**
   * Determines whether or not the requested control point id
   * exists in the list
   *
   * @param psCpId The control point id to be checked for
   *
   * @return bool
   */
  bool ControlPointList::HasControlPoint(const std::string &psCpId) {
    int index = mqCpList.indexOf(QString(psCpId.c_str()));

    if(index == -1 || index >= Size())
      return false;

    mbFound[index] = true;
    return true;
  }


  /**
   * How many control points in the list
   *
   * @return int Returns number of control point in the list
   */
  int ControlPointList::Size() const {
    return mqCpList.size();
  }


  /**
   * Return a control point id given an index
   *
   * @param piIndex The index of the desired control point id
   *
   * @return std::string The control point id returned
   */
  std::string ControlPointList::ControlPointId(int piIndex) {
    int size = Size();
    if(piIndex >= 0 && piIndex < size) {
      return (mqCpList.value(piIndex).toStdString());
    }
    else {
      iString num = iString(piIndex);
      std::string msg = "Index [" + (std::string) num + "] is invalid";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }

  /**
   * return a list index given a control point id
   *
   * @param ps_cpId The control point id to be searched for
   *
   * @return int The index of the control point id
   */
  int ControlPointList::ControlPointIndex(const std::string &psCpId) {
    if(HasControlPoint(psCpId)) {
      return mqCpList.indexOf(QString(psCpId.c_str()));
    }
    else {
      std::string msg = "Requested control point id [" + psCpId + "] ";
      msg += "does not exist in the list";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }

  /**
   * Register invalid control point and calculate the valid &
   * invalid point count
   *
   * @param Pvl  Will contain stats in pvl keywords
   */
  void ControlPointList::RegisterStatistics(Pvl &pcPvlLog) {
    int size = Size();
    int iNotFound = 0;
    std::string sPointsNotFound = "";

    for(int i = 0; i < size; i++) {
      if(!mbFound[i]) {
        if(iNotFound) {
          sPointsNotFound += ", ";
        }
        sPointsNotFound += mqCpList.value(i).toStdString();
        iNotFound++;
      }
    }

    pcPvlLog += Isis::PvlKeyword("TotalPoints", size);
    pcPvlLog += Isis::PvlKeyword("ValidPoints", size - iNotFound);
    pcPvlLog += Isis::PvlKeyword("InvalidPoints", iNotFound);
    pcPvlLog += Isis::PvlKeyword("InvalidPointIds", sPointsNotFound);
  }
}

