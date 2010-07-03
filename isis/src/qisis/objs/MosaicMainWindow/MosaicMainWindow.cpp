#include "MosaicMainWindow.h"

#include <QMenu>
#include <QSettings>

#include "FileDialog.h"
#include "ToolPad.h"


namespace Qisis {
  MosaicMainWindow::MosaicMainWindow (QString title, QWidget *parent) : 
    Qisis::MainWindow(title, parent) {

    p_filename = "";
    installEventFilter(this);

    this->setWindowTitle(title);

    p_permToolbar = new QToolBar("Standard Tools",this);
    p_permToolbar->setObjectName("perm");
    p_permToolbar->setAllowedAreas(Qt::TopToolBarArea|Qt::BottomToolBarArea);
    p_permToolbar->setIconSize(QSize(22,22));
    this->addToolBar(p_permToolbar);

    p_activeToolbar = new QToolBar("Active Tool",this);
    p_activeToolbar->setObjectName("Active");
    p_activeToolbar->setAllowedAreas(Qt::TopToolBarArea|Qt::BottomToolBarArea);
    p_activeToolbar->setIconSize(QSize(22,22));
    this->addToolBar(p_activeToolbar);
    
    QStatusBar *sbar = statusBar();
    sbar->showMessage("Ready");

    p_toolpad = new ToolPad("Tool Pad",this);
    p_toolpad->setObjectName("MosaicMainWindow");
    p_toolpad->setAllowedAreas(Qt::LeftToolBarArea|Qt::RightToolBarArea);
    this->addToolBar(Qt::RightToolBarArea,p_toolpad);

    p_progressBar = new QProgressBar(parent);
    p_progressBar->setOrientation(Qt::Horizontal);
    sbar->addWidget(p_progressBar);

    setupMenus();
    readSettings();
  }


