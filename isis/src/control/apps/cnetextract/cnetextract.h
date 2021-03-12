#ifndef cnetextract_h
#define cnetextract_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <set>
#include <sstream>

#include <QMap>
#include <QSet>
#include <QVector>

#include "Angle.h"
#include "Camera.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Cube.h"
#include "CubeManager.h"
#include "Environment.h"
#include "FileList.h"
#include "IException.h"
#include "IString.h"
#include "Longitude.h"
#include "Latitude.h"
#include "TProjection.h"
#include "Progress.h"
#include "ProjectionFactory.h"
#include "Pvl.h"
#include "SerialNumber.h"
#include "SurfacePoint.h"
#include "UserInterface.h"

namespace Isis {
  void cnetextract(UserInterface &ui, Pvl *log);

  void cnetextract(ControlNet outNet, UserInterface &ui, Pvl *log);
}

#endif
