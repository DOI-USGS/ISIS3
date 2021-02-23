#ifndef BlinkTool_h
#define BlinkTool_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// This is the only include allowed in this file!
#include "Tool.h"

class QDoubleSpinBox;
class QDialog;
class QToolButton;
class QSplitter;
class QMenu;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QAction;
class QListWidget;
class QListWidgetItem;

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   * @internal
   *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
   *            CubeViewport.  Fixed misuse of includes.
   */
  class BlinkTool : public Tool {
      Q_OBJECT

    public:
      BlinkTool(QWidget *parent);
      void addTo(QMenu *menu);
      void addToPermanent(QToolBar *perm);
      void writeSettings();
      void readSettings();

      /**
       * Returns the menu name.
       *
       *
       * @return QString
       */
      QString menuName() const {
        return "&Options";
      };

    protected:
      void updateTool();
      bool eventFilter(QObject *o, QEvent *e);

    private slots:
      void toggleLink(QListWidgetItem *item);
      void reverse();
      void stop();
      void start();
      void advance();
      void timeout();
      void updateWindow();

    private:
      QAction *p_action; //!< The action associated with this tool
      QWidget *p_blinkWindow;//!< The blink tool widget
      QListWidget *p_listWidget;//!< The list widget with the blink tool
      QDoubleSpinBox *p_timeBox;//!< Time selection box
      bool p_timerOn;//!< Is the timer on?
      QDialog *p_dialog;//!< Dialog widget
      QSplitter *p_splitter;
  };
};

#endif
