#ifndef BundleStatisticsTreeWidgetItem_H
#define BundleStatisticsTreeWidgetItem_H

#include <QObject>
#include <QTreeWidgetItem>


namespace Isis {
  class BundleStatistics;

  /**
   * @brief A control in the project tree widget
   *
   * This class visualizes a BundleStatistics * from the project in the project tree widget.
   *
   * @author 2014-07-23 Ken Edmundson
   *
   * @internal
   */
  class BundleStatisticsTreeWidgetItem : public QObject, public QTreeWidgetItem {
    Q_OBJECT
    public:
      BundleStatisticsTreeWidgetItem(BundleStatistics *bundleStatistics, QTreeWidget *parent = 0);
      virtual ~BundleStatisticsTreeWidgetItem();

      BundleStatistics *bundleStatistics();
      void selectionChanged();

    private:
      BundleStatistics *m_bundleStatistics;
  };
}

#endif

