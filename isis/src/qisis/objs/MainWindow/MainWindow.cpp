#include "MainWindow.h"
#include <iostream>

namespace Qisis {
  /**
   * Mainwindow constructor
   * 
   * 
   * @param title 
   * @param parent 
   * @param flags 
   */
  MainWindow::MainWindow(QString title, QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags) {
    this->setWindowTitle(title);
    if(parent){
      p_appName = parent->windowTitle().toStdString();
    }
    
    readSettings();
  }


  /**
   * 
   * 
   */
  MainWindow::~MainWindow(){
  }


  /** 
   * This method is overridden so that we can be sure to write the 
   * current settings of the Main window. 
   * 
   * @param event
   */
  void MainWindow::closeEvent(QCloseEvent *event) {
    writeSettings(); 
  }


  /** 
   * This method ensure that the settings get written even if the 
   * Main window was only hidden, not closed. 
   * 
   * @param event
   */
  void MainWindow::hideEvent(QHideEvent *event) {
    writeSettings(); 
  }


  /** 
   * This method is called from the constructor so that when the
   * Main window is created, it know's it's size and location.
   * 
   */
  void MainWindow::readSettings(){
    if(p_appName == "") {
      p_appName = this->windowTitle().toStdString();
    }

    std::string instanceName = this->windowTitle().toStdString();
    Isis::Filename config("$HOME/.Isis/"+ p_appName + "/" + instanceName + ".config");
    QSettings settings(QString::fromStdString(config.Expanded()),QSettings::NativeFormat);
    QPoint pos = settings.value("pos", QPoint(300, 100)).toPoint();
    QSize size = settings.value("size", QSize(900, 500)).toSize();
    resize(size);
    move(pos);
  }


  /** 
   * This method is called when the Main window is closed or
   * hidden to write the size and location settings to a config
   * file in the user's home directory.
   * 
   */
  void MainWindow::writeSettings() {
    /*We do not want to write the settings unless the window is 
      visible at the time of closing the application*/
    if(!this->isVisible()) return;

    if(p_appName == "") {
      p_appName = this->windowTitle().toStdString();
    }

    std::string instanceName = this->windowTitle().toStdString();
    Isis::Filename config("$HOME/.Isis/"+ p_appName + "/" + instanceName + ".config");
    QSettings settings(QString::fromStdString(config.Expanded()),QSettings::NativeFormat);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
  }


   /** 
   * This event filter is installed on the parent of this window.
   * When the user closes the main window of the application, the 
   * Mainwindow will write their settings even though they did not
   * receive the close event themselves. 
   * 
   * @param o
   * @param e
   * 
   * @return bool
   */
   bool MainWindow::eventFilter(QObject *o,QEvent *e) {
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

