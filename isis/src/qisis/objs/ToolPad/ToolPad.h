#ifndef Qisis_ToolPad_h
#define Qisis_ToolPad_h

#include <QToolBar>
#include <QActionGroup>

namespace Qisis {
  class ToolPad : public QToolBar {
    Q_OBJECT

    public:
      /**
       * Toolpad constructor.
       * 
       * 
       * @param label 
       * @param parent 
       */
      ToolPad (const QString &label, QWidget *parent) : QToolBar(label,parent) {
        p_actionGroup = new QActionGroup(parent);
        p_actionGroup->setExclusive(true);
        setIconSize(QSize(22,22));
      }


      /**
       * Adds an action to the action group and tool bar.
       * 
       * 
       * @param action 
       */
      void addAction (QAction *action) {
        action->setCheckable(true);
        p_actionGroup->addAction(action);
        QToolBar::addAction(action);
      }

    private:
      QActionGroup *p_actionGroup;//!< Group of actions.
  };
};

#endif