  /**
   * Sets up the menus on the menu bar for the qmos window.
   */
  void MosaicMainWindow::setupMenus(){
    // Create the file menu 
    QMenu *fileMenu = menuBar()->addMenu("&File");

    QAction *open = new QAction(this);
    open->setText("Open...");
    open->setIcon(QPixmap(QString::fromStdString(Isis::Filename("$base/icons").Expanded().c_str()) + "/fileopen.png"));
    connect(open,SIGNAL(activated()),this,SLOT(open()));

    QAction *openList = new QAction(this);
    openList->setText("Import List...");
    openList->setIcon(QPixmap(QString::fromStdString(Isis::Filename("$base/icons").Expanded().c_str())+"/mActionHelpContents.png"));
    connect(openList,SIGNAL(activated()),this,SLOT(openList()));

    QAction *saveList = new QAction(this);
    saveList->setText("Export List...");
    //saveList->setIcon(QPixmap(QString::fromStdString(Isis::Filename("$base/icons").Expanded().c_str())+"/filesaveas.png"));
    connect(saveList,SIGNAL(activated()),this,SLOT(saveList()));

    QAction *exportView = new QAction(this);
    exportView->setText("Export View...");
    //exportView->setIcon(QPixmap(QString::fromStdString(Isis::Filename("$base/icons").Expanded().c_str())+"/fileexport.png"));
    connect(exportView,SIGNAL(activated()),this,SLOT(exportView()));

    QAction *saveProject = new QAction(this);
    saveProject->setText("Save Project...");
    saveProject->setShortcut(Qt::CTRL+Qt::Key_S);
    saveProject->setIcon(QPixmap(QString::fromStdString(Isis::Filename("$base/icons").Expanded().c_str()) + "/mActionFileSave.png"));
    connect(saveProject,SIGNAL(activated()),this,SLOT(saveProject()));

    QAction *saveProjectAs = new QAction(this);
    saveProjectAs->setText("Save Project As...");
    saveProjectAs->setIcon(QPixmap(QString::fromStdString(Isis::Filename("$base/icons").Expanded().c_str()) + "/mActionFileSaveAs.png"));
    connect(saveProjectAs,SIGNAL(activated()),this,SLOT(saveProjectAs()));

    QAction *loadProject = new QAction(this);
    loadProject->setText("Load Project...");                                                                //linguist-editpaste
    loadProject->setIcon(QPixmap(QString::fromStdString(Isis::Filename("$base/icons").Expanded().c_str()) + "/mActionExportMapServer.png"));
    connect(loadProject,SIGNAL(activated()),this,SLOT(loadProject()));

    QAction *close = new QAction(this);
    close->setText("Close");
    connect(close,SIGNAL(activated()),this,SLOT(close()));

    fileMenu->addAction(open);
    fileMenu->addAction(openList);
    fileMenu->addAction(saveList);
    fileMenu->addAction(exportView);
    fileMenu->addSeparator();
    fileMenu->addAction(saveProject);
    fileMenu->addAction(loadProject);
    fileMenu->addSeparator();
    fileMenu->addAction(close);

    permanentToolBar()->addAction(loadProject);
    permanentToolBar()->addAction(saveProject);
    permanentToolBar()->addAction(saveProjectAs);
    permanentToolBar()->addSeparator();
    permanentToolBar()->addAction(open);
    permanentToolBar()->addAction(openList);
    permanentToolBar()->addSeparator();

    p_viewMenu = menuBar()->addMenu("View");
    p_viewMenu->setTearOffEnabled(true);
    connect(p_viewMenu, SIGNAL(triggered(QAction *)), this, SLOT(hideShowColumns(QAction *)));
    
    p_itemColumn = new QAction(this);
    p_itemColumn->setText("Item Column");
    p_itemColumn->setCheckable(true);
    p_itemColumn->setChecked(true);

    p_footprintColumn = new QAction(this);
    p_footprintColumn->setText("Footprint Column");
    p_footprintColumn->setCheckable(true);
    p_footprintColumn->setChecked(true);

    p_outlineColumn = new QAction(this);
    p_outlineColumn->setText("Outline Column");
    p_outlineColumn->setCheckable(true);
    p_outlineColumn->setChecked(true);

    p_imageColumn = new QAction(this);
    p_imageColumn->setText("Image Column");
    p_imageColumn->setCheckable(true);
    p_imageColumn->setChecked(true);

    p_labelColumn = new QAction(this);
    p_labelColumn->setText("Label Column");
    p_labelColumn->setCheckable(true);
    p_labelColumn->setChecked(true);

    p_resolutionColumn = new QAction(this);
    p_resolutionColumn->setText("Resolution Column");
    p_resolutionColumn->setCheckable(true);
    p_resolutionColumn->setChecked(true);

    p_emissionAngleColumn = new QAction(this);
    p_emissionAngleColumn->setText("Emission Angle Column");
    p_emissionAngleColumn->setCheckable(true);
    p_emissionAngleColumn->setChecked(true);

    p_incidenceAngleColumn = new QAction(this);
    p_incidenceAngleColumn->setText("Incidence Angle Column");
    p_incidenceAngleColumn->setCheckable(true);
    p_incidenceAngleColumn->setChecked(true);

    p_islandColumn = new QAction(this);
    p_islandColumn->setText("Island Column");
    p_islandColumn->setCheckable(true);
    p_islandColumn->setChecked(true);

    p_notesColumn = new QAction(this);
    p_notesColumn->setText("Notes Column");
    p_notesColumn->setCheckable(true);
    p_notesColumn->setChecked(true);

    p_referenceFootprint = new QAction(this);
    p_referenceFootprint->setText("Show Reference Footprint");
    p_referenceFootprint->setCheckable(true);
    p_referenceFootprint->setChecked(false);

    p_viewMenu->addSeparator();
    p_viewMenu->addAction(p_itemColumn);
    p_viewMenu->addAction(p_footprintColumn);
    p_viewMenu->addAction(p_outlineColumn);
    p_viewMenu->addAction(p_imageColumn);
    p_viewMenu->addAction(p_labelColumn);
    p_viewMenu->addAction(p_resolutionColumn);
    p_viewMenu->addAction(p_emissionAngleColumn);
    p_viewMenu->addAction(p_incidenceAngleColumn);
    p_viewMenu->addAction(p_islandColumn);
    p_viewMenu->addAction(p_notesColumn);
    p_viewMenu->addSeparator();
    p_viewMenu->addAction(p_referenceFootprint);

  }


