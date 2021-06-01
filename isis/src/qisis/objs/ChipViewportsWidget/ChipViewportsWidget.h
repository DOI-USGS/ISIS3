#ifndef ChipViewportsWidget_h
#define ChipViewportsWidget_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ControlPoint.h"

#include <QWidget>
#include <QPointer>

class QAction;
class QGridLayout;
class QLabel;
class QMouseEvent;

namespace Isis {
  class ChipViewport;
  class ControlMeasure;
  class ControlNet;
  class ControlPoint;
  class Cube;
  class CubeViewport;
  class MdiCubeViewport;
  class SerialNumberList;
  class Stretch;
  class UniversalGroundMap;

  /**
   * @brief Scrolled widget for display ChipViewports
   *
   * @ingroup Visualization Tools
   *
   * @author 2016-06-15 Tracie Sucharski
   *
   * @internal
   */
  class ChipViewportsWidget : public QWidget {
    Q_OBJECT

    public:
      ChipViewportsWidget(QWidget *parent);
      virtual ~ChipViewportsWidget();

      bool eventFilter(QObject *object, QEvent *event);

    signals:
      void controlPointChanged(QString pointId);
      void netChanged();
      void newControlNetwork(ControlNet *);
      void stretchChipViewport(Stretch *, CubeViewport *);
      void measureChanged();
      void saveControlNet();

    public slots:
      void setSerialNumberList(SerialNumberList *snList);
      void setControlNet(ControlNet *cnet, QString cnetFilename);
      void setPoint(ControlPoint *controlPoint);

//    ChipViewport *referenceChipViewport();

//    void savePoint();
//    void colorizeSavePointButton();

//    void refresh();

    protected:

    private slots:
      void showPoints(bool showPoints);
      void geomChips(bool geomChips);
      void clearPoint();
//    void enterWhatsThisMode();

    private:
      void createChipViewports(QWidget *parent);
      void createActions();

      void loadPoint();

      void mousePressEvent(QObject *object, QMouseEvent *event);

    private:

      QPointer<QWidget> m_parent;
//    bool m_addMeasuresButton;

      QString m_cnetFileName;
      QPointer<QLabel> m_cnetFileNameLabel;
      bool m_netChanged;

      QPointer<QAction> m_closeChipViewportWidget;

      QPointer<QAction> m_saveChips;


      QPointer<QLabel> m_ptIdValue;

      QPointer<ControlPoint> m_controlPoint;
      SerialNumberList *m_serialNumberList;
      QPointer<ControlNet> m_controlNet;

      QPointer<QGridLayout> m_chipViewportsLayout;
      QList<ChipViewport *> m_chipViewports;
//    QMap<Cube *, ControlMeasureEditWidget *> m_cubeMeasureEditMap;
//    QPointer<ControlMeasureEditWidget> m_referenceMeasureEditor;
  };
};
#endif
