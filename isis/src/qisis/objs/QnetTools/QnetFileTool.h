#ifndef QnetFileTool_h
#define QnetFileTool_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FileTool.h"
#include <QCloseEvent>

class QString;
class QWidget;

namespace Isis {
  class ControlNet;
  class ControlPoint;
  class QnetTool;
  class SerialNumberList;

  /**
   * @brief Qnet File operations
   *
   * @ingroup Visualization Tools
   *
   * @author 2006-02-01 Jeff Anderson
   *
   * @internal
   *   @history 2006-08-02 Tracie Sucharski - Initialize cameras for every image
   *                           in cube list.
   *   @history 2008-11-24 Jeannie Walldren - Replace references to PointEdit class
   *                           with ControlPointEdit
   *   @history 2008-11-26 Jeannie Walldren - Added cNetName parameter to
   *                           controlNetworkUpdated() so that QnetTool can read the
   *                           name of the control net file.
   *   @history  2008-11-26 Tracie Sucharski - Remove all polygon/overlap
   *                           polygon/overlap references, this functionality will
   *                           be in qmos.
   *   @history 2008-12-10 Jeannie Walldren - Reworded "What's this?" description
   *                           for saveAs action. Changed "Save As" action text to
   *                           match QnetTool's "Save As" action 2010-06-03 Jeannie
   *                           Walldren - Removed "std::" in .cpp file since "using
   *                           namespace std"
   *   @history 2010-07-01 Jeannie Walldren - Added file extension filters for
   *                           input control network. Replaced #includes with
   *                           forward class declarations and moved #include to
   *                           .cpp file.
   *   @history 2010-10-28 Tracie Sucharski - Fixed some include problems caused
   *                           by changes made to the ControlNet,ControlPoint,
   *                           ControlMeasure header files.
   *   @history 2010-11-17 Eric Hyer - Added newControlNetwork SIGNAL
   *   @history 2010-12-10 Tracie Sucharski - Renamed slot loadPoint to
   *                           loadPointImages.
   *   @history 2011-06-03 Tracie Sucharski - Add Open Ground & Open Dem
   *                           signals.
   *   @history 2011-07-07 Tracie Sucharski - Disable Open Ground and Open Dem
   *                           until list & net open.
   *   @history 2011-11-01 Tracie Sucharski - Added save slot.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards. References #972.
   *   @history 2012-10-11 Debbie A. Cook, Updated to use new Target class.  References Mantis tickets
   *                           #775 and #1114.
   *   @history 2016-04-22 Jeannie Backer - Modified to use cube labels to set
   *                           ControlNet's target instead of the TargetName.
   *                           References #3892
   *  @history 2018-04-24 Adam Goins - Added QCloseEvent optional parameter to slot "exit()" to
   *                          set the CloseEvent triggered by an onwindowclose
   *                          to ignore the event if the 'cancel' option was selected
   *                          after clicking the close button of the viewport window.
   *                          This fixes an issue where clicking the close button and then clicking
   *                          'cancel' from the QMessageBox would close the window but keep the
   *                          application running. Fixes #4146.
   *
   */

  class QnetFileTool : public FileTool {
      Q_OBJECT

    public:
      QnetFileTool(QnetTool *qnetTool, QWidget *parent);
      ~QnetFileTool();

      virtual void addTo(QMenu *menu);

      ControlNet *controlNet();
      SerialNumberList *serialNumberList();

    signals:
      void serialNumberListUpdated();
      void controlNetworkUpdated(QString cNetName);
      void newControlNetwork(ControlNet *);
      void newGroundFile();
      void newDemFile();

    public slots:
      virtual void open();
      virtual void exit(QCloseEvent *event = NULL);
      virtual void save();
      virtual void saveAs();
      void loadPointImages(ControlPoint *point);
      void loadImage(const QString &serialNumber);
      void setDirty();

    private:
      QString m_cnetFileName;
      bool m_isDirty;
      QAction *m_openGround;
      QAction *m_openDem;
      QnetTool *m_qnetTool;
  };
};

#endif
