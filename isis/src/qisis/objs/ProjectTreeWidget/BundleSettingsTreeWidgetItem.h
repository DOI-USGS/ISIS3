#ifndef BundleSettingsTreeWidgetItem_H
#define BundleSettingsTreeWidgetItem_H

#include <QObject>
#include <QTreeWidgetItem>

#include "BundleSettings.h"


namespace Isis {

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
      BundleSettingsTreeWidgetItem(BundleSettingsQsp bundleSettings, QTreeWidget *parent = 0);
      virtual ~BundleSettingsTreeWidgetItem();

      BundleSettingsQsp bundleSettings();
      void selectionChanged();

    private:
      BundleSettingsQsp m_bundleSettings;
  };
}

#endif