  /**
   * Allows users to decide which columns in the tree widget they 
   * would like to have visible. 
   * 
   * 
   * @param action 
   */
  void MosaicMainWindow::hideShowColumns(QAction *action){
    ((MosaicWidget *)centralWidget())->viewMenuAction(action);
  }


  /** 
   * Calles MosaicWidget's open method which opens a cube file and
   * displays the footprint in the graphics view.
   * 
   */
  void MosaicMainWindow::open () {
   ((MosaicWidget *)centralWidget())->open();
  }


  /** 
   * Opens a list of cube files instead of one at a time.
   * 
   */
  void MosaicMainWindow::openList () {
  ((MosaicWidget *)centralWidget())->openList();
  }


  /** 
   * This overriden method is called from the constructor so that 
   * when the Mosaicmainwindow is created, it know's it's size 
   * and location and the tool bar location. 
   * 
   */
  void MosaicMainWindow::readSettings(){
    // Call the base class function to read the size and location
    this->MainWindow::readSettings();
    // Now read the settings that are specific to this window.
    std::string instanceName = this->windowTitle().toStdString();
    Isis::Filename config("$HOME/.Isis/" + instanceName + "/" + instanceName + ".config");
    QSettings settings(QString::fromStdString(config.Expanded()), QSettings::NativeFormat);
    QByteArray state = settings.value("state", QByteArray("0")).toByteArray();
    restoreState(state);
    QList <QAction *> actionList = p_viewMenu->actions();
    for(int i = 0; i < actionList.size(); i++){
      QString itemTitle = actionList[i]->text();     
      bool state = (bool)settings.value(itemTitle, 0).toBool();
      actionList[i]->setChecked(state);
    }
    p_referenceFootprint->setChecked(false);
  }


  /** 
   * This overriden method is called when the Mosaicmainwindow 
   * is closed or hidden to write the size and location settings 
   * (and tool bar location) to a config file in the user's home 
   * directory. 
   * 
   */
  void MosaicMainWindow::writeSettings(){
    // Call the base class function to write the size and location
    this->MainWindow::writeSettings();
    // Now write the settings that are specific to this window.
    std::string instanceName = this->windowTitle().toStdString();
    Isis::Filename config("$HOME/.Isis/" + instanceName + "/" + instanceName + ".config");
    QSettings settings(QString::fromStdString(config.Expanded()), QSettings::NativeFormat);
    settings.setValue("state", this->saveState());

    //-----------------------------------------------------------
    // Save the check state of all the actions in the view menu.
    //-----------------------------------------------------------
    QList <QAction *>actionList = p_viewMenu->actions();
    for(int i = 0; i < actionList.size(); i++){
        QString itemTitle = actionList[i]->text();
        settings.setValue(itemTitle, actionList[i]->isChecked());
      }
   /* settings.setValue("nameColumn", p_nameColumn->isChecked());
    settings.setValue("itemColumn", p_itemColumn->isChecked());
    settings.setValue("footprintColumn", p_footprintColumn->isChecked());
    settings.setValue("outlineColumn", p_outlineColumn->isChecked());
    settings.setValue("imageColumn", p_imageColumn->isChecked());
    settings.setValue("labelColumn", p_labelColumn->isChecked());
    settings.setValue("resolutionColumn", p_resolutionColumn->isChecked());
    settings.setValue("emissionColumn", p_emissionAngleColumn->isChecked());
    settings.setValue("incidenceAngleColumn", p_incidenceAngleColumn->isChecked());
    settings.setValue("islandColumn", p_islandColumn->isChecked());
    settings.setValue("notesColumn", p_notesColumn->isChecked());
    settings.setValue("referenceFootprint", p_referenceFootprint->isChecked());*/
  }


