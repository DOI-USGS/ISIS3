#ifndef SensorInfoWidget_H
#define SensorInfoWidget_H

#include <QFrame>
#include <QString>

namespace Ui {
  class SensorInfoWidget;
}

namespace Isis {
  class Directory;
  class GuiCamera;

  /**
   * @brief Widget for displaying information about a sensor
   *
   * @ingroup ControlNetworks
   *
   * @author 2015-07-10 Ken Edmundson
   *
   * @internal
   *   @history 2015-07-10 Ken Edmundson - Original version.
   *   @history 2017-08-14 Summer Stapleton - Updated icons/images to properly licensed or open 
   *                           source images. Fixes #5105.
   *   @history 2018-07-26 Tracie Sucharski - Reformated the widget to get rid of fixed sizes and
   *                           use layouts to handle sizing instead.  Also put entire widget in
   *                           a scrolled area.
   */
  class SensorInfoWidget : public QFrame {
    Q_OBJECT

    public:
      explicit SensorInfoWidget(GuiCamera* camera, Directory *directory, QWidget *parent = 0);
      ~SensorInfoWidget();

    private:
      Ui::SensorInfoWidget *m_ui; //!< The widget ui

//      QString formatPoleRaString();
//      QString formatPoleDecString();
//      QString formatPmString();

      Directory *m_directory; //!< Unused
      GuiCamera *m_camera; //!< The sensor whose information is being displayed
  };
}

#endif // SensorInfoWidget_H
