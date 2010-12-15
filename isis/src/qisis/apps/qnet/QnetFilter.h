#ifndef QnetFilter_h
#define QnetFilter_h

#include <QAction>
#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QList>


namespace Qisis {
  class QnetFilter : public QWidget {
    Q_OBJECT

    public:
      QnetFilter (QWidget *parent);
      virtual ~QnetFilter () {};
      virtual void filter() { return; };

    signals:
      void filteredListModified();
  };
};

#endif
