#ifndef hyb2onc2isis_h
#define hyb2onc2isis_h

#include <QDebug>
#include <QString>

#include "Pvl.h"
#include "UserInterface.h"

namespace Isis {

extern void hyb2onc2isis(UserInterface &ui);
extern Pvl * hyb2onc2isis(QString &fitsFileName,QString &outputCubeFileName,QString target="");

}

#endif
