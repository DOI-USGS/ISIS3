#ifndef BundleSettingsTreeWidgetItem_H
#define BundleSettingsTreeWidgetItem_H

#include <QObject>
#include <QTreeWidgetItem>


namespace Isis {
  class BundleSettings;

  /**
   * @brief A control in the project tree widget
   *
   * This class visualizes a BundleSettings * from the project in the project tree widget.
   *
   * @author 2014-07-23 Ken Edmundson
   *
   * @internal
   */
  class BundleSettingsTreeWidgetItem : public QObject, public QTreeWidgetItem {
    Q_OBJECT
    public:
      BundleSettingsTreeWidgetItem(BundleSettings *bundleSettings, QTreeWidget *parent = 0);
      virtual ~BundleSettingsTreeWidgetItem();

      BundleSettings *bundleSettings();
      void selectionChanged();

    private:
      BundleSettings *m_bundleSettings;
  };
}

#endif

