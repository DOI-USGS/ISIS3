#ifndef ControlTreeWidgetItem_H
#define ControlTreeWidgetItem_H

#include <QObject>
#include <QTreeWidgetItem>


namespace Isis {
  class Control;

  /**
   * @brief A control in the project tree widget
   *
   * This class visualizes a Control * from the project in the project tree widget.
   *
   * @author 2012-09-26 Tracie Sucharski
   *
   * @internal
   */
  class ControlTreeWidgetItem : public QObject, public QTreeWidgetItem {
    Q_OBJECT
    public:
      ControlTreeWidgetItem(Control *control, QTreeWidget *parent = 0);
      virtual ~ControlTreeWidgetItem();

      Control *control();
      void selectionChanged();

    private:
      Control *m_control;
  };
}

#endif

