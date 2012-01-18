#include "IsisDebug.h"

#include "CubePlotCurve.h"

#include <iostream>

#include <QAction>
#include <QApplication>
#include <QBuffer>
#include <QByteArray>
#include <QClipboard>
#include <QDataStream>
#include <QEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QString>

#include <qwt_legend.h>

#include "Cube.h"
#include "CubeViewport.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"
#include "CubePlotCurveConfigureDialog.h"

namespace Isis {
  /**
   * This constructs a CubePlotCurve... a subclass of PlotCurve.
   * This class was created specifically for use withthe plot
   * tool. With this class the programmer can set the cube view
   * port that the curve is associated with along with the
   * vertices on the cvp of which the curve gets it data.
   */
  CubePlotCurve::CubePlotCurve(Units xUnits, Units yUnits) :
      PlotCurve(xUnits, yUnits) {
    m_renameAutomatically = true;
    updateLegendItemWidget();
  }


  /**
   * Construct the plot tool curve given the past results of toByteArray(...).
   * This is used for copy/paste and drag/drop.
   */
  CubePlotCurve::CubePlotCurve(const QByteArray &parentAndChildData) :
      PlotCurve(Unknown, Unknown) {
    m_legendItem = NULL;
    m_legendItem = PlotCurve::legendItem();
    m_legendItem->installEventFilter(this);

    QByteArray classData = fromByteArray(parentAndChildData);

    QString expectedHeader("PLOT_TOOL_CURVE_V1");
    int headerKeySize = expectedHeader.toUtf8().size();

    if (classData.size() > headerKeySize) {
      int dataPos = 0;
      const char *rawClassData = classData.data();

      QString givenKey = QString::fromUtf8(classData.data() + dataPos,
                                           headerKeySize);
      dataPos += headerKeySize;
      if (givenKey != expectedHeader) {
        iString msg = "The given byte array does not contain the required "
            "header";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }

      int sourceCubeSize = *(((int *)(rawClassData + dataPos)));
      dataPos += sizeof(int);

      m_sourceCube = QString::fromUtf8(classData.data() + dataPos,
                                       sourceCubeSize);
      dataPos += sourceCubeSize;

      int pointListSize = *(((int *)(rawClassData + dataPos)));
      dataPos += sizeof(int);

      for (int i = 0; i < pointListSize; i ++) {
        double x = *((double *)(rawClassData + dataPos));
        dataPos += sizeof(double);

        double y = *((double *)(rawClassData + dataPos));
        dataPos += sizeof(double);

        m_pointList.append(QPointF(x, y));
      }

      ASSERT(dataPos == classData.size());
    }
    else {
      iString msg = "The given byte array is not large enough to contain the "
          "required header";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * This will start the drag & drop operation for these curves.
   *
   */
  bool CubePlotCurve::eventFilter(QObject *o, QEvent *e) {
    bool blockWidgetFromEvent = false;

    switch (e->type()) {
      case QEvent::MouseButtonPress: {
        mousePressEvent((QMouseEvent *)e);
        blockWidgetFromEvent = true;
        break;
      }

      case QEvent::MouseButtonDblClick:
      case QEvent::MouseButtonRelease:
        blockWidgetFromEvent = true;
        break;


      default:
        break;
    }

    return blockWidgetFromEvent;
  }


  void CubePlotCurve::paint(CubeViewport *vp, QPainter *painter) {
    if (m_sourceCube == vp->cube()->getFilename()) {
      QPen customPen;
      customPen.setColor(pen().color());
      customPen.setWidth(pen().width());

      if (pen().style() != Qt::NoPen)
        customPen.setStyle(pen().style());

      painter->setPen(customPen);
      QList <QPointF> points = sourceVertices();

      for (int p = 1; p <= points.size(); p++) {
        int sample1;
        int sample2;
        int line1;
        int line2;

        if (p != points.size()) {
          vp->cubeToViewport(points[p - 1].x(), points[p - 1].y(), sample1, line1);
          vp->cubeToViewport(points[p].x(), points[p].y(), sample2, line2);
        }
        else {
          vp->cubeToViewport(points[p - 1].x(), points[p - 1].y(),
                             sample1, line1);
          vp->cubeToViewport(points[0].x(), points[0].y(), sample2, line2);
        }

        painter->drawLine(QPoint(sample1, line1), QPoint(sample2,  line2));
      }
    }
  }


  /**
   * This method returns a list of points which are the vertices
   * of the selected area (by the rubberband) on the cvp.
   *
   * @return QList<QPoint>
   */
  QList <QPointF > CubePlotCurve::sourceVertices()  const {
    return m_pointList;
  }


  /**
   * TODO: does this work?
   */
  QWidget *CubePlotCurve::legendItem() const {
    return m_legendItem;
  }


  /**
   * This method returns the cube view port associated with the
   * curve.
   *
   * @return CubeViewport*
   */
  QString CubePlotCurve::sourceCube() const {
    return m_sourceCube;
  }


  void CubePlotCurve::enableAutoRenaming(bool allowed) {
    m_renameAutomatically = allowed;
  }


  void CubePlotCurve::copySource(const CubePlotCurve &other) {
    m_sourceCube = other.m_sourceCube;
    m_pointList = other.m_pointList;
    emit needsRepaint();
  }


  void CubePlotCurve::setSource(CubeViewport *cvp, QList<QPoint> screenPoints,
                                int band) {
    if (m_originalName != "" && m_sourceCube != "" && m_renameAutomatically) {
      setTitle(m_originalName);
    }
    else if (m_originalName == "") {
      m_originalName = title().text();
    }

    m_sourceCube = "";
    m_pointList.clear();

    if (cvp) {
      m_sourceCube = cvp->cube()->getFilename().ToQt();

      if (m_renameAutomatically) {
        setTitle(title().text() + " - " + QFileInfo(m_sourceCube).baseName());

        if (band != -1) {
          setTitle(title().text() + "+" + iString(band).ToQt());
        }
      }

      foreach (QPoint screenpoint, screenPoints) {
        double sample = 0.0;
        double line = 0.0;

        cvp->viewportToCube(screenpoint.x(), screenpoint.y(), sample, line);

        m_pointList.append(QPointF(sample, line));
      }
    }

    emit needsRepaint();
  }


  void CubePlotCurve::updateLegendItemWidget() {
    if (!m_legendItem) {
      m_legendItem = PlotCurve::legendItem();
      m_legendItem->installEventFilter(this);
      connect(m_legendItem, SIGNAL(destroyed(QObject *)),
              this, SLOT(updateLegendItemWidget()));
    }
  }


  /**
   * Ownership of the return value is passed to the caller.
   */
  QMimeData *CubePlotCurve::createMimeData() const {
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/isis3-plot-curve",
                      toByteArray());
    return mimeData;
  }


  QByteArray CubePlotCurve::toByteArray() const {
    QByteArray classData;

    classData.append(PlotCurve::toByteArray());

    QString header("PLOT_TOOL_CURVE_V1");
    int size = header.size();
    classData.append(header);

    size = m_sourceCube.toUtf8().size();
    classData.append((char *)&size, sizeof(int));
    classData.append(m_sourceCube.toUtf8());

    size = m_pointList.size();
    classData.append((char *)&size, sizeof(int));

    for (int i = 0; i < size; i ++) {
      QPointF data = m_pointList[i];
      double x = data.x();
      double y = data.y();

      classData.append((char *)&x, sizeof(double));
      classData.append((char *)&y, sizeof(double));
    }

    return classData;
  }


  /**
   * This will only be called for the legend item's events. So it is the right
   *   place to start a drag & drop.
   */
  void CubePlotCurve::mousePressEvent(QMouseEvent *event) {
    bool deleteThisCurve = false;
    if (event->button() == Qt::LeftButton) {
      QDrag *drag = new QDrag(plot()->legend()->contentsWidget());

      // The icon for drag & drop sometimes gets in the way of the image,
      //   so let's move the image a little more to the right of the cursor
      //   than usual.
      QPoint newHotSpot = drag->hotSpot();
      newHotSpot.setX(newHotSpot.x() * 2);
      drag->setHotSpot(newHotSpot);

      drag->setMimeData(createMimeData());
      drag->setPixmap(QPixmap::grabWidget(m_legendItem));

      Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction,
          Qt::CopyAction);

      if (dropAction == Qt::MoveAction) {
        deleteThisCurve = true;
      }
    }
    else if (event->button() == Qt::RightButton) {
      QMenu contextMenu;

      QAction *cutAct = new QAction(QIcon::fromTheme("edit-cut"), "Cut", this);
      contextMenu.addAction(cutAct);
      QAction *copyAct = new QAction(QIcon::fromTheme("edit-copy"), "Copy",
                                     this);
      // copyAct
      contextMenu.addAction(copyAct);

      contextMenu.addSeparator();

      QAction *deleteAct = new QAction(QIcon::fromTheme("edit-delete"), "Delete",
                                       this);
      contextMenu.addAction(deleteAct);

      contextMenu.addSeparator();

      QString configureIconFile =
          (iString)Filename("$base/icons/plot_configure.png").Expanded();
      QAction *configureAct = new QAction(QPixmap(configureIconFile),
                                          "Configure...", this);
      contextMenu.addAction(configureAct);

      QAction *chosenAct = contextMenu.exec(
          m_legendItem->mapToGlobal(event->pos()));

      if (chosenAct == cutAct || chosenAct == copyAct) {
        QApplication::clipboard()->setMimeData(createMimeData());

        if (chosenAct == cutAct) {
          deleteThisCurve = true;
        }
      }
      else if (chosenAct == configureAct) {
        CubePlotCurveConfigureDialog configure(this);
        configure.exec();
        emit needsRepaint();
      }
      else if (chosenAct == deleteAct) {
        deleteThisCurve = true;
      }
    }

    if (deleteThisCurve) {
      setColor(QColor(255, 255, 255, 0));
      emit needsRepaint();

      QwtPlot *plotToReplot = plot();

      connect(this, SIGNAL(removing()),
              this, SLOT(deleteLater()), Qt::QueuedConnection);
      connect(this, SIGNAL(removing()),
              plotToReplot, SLOT(replot()), Qt::QueuedConnection);
      emit removing();
    }
  }
}

