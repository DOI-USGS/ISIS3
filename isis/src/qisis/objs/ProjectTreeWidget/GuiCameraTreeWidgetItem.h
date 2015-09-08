#ifndef GuiCameraTreeWidgetItem_H
#define GuiCameraTreeWidgetItem_H

#include <QObject>
#include <QTreeWidgetItem>

#include "GuiCamera.h"

namespace Isis {

  /**
   * @brief A control in the project tree widget
   *
   * This class visualizes a GuiCamera * from the project in the project tree widget.
   *
   * @author 2015-06-23 Ken Edmundson
   *
   * @internal
   *   @history 2015-06-23 Ken Edmundson - Original version.
   */
  class GuiCameraTreeWidgetItem : public QObject, public QTreeWidgetItem {
    Q_OBJECT
    public:
      GuiCameraTreeWidgetItem(GuiCameraQsp guiCamera,
                                       QTreeWidget *parent = 0);
      virtual ~GuiCameraTreeWidgetItem();

      GuiCameraQsp guiCamera();
      void selectionChanged();

    private:
      GuiCameraQsp m_guiCamera;
  };
}

#endif

