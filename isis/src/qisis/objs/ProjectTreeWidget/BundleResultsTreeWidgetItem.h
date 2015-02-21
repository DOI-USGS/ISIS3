#ifndef BundleResultsTreeWidgetItem_H
#define BundleResultsTreeWidgetItem_H

#include <QObject>
#include <QTreeWidgetItem>


namespace Isis {
  class BundleResults;

  /**
   * @brief A control in the project tree widget
   *
   * This class visualizes a BundleResults * from the project in the project tree widget.
   *
   * @author 2014-07-23 Ken Edmundson
   *
   * @internal
   *   @history 2015-02-20 Jeannie Backer - Name changed from BundleStatistics to BundleResults.
   */
  class BundleResultsTreeWidgetItem : public QObject, public QTreeWidgetItem {
    Q_OBJECT
    public:
      BundleResultsTreeWidgetItem(BundleResults *bundleResults, QTreeWidget *parent = 0);
      virtual ~BundleResultsTreeWidgetItem();

      BundleResults *bundleResults();
      void selectionChanged();

    private:
      BundleResults *m_bundleResults;
  };
}

#endif

