#ifndef PaletteWindow_h
#define PaletteWindow_h
#include <QMainWindow>
#include <QtGui>
#include <QSettings>
#include "Filename.h"

namespace Qisis {
 

  class PaletteWindow : public QMainWindow {
    Q_OBJECT
    public:
      PaletteWindow (QWidget *parent=0);
      void closeEvent(QCloseEvent *event);
      void hideEvent(QHideEvent *event);
      void readSettings();
      void writeSettings();

    protected:
      bool eventFilter(QObject *o,QEvent *e);

    private:
      std::string p_appName; //!< Application name.


  };
};

#endif
