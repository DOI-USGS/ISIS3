#ifndef QtieTool_h
#define QtieTool_h

#include <QAction>
#include <QPushButton>
#include <QToolButton>
#include <QString>

#include "Tool.h"
#include "Table.h"
#include "Cube.h"
#include "ControlPointEdit.h"
#include "SerialNumber.h"
#include "SerialNumberList.h"
#include "ImageOverlap.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "ChipViewport.h"
#include "Pvl.h"
#include "AutoRegFactory.h"

class QLineEdit;
class QMainWindow;

namespace Qisis {
  class MdiCubeViewport;

/**
  * @brief Tool for picking Tie points
  *
  * @ingroup Visualization Tools
  *
  * @author 2008-09-09 Tracie Sucharski 
  *  
  * @history 2008-11-19  Tracie Sucharski - Addition option to constructor 
  *                          to allow mouse events on leftChipViewport.
  * @history 2009-06-10  Tracie Sucharski - Added new slot, clearFiles which 
  *                          allow new files to be opened.
  * @history  2010-05-18 Jeannie Walldren - Modified createPoint() method Point ID
  *                           Dialog to return if "Cancel" is clicked.
  * @history 2010-05-18  Tracie Sucharski - Added pointId to the Tie tool 
  *                          Dialog.
  *
  */
  class QtieTool : public Qisis::Tool {
    Q_OBJECT

    public:
      QtieTool (QWidget *parent);
      void paintViewport (MdiCubeViewport *cvp,QPainter *painter);
//      void addTo (QMenu *menu);
      static QString lastPtIdValue;

    signals:
      void tieToolSave();
      void editPointChanged();
      void newSolution(Isis::Table *cmatrix);

    public slots:
      void setFiles(Isis::Cube &baseCube,Isis::Cube &matchCube,Isis::ControlNet &cnet);
      void clearFiles();
      void createPoint(double lat,double lon);
      void modifyPoint(Isis::ControlPoint *point);
      void deletePoint(Isis::ControlPoint *point);

    protected:
      QAction *toolPadAction(ToolPad *pad);

    protected slots:
      void mouseButtonRelease(QPoint p, Qt::MouseButton s);

    private slots:
      void drawMeasuresOnViewports();
      void solve();
      void writeNewCmatrix(Isis::Table *cmatrix);
      void saveNet();

      void pointSaved ();

      void setTemplateFile();
      void viewTemplateFile();
      //void setInterestOp();
      void setTwist(bool twist) { p_twist = twist; };
      void setIterations(int maxIterations) { p_maxIterations = maxIterations; };
      //void setTolerance () { p_tolerance = p_tolValue->text().toDouble(); };

    private:
      void createQtieTool(QWidget *parent);
      QMainWindow *p_tieTool;
      void createMenus();

      void loadPoint();

      QAction *p_createPoint;
      QAction *p_modifyPoint;
      QAction *p_deletePoint;

      QMainWindow *p_mw;
      ControlPointEdit *p_pointEditor;
      QLineEdit *p_tolValue;
      QLabel *p_ptIdValue;

      enum CubeIndex {
        Match,
        Base
      };

      Isis::SerialNumberList *p_serialNumberList;
      Isis::ControlNet *p_controlNet;

      Isis::ControlPoint *p_controlPoint;
      int p_ptIdIndex;

      Isis::ControlMeasure *p_baseMeasure;
      Isis::ControlMeasure *p_matchMeasure;
      Isis::Cube *p_baseCube;
      Isis::Cube *p_matchCube;
      std::string p_baseSN;
      std::string p_matchSN;
      Isis::UniversalGroundMap *p_baseGM;
      Isis::UniversalGroundMap *p_matchGM;

      bool p_twist;
      double p_tolerance;
      int p_maxIterations;

  };
};

#endif
