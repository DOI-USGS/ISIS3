#ifndef NewGroundSourceLocationDialog_h
#define NewGroundSourceLocationDialog_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <QCheckBox>
#include <QDir>
#include <QFileDialog>
#include <QPointer>
#include <QString>
#include <QWidget>

namespace Isis {

  /**
  * @brief Dialog used by ControlPointEditWidget to select a new location for ground source files.
  * Gives option of using new location for all subsequent ground points and whether to update the
  * control net to reflect the new location.
  *
  * @ingroup Visualization Tools
  *
  * @author 2017-01-05 Tracie Sucharski
  *
  * @internal
  *   @history 2017-01-05 Tracie Sucharski - Initial Version
  */
  class NewGroundSourceLocationDialog : public QFileDialog {
      Q_OBJECT
    public:
      NewGroundSourceLocationDialog(QString title, QString &directory, QWidget *parent = 0);

//    QDir newGroundSourceLocation();
      bool changeAllGroundSourceLocation();
      bool changeControlNet();

    private:
      QPointer<QCheckBox> m_changeAllGround;   //!< Change location of all subsequent ground control points
      QPointer<QCheckBox> m_changeControlNet;  //!< Change location of ground source in the control network
  };
};

#endif