  /** 
   * Saves the list of cubes to a text file.
   * 
   */
  void MosaicMainWindow::saveList(){
    QString fn =  QFileDialog::getSaveFileName(this, "Save File", 
                                                     QDir::currentPath() + "/untitled.lis",
                                                     "List (*.lis)");
    if (fn.isEmpty()) return;
    QString filename;

    if(!fn.endsWith(".lis")) {
        filename = fn + ".lis";
      } else {
        filename = fn;
      }
   ((MosaicWidget *)centralWidget())->saveList(filename);
  }


  /**
   * Allows the user to save a project file.
   * 
   */
  void MosaicMainWindow::saveProjectAs(){
    QString fn =  QFileDialog::getSaveFileName(this, "Save Project", 
                                                     QDir::currentPath() + "/untitled.mos",
                                                     "Mosaic (*.mos)");
    if (fn.isEmpty()) return;
    QString filename;

    if(!fn.endsWith(".mos")) {
        filename = fn + ".mos";
      } else {
        filename = fn;
      }
    ((MosaicWidget *)centralWidget())->saveProject(filename);
    p_filename = filename;
  }


  /**
   * Called from the file menu to save a project file.
   * 
   */
  void MosaicMainWindow::saveProject(){
    //--------------------------------------
    // If the project does not already have
    // a name, call saveProjectAs
    //--------------------------------------
    if(p_filename == "" ) {
      saveProjectAs();
      return;
    }
    ((MosaicWidget *)centralWidget())->saveProject(p_filename);
  }


  /**
   * Allows users to select a project which is then read in and
   * displayed in the qmos window.
   * 
   */
  void MosaicMainWindow::loadProject(){
    QString fn =  QFileDialog::getOpenFileName(this, "Load Project", 
                                                     QDir::currentPath(),
                                                     "Mosaic (*.mos)");
    if (fn.isEmpty()) return;
    QString filename;

    if(!fn.endsWith(".mos")) {
        filename = fn + ".mos";
      } else {
        filename = fn;
      }
    ((MosaicWidget *)centralWidget())->readProject(filename);
    p_filename = filename;
  }


  /**
   * Saves the graphics view as a png, jpg, of tif file.
   * 
   */
  void MosaicMainWindow::exportView () {
  
    QString output =
      QFileDialog::getSaveFileName((QWidget *)parent(),
                                   "Choose output file",
                                   QDir::currentPath() + "/untitled.png",
                                   QString("Images (*.png *.jpg *.tif)"));
    if (output.isEmpty()) return;
    
    // Use png format is the user did not add a suffix to their output filename.
    if(QFileInfo(output).suffix().isEmpty()){
      output = output + ".png";
    }

    QString format = QFileInfo(output).suffix();
    QPixmap pm = QPixmap::grabWidget(((MosaicWidget *)centralWidget())->scene()->views().last());
    
    std::string formatString = format.toStdString();
    if (!pm.save(output,formatString.c_str())) {
      QMessageBox::information((QWidget *)parent(),"Error","Unable to save "+output);
      return;
    }
  }
  

  /** 
   * This event filter is installed in the constructor.
   * @param o
   * @param e
   * 
   * @return bool
   */
  bool MosaicMainWindow::eventFilter(QObject *o,QEvent *e) {

    switch (e->type()) {
      case QEvent::Close:{
        writeSettings();
        break;
      }
      
      default: {
        return false;
      }
    }
    return false;
  }
}

