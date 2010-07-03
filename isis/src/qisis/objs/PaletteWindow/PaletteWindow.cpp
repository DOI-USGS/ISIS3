#include "PaletteWindow.h"

namespace Qisis {
  /**
   * PaletteWindow constructor
   * 
   * 
   * @param parent 
   */
  PaletteWindow::PaletteWindow(QWidget *parent) : QMainWindow(parent) {
    readSettings();
    parent->installEventFilter(this);
    p_appName = parent->windowTitle().toStdString();
  }


  /** 
   * This method is overridden so that we can be sure to write the 
   * current settings of the Palette window. 
   * 
   * @param event
   */
  void PaletteWindow::closeEvent(QCloseEvent *event) {
    writeSettings(); 
  }


  /** 
   * This method ensure that the settings get written even if the 
   * Palette window was only hidden, not closed. 
   * 
   * @param event
   */
  void PaletteWindow::hideEvent(QHideEvent *event) {
    writeSettings(); 
  }


  /** 
   * This method is called from the constructor so that when the
   * Palette window is created, it know's it's size and location.
   * 
   */
  void PaletteWindow::readSettings(){
    std::string instanceName = this->windowTitle().toStdString();
    Isis::Filename config("$HOME/.Isis/"+ p_appName + "/" + instanceName + ".config");
    QSettings settings(QString::fromStdString(config.Expanded()),QSettings::NativeFormat);
    QPoint pos = settings.value("pos", QPoint(300, 100)).toPoint();
    QSize size = settings.value("size", QSize(900, 500)).toSize();
    resize(size);
    move(pos);
  }


  /** 
   * This method is called when the Palette window is closed or
   * hidden to write the size and location settings to a config
   * file in the user's home directory.
   * 
   */
  void PaletteWindow::writeSettings() {
    /*We do not want to write the settings unless the window is 
      visible at the time of closing the application*/
    if(!this->isVisible()) return;

    std::string instanceName = this->windowTitle().toStdString();
    Isis::Filename config("$HOME/.Isis/"+ p_appName + "/" + instanceName + ".config");
    QSettings settings(QString::fromStdString(config.Expanded()),QSettings::NativeFormat);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
  }


  /** 
   * This event filter is installed on the parent of this window.
   * When the user closes the main window of the application, the 
   * Palette windows will write their settings even though they did 
   * not receive the close event themselves. 
   * 
   * @param o
   * @param e
   * 
   * @return bool
   */
  bool PaletteWindow::eventFilter(QObject *o,QEvent *e) {
    switch (e->type()) {
      case QEvent::Close:{
        writeSettings();
      }

      default: {
        return false;
      }
    }
  }

}

