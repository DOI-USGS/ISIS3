#ifndef Qnet_h
#define Qnet_h

#include "SerialNumberList.h"
#include "ControlNet.h"
#include "ViewportMainWindow.h"

#ifdef IN_QNET
#define EXTERN
#else
#define EXTERN extern
#endif

namespace Qisis {
  namespace Qnet {
    EXTERN Isis::SerialNumberList *g_serialNumberList;
    EXTERN Isis::ControlNet *g_controlNetwork;
    EXTERN Qisis::ViewportMainWindow *g_vpMainWindow;
    EXTERN QList<int> g_filteredImages;
    EXTERN QList<int> g_filteredOverlaps;
    EXTERN QList<int> g_filteredPoints;
    EXTERN QList<int> g_loadedImages;
  }
}

#endif

