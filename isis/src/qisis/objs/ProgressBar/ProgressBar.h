#ifndef ProgressBar_h
#define ProgressBar_h

#include <QProgressBar>

namespace Isis {
  /**   
   *
   * @author ????-??-?? Steven Lambright
   * @internal ????-??-?? Steven Lambright - QProgressBar with customizable text
   *
   *                            
   */

  class ProgressBar : public QProgressBar {
      Q_OBJECT

    public:
      ProgressBar(QWidget *parent = 0);
      ProgressBar(QString textDescription, QWidget *parent = 0);
      ~ProgressBar();

      void setText(QString text);
      virtual QString text() const;

    private:
      QString m_customText;

  };
}

#endif
