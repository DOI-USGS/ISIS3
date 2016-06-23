#ifndef TargetInfoWidget_H
#define TargetInfoWidget_H

#include <QFrame>
#include <QString>

namespace Ui {
  class TargetInfoWidget;
}

namespace Isis {
  class Directory;

  /**
   * @brief Widget for displaying information about a target
   *
   * @ingroup ControlNetworks
   *
   * @author 2015-06-13 Ken Edmundson
   *
   * @internal
   *   @history 2015-06-13 Ken Edmundson - Original version.
   */
  class TargetBody;

  class TargetInfoWidget : public QFrame {
    Q_OBJECT

    public:
      explicit TargetInfoWidget(TargetBody* target, Directory *directory, QWidget *parent = 0);
      ~TargetInfoWidget();

    private:
      Ui::TargetInfoWidget *m_ui; //!< The widget's ui

      QString formatPoleRaString();
      QString formatPoleDecString();
      QString formatPmString();

      Directory *m_directory; //!< Unused
      TargetBody *m_target; //!< The target whose information is being displayed
  };
}

#endif // TargetInfoWidget_H
