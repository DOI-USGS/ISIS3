#ifndef ControlGroupTreeWidgetItem_H
#define ControlGroupTreeWidgetItem_H

#include <QObject>
#include <QTreeWidgetItem>


namespace Isis {

  class ControlList;

  /**
   * @author 2012-09-26 Tracie Sucharski
   *
   * @internal
   */
  class ControlGroupTreeWidgetItem : public QObject, public QTreeWidgetItem {
    Q_OBJECT
    public:
      ControlGroupTreeWidgetItem(ControlList *controlList, QTreeWidget *parent = 0);
      virtual ~ControlGroupTreeWidgetItem();

      ControlList *controlList();
      void selectionChanged();

    private slots:
      void updateCount(int newCount);

    private:
      ControlList *m_controlList;
  };
}

#endif

