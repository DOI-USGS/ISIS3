#ifndef SensorInfoWidget_H
#define SensorInfoWidget_H

#include <QFrame>
#include <QString>

namespace Ui {
  class SensorInfoWidget;
}

namespace Isis {
  class Directory;

  /**
   * @brief
   *
   * @ingroup ControlNetworks
   *
   * @author 2015-07-10 Ken Edmundson
   *
   * @internal
   *   @history 2015-07-10 Ken Edmundson - Original version.
   */
  class GuiCamera;

  class SensorInfoWidget : public QFrame {
    Q_OBJECT

    public:
      explicit SensorInfoWidget(GuiCamera* camera, Directory *directory, QWidget *parent = 0);
      ~SensorInfoWidget();

    private:
      Ui::SensorInfoWidget *m_ui;

//      QString formatPoleRaString();
//      QString formatPoleDecString();
//      QString formatPmString();

      Directory *m_directory;
      GuiCamera *m_camera;
  };
}

#endif // SensorInfoWidget_H
