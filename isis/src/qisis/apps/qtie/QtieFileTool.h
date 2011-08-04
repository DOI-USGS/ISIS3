#ifndef QtieFileTool_h
#define QtieFileTool_h

/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2010/05/12 17:22:27 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <QAction>
#include <QListWidget>
#include <QComboBox>
#include <QDialog>
#include <QStackedWidget>
#include <QLabel>
#include <QStringList>
#include "Tool.h"
#include "FileTool.h"
#include "SerialNumberList.h"
#include "ControlNet.h"
#include "Projection.h"

namespace Isis {
  class Cube;
}

namespace Isis {

  /**
   * @brief Qtie File operations
   *
   * @ingroup Visualization Tools
   *
   * @author 2007-08-09 Tracie Sucharski, adapted from QnetFileTool
   *
   * @internal
   * @history 2009-06-10 Tracie Sucharski - Added new signal for opening new
   *                        files.
   *
   */

  class QtieFileTool : public FileTool {
      Q_OBJECT

    signals:
      void cubesOpened(Cube &baseCube, Cube &matchCube, ControlNet &cnet);
      void newFiles();

    public:
      QtieFileTool(QWidget *parent);

    public slots:
      virtual void open();
      //virtual void exit();

    private:

  };
};

#endif
