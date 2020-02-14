#ifndef spiceinit_h
#define spiceinit_h

#include <vector>

#include <QString>
#include <QList>
#include <QVector>

#include "ControlNet.h"
#include "ControlMeasure.h"
#include "UserInterface.h"
#include "Progress.h"

namespace Isis {
  extern void cubeConvexHullAndMeasures(QString &serialNum,ControlNet &net, double &area, int &validMeasures,
                            QList <ControlMeasure *> *measToIgnor=NULL);

  extern void cnetwinnow(UserInterface &ui);

  extern void cnetwinnow(ControlNet &net, Progress &progress, UserInterface &ui);
}

#endif
