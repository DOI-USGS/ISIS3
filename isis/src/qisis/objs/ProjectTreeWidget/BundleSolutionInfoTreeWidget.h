#ifndef BundleSolutionInfoTreeWidgetItem_H
#define BundleSolutionInfoTreeWidgetItem_H

#include <QObject>
#include <QTreeWidgetItem>


namespace Isis {
  class BundleSolutionInfo;

  /**
   * @brief A control in the project tree widget
   *
   * This class visualizes a BundleSolutionInfo * from the project in the project tree widget.
   *
   * @author 2014-07-23 Ken Edmundson
   *
   * @internal
   *   @history 2015-02-20 Jeannie Backer - Name changed from BundleResults to
   *                           BundleSolutionInfo.
   */
  class BundleSolutionInfoTreeWidgetItem : public QObject, public QTreeWidgetItem {
    Q_OBJECT
    public:
      BundleSolutionInfoTreeWidgetItem(BundleSolutionInfo *bundleSolutionInfo, 
                                       QTreeWidget *parent = 0);
      virtual ~BundleSolutionInfoTreeWidgetItem();

      BundleSolutionInfo *bundleSolutionInfo();
      void selectionChanged();

    private:
      BundleSolutionInfo *m_bundleSolutionInfo;
  };
}

#endif

