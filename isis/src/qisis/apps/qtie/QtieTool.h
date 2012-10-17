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

namespace Isis {
  class MdiCubeViewport;

  /**
    * @brief Tool for picking Tie points
    *
    * @ingroup Visualization Tools
    *
    * @author 2008-09-09 Tracie Sucharski
    *
    * @internal
    *   @history 2008-11-19 Tracie Sucharski - Addition option to constructor
    *                           to allow mouse events on leftChipViewport.
    *   @history 2009-06-10 Tracie Sucharski - Added new slot, clearFiles which
    *                           allow new files to be opened.
    *   @history 2010-05-18 Jeannie Walldren - Modified createPoint() method
    *                           Point ID Dialog to return if "Cancel" is
    *                           clicked.
    *   @history 2010-05-18 Tracie Sucharski - Added pointId to the Tie tool
    *                           Dialog.
    *   @history 2010-11-23 Eric Hyer - Added stretchChipViewport SIGNAL for
    *                           forwarding of SIGNAL from StretchTool to
    *                           ChipViewport
    *   @history 2010-12-17 Eric Hyer - Code for open dialog for setting the
    *                           registration template now in this class, not in
    *                           ControlPointEdit
    *   @history 2012-05-10 Tracie Sucharski - Reset pointers to NULL, when creating
    *                           new control point, if doesn't exist on basemap, return.
    *   @history 2012-05-15 Tracie Sucharski - Moved much of the error checking out of this class 
    *                           and into the QtieFileTool class.
    *   @history 2012-06-20 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
    *                           coding standards. References #972.
    * @history 2012-10-11 Debbie A. Cook, Updated to use new Target class.  References Mantis tickets 
    *                           #775 and #1114.
    */
  class QtieTool : public Tool {
      Q_OBJECT

    public:
      QtieTool(QWidget *parent);
      void paintViewport(MdiCubeViewport *cvp, QPainter *painter);
//      void addTo (QMenu *menu);
      static QString lastPtIdValue;

    signals:
      void tieToolSave();
      void editPointChanged();
      void newSolution(Table *cmatrix);
      void stretchChipViewport(Stretch *, CubeViewport *);

    public slots:
      void setFiles(Cube *baseCube, Cube *matchCube, ControlNet *cnet);
      void clearFiles();
      void createPoint(double lat, double lon);
      void modifyPoint(ControlPoint *point);
      void deletePoint(ControlPoint *point);

    protected:
      QAction *toolPadAction(ToolPad *pad);

    protected slots:
      void mouseButtonRelease(QPoint p, Qt::MouseButton s);

    private slots:
      void enterWhatsThisMode();
      void drawMeasuresOnViewports();
      void solve();
      void writeNewCmatrix(Table *cmatrix);
      void saveNet();

      void measureSaved();

      void setTemplateFile();
      void viewTemplateFile();
      //void setInterestOp();
      void setTwist(bool twist) {
        p_twist = twist;
      };
      void setIterations(int maxIterations) {
        p_maxIterations = maxIterations;
      };
      //void setSigma0 () { p_sigma0 = p_tolValue->text().toDouble(); };

    private:
      void createQtieTool(QWidget *parent);
      QMainWindow *p_tieTool;
      void createMenus();
      void createToolBars();

      void loadPoint();

      QAction *p_whatsThis;
      QAction *p_createPoint;
      QAction *p_modifyPoint;
      QAction *p_deletePoint;
      QAction *p_saveNet;

      QMainWindow *p_mw;
      ControlPointEdit *p_pointEditor;
      QLineEdit *p_tolValue;
      QLabel *p_ptIdValue;

      enum CubeIndex {
        Match,
        Base
      };

      SerialNumberList *p_serialNumberList;
      ControlNet *p_controlNet;

      ControlPoint *p_controlPoint;
      int p_ptIdIndex;

      Cube *p_baseCube;
      Cube *p_matchCube;
      std::string p_baseSN;
      std::string p_matchSN;
      UniversalGroundMap *p_baseGM;
      UniversalGroundMap *p_matchGM;

      bool p_twist;
      double p_sigma0;
      int p_maxIterations;

  };
};

#endif
