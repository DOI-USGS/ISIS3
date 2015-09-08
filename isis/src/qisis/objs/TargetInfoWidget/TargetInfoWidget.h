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
   * @brief
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
      Ui::TargetInfoWidget *m_ui;

      QString formatPoleRaString();
      QString formatPoleDecString();
      QString formatPmString();

      Directory *m_directory;
      TargetBody *m_target;
  };
}

#endif // TargetInfoWidget_H
