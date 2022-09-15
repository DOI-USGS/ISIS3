#ifndef LatLonGridTool_h
#define LatLonGridTool_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Tool.h"


#include <QMap>
#include <QStack>
#include <QPointer>


class QToolButton;
class QPainter;
class QCheckBox;

namespace Isis {
  class MdiCubeViewport;


  /**
   * @brief Lat Lon Grid View Tool
   *
   * This tool is part of the Qisis namespace and allows visualizes latitude and
   * longitude lines on cube.
   *
   * @ingroup Visualization Tools
   *
   * @author  2022-08-08  Amy Stamile
   */
  class LatLonGridTool : public Tool {
      Q_OBJECT

    public:
      LatLonGridTool(QWidget *parent);
      void paintViewport(MdiCubeViewport *mvp, QPainter *painter);

    protected:
      QAction *toolPadAction(ToolPad *pad);
      QWidget *createToolBarWidget(QStackedWidget *active);
      void updateTool();

    private:
      QWidget *m_container;
      QPointer<QCheckBox> m_gridCheckBox;
  };
};

#endif
