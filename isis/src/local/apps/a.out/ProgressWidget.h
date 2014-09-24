#ifndef ProgressWidget_H
#define ProgressWidget_H

#include <QProgressBar>

namespace Isis {

  /**
   * @brief Warning Widget for cnetsuite
   *
   * @author 2012-05-29 Steven Lambright and Tracie Sucharski
   *
   * @internal
   */
  class ProgressWidget : public QWidget {
      Q_OBJECT
    public:
      ProgressWidget(QWidget *parent = 0);
      virtual ~ProgressWidget();


    private:
      Q_DISABLE_COPY(ProgressWidget);
      QList<QProgressBar *> m_progressBars;
  };
}

#endif

