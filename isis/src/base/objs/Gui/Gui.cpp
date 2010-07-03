#include <string>
#include <sstream>

#include <QWidget>

#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QIcon>
#include <QToolBar>
#include <QSplitter>
#include <QScrollArea>
#include <QStatusBar>
#include <QLineEdit>
#include <QWhatsThis>
#include <QMessageBox>
#include <QFrame>
#include <QDesktopWidget>

#include "Gui.h"
#include "UserInterface.h"
#include "Preference.h"
#include "iString.h"
#include "iException.h"
#include "Application.h"
#include "PvlObject.h"
#include "PvlGroup.h"
#include "Pvl.h"
#include "SessionLog.h"
#include "Filename.h"

namespace Isis {

  //! Singleton
  Gui *Gui::p_gui = NULL;
  Gui *Gui::Create(Isis::UserInterface &ui, int &argc, char *argv[]) {
    // Don't recreate
    if(p_gui != NULL) return p_gui;

    // Get preferences
    PvlGroup &uiPref = Preference::Preferences().FindGroup("UserInterface");

    // Create the application
    new QApplication(argc, argv);
    QApplication::setQuitOnLastWindowClosed(true);

    Isis::Filename qtpluginpath("$ISISROOT/3rdParty/plugins");
    QCoreApplication::addLibraryPath(qtpluginpath.Expanded().c_str());


    // Qt is smart enough to use the style of the system running the program.
    // However, Isis supports overriding this with a setting in IsisPreferences.
    // Here we check to see if this has been done and force the style if needed.
    if(uiPref.HasKeyword("GuiStyle")) {
      std::string style = uiPref["GuiStyle"];
      QApplication::setStyle((iString)style);
    }

    // Create the main window
    p_gui = new Gui(ui);
    p_gui->show();

    return p_gui;
  }

  //! Constructor
  Gui::Gui(Isis::UserInterface &ui) : QMainWindow(0, Qt::Window) {
    // Create the toolbar and menu and populate them with actions
    CreateAreas();

    // Add parameters to the main area
    for(int group = 0; group < ui.NumGroups(); group++) {
      for(int param = 0; param < ui.NumParams(group); param++) {
        p_parameters.push_back(AddParameter(ui, group, param));
      }
    }

    // Load the values from the UI into the GUI
    for(int p = 0; p < (int)p_parameters.size(); p++) {
      GuiParameter *param = p_parameters[p];
      param->Update();
      connect(param, SIGNAL(ValueChanged()), this, SLOT(UpdateCommandLine()));
    }


    // Make the horizontal direction in the scrolling widget non-stretchable
    p_scrollLayout->addStretch(1);

    // Setup status bar
    p_progressBar = new QProgressBar();
    p_progressBar->setMinimum(0);
    p_progressBar->setMaximum(100);
    p_progressBar->setValue(0);
    p_progressBar->setMinimumWidth(200);

    p_statusText = new QLabel("Ready");

    statusBar()->setSizeGripEnabled(true);
    statusBar()->addWidget(p_progressBar, 0);
    statusBar()->addWidget(p_statusText, 3);

    // Setup the current history pointer
    p_historyEntry = -1;

  }

  //! Destructor
  Gui::~Gui() {
    for(unsigned int i = 0; i < p_parameters.size(); i++) {
      delete p_parameters[i];
    }

    p_parameters.clear();
  }

  // Create the main widget, menus, toolbars, status, actions
  void Gui::CreateAreas() {
    // Create the main area
    QSplitter *split = new QSplitter(Qt::Vertical, this);

    // Add a scrolled area for the parameters to the splitter
    p_scrollArea = new QScrollArea();
    p_scrollWidget = new QWidget();
    p_scrollLayout = new QVBoxLayout;
    p_scrollWidget->setLayout(p_scrollLayout);
    p_scrollArea->setWidget(p_scrollWidget);
    p_scrollArea->setWidgetResizable(true);

    // Set the scroll area size
    int height = QApplication::desktop()->height();

    // Add the log area to the bottom of the spliter
    p_log = new GuiLog();
    p_log->setMinimumHeight(10);
    p_log->resize(p_log->width(), 250);

    split->addWidget(p_scrollArea);
    split->addWidget(p_log);
    split->setChildrenCollapsible(false);
    split->setStretchFactor(0, 3);
    split->setStretchFactor(1, 0);
    setCentralWidget(split);
    resize(720, (int)(height / 2) + 350);

    // Create all the actions for menus, toolbars...
    p_processAction = CreateProcessAction();
    p_stopAction = CreateStopAction();
    p_exitAction = CreateExitAction();

    p_previousHistoryAction = CreatePreviousHistoryAction();
    p_nextHistoryAction = CreateNextHistoryAction();
    p_resetAction = CreateResetAction();

    p_saveLogAction = CreateSaveLogAction();
    p_clearLogAction = CreateClearLogAction();

    QAction *whatsThisAction = CreateWhatsThisAction();

    // Create the File menu
    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(p_processAction);
    fileMenu->addAction(p_stopAction);
    fileMenu->addAction(p_exitAction);

    // Create the Options menu
    QMenu *optionsMenu = menuBar()->addMenu("&Options");
    optionsMenu->addAction(p_resetAction);
    optionsMenu->addAction(p_previousHistoryAction);
    optionsMenu->addAction(p_nextHistoryAction);
    optionsMenu->addAction(p_saveLogAction);
    optionsMenu->addAction(p_clearLogAction);

    // Create the Controls Toolbar
    QToolBar *tb = addToolBar("Controls");
    tb->setIconSize(QSize(22, 22));
    tb->addAction(p_processAction);
    tb->addAction(p_stopAction);
    tb->addAction(p_exitAction);
    tb->addSeparator();

    tb->addAction(p_previousHistoryAction);
    tb->addAction(p_nextHistoryAction);
    tb->addAction(p_resetAction);
    tb->addSeparator();

    tb->addAction(p_saveLogAction);
    tb->addAction(p_clearLogAction);
    tb->addSeparator();

    tb->addAction(whatsThisAction);

    QAction *showControls = new QAction(this);
    showControls->setText("Controls");
    showControls->setCheckable(true);
    connect(showControls, SIGNAL(toggled(bool)), tb, SLOT(setVisible(bool)));

    tb->installEventFilter(this);

    // Create the command line toolbar
    tb = new QToolBar("Command Line");
    addToolBar(Qt::BottomToolBarArea, tb);
    tb->setIconSize(QSize(22, 22));
    tb->setAllowedAreas(Qt::BottomToolBarArea);
    p_commandLineEdit = new QLineEdit(tb);
    p_commandLineEdit->setReadOnly(true);
    tb->addWidget(p_commandLineEdit);
    QAction *showCommandLine = new QAction(this);
    showCommandLine->setText("Command Line");
    showCommandLine->setCheckable(true);
    connect(showCommandLine, SIGNAL(toggled(bool)), tb, SLOT(setVisible(bool)));
    //tb->hide();

    // Create the view menu
    QMenu *viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(showControls);
    viewMenu->addAction(showCommandLine);
    showControls->setChecked(true);
    showCommandLine->setChecked(true);

    // Create the Help menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(whatsThisAction);

    QAction *aboutProgram = new QAction(this);
    aboutProgram->setText("About this program");
    aboutProgram->setShortcut(Qt::CTRL + Qt::Key_H);
    helpMenu->addAction(aboutProgram);
    connect(aboutProgram, SIGNAL(triggered(bool)), this, SLOT(AboutProgram()));

    QAction *aboutIsis = new QAction(this);
    aboutIsis->setText("About Isis");
    aboutIsis->setShortcut(Qt::CTRL + Qt::Key_I);
    helpMenu->addAction(aboutIsis);
    connect(aboutIsis, SIGNAL(triggered(bool)), this, SLOT(AboutIsis()));
  }


  // Create the "Begin/Start Processing" action
  QAction *Gui::CreateProcessAction() {
    QAction *processAction = new QAction(this);
    QString baseDir = (iString)Filename("$BASE/icons").Expanded();
    processAction->setIcon(QPixmap(baseDir + "/guiRun.png"));
    processAction->setText("&Run");
    processAction->setToolTip("Run");
    QString processActionWhatsThisText = "<p><b>Function: </b> \
                      Runs the application with the current parameters</p> \
                      <p><b>Shortcut: </b> Ctrl+R</p>";
    processAction->setShortcut(Qt::CTRL + Qt::Key_R);
    processAction->setWhatsThis(processActionWhatsThisText);

    connect(processAction, SIGNAL(triggered(bool)), this, SLOT(StartProcess()));

    return processAction;
  }

  /**
   *  The user pressed the go button
   *
   *  @internal
   *  @history  2007-05-16 Tracie Sucharski - Change cursor to wait while
   *                                   processing.
   *
   */
  void Gui::StartProcess() {
    p_processAction->setEnabled(false);
    ProgressText("Working");
    Progress(0);
    p_stop = false;

    Isis::UserInterface &ui = Isis::iApp->GetUserInterface();

    // Pull the values from the parameters and put them into the Aml
    for(int p = 0; p < (int)p_parameters.size(); p++) {
      GuiParameter &param = *(p_parameters[p]);
      ui.Clear(param.Name());
      if(param.IsEnabled() && param.IsModified()) {
        Isis::iString value = param.Value();
        value.ConvertWhiteSpace();
        value.Compress();
        value.Trim(" ");
        if(value.length() > 0) {
          ui.PutAsString(param.Name(), value);
        }
      }
    }

    // Make sure the parameters were valid
    // Call the application's main
    ProcessEvents();
    try {
      ui.VerifyAll();
      ui.SaveHistory();
      Isis::SessionLog::TheLog(true);
      QApplication::setOverrideCursor(Qt::WaitCursor);
      (*p_funct)();  // Call IsisMain
      QApplication::restoreOverrideCursor();
      Isis::iApp->FunctionCleanup();

      // Display the parameters incase the app changed one or more
      for(int p = 0; p < (int)p_parameters.size(); p++) {
        GuiParameter &param = *(p_parameters[p]);
        param.Update();
      }

      Progress(100);
      ProgressText("Done");
    }
    catch(Isis::iException &e) {
      QApplication::restoreOverrideCursor();
      if(e.Type() == Isis::iException::Cancel) {
        e.Clear();
        ProgressText("Stopped");
      }
      else {
        Isis::iApp->FunctionError(e);
        if(ShowWarning()) exit(0);
        ProgressText("Error");
      }
    }

    p_processAction->setEnabled(true);
  }

  // Create the "Exit" action
  QAction *Gui::CreateExitAction() {
    QAction *exitAction = new QAction(this);
    QString baseDir = (iString)Filename("$BASE/icons").Expanded();
    exitAction->setIcon(QPixmap(baseDir + "/guiExit.png"));
    exitAction->setText("&Exit");
    exitAction->setToolTip("Exit");
    QString exitWhatsThisText = "<p><b>Function: </b> \
               Closes the program window </p> <p><b>Shortcut: </b> Ctrl+Q</p>";
    exitAction->setWhatsThis(exitWhatsThisText);
    exitAction->setShortcut(Qt::CTRL + Qt::Key_Q);
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    return exitAction;
  }

  // Create the "Reset" action
  QAction *Gui::CreateResetAction() {
    QAction *resetAction = new QAction(this);
    QString baseDir = (iString)Filename("$BASE/icons").Expanded();
    resetAction->setIcon(QPixmap(baseDir + "/guiReset.png"));
    resetAction->setText("&Reset");
    resetAction->setToolTip("Reset parameters");
    QString resetWhatsThisText = "<p><b>Function: </b> \
                Resets the application parameters to their default values</p> \
                <p><b>Shortcut: </b> F3</p>";
    resetAction->setWhatsThis(resetWhatsThisText);
    resetAction->setShortcut(Qt::Key_F3);
    connect(resetAction, SIGNAL(triggered()), this, SLOT(ResetParameters()));

    return resetAction;
  }

  // Create the "Stop" action
  QAction *Gui::CreateStopAction() {
    QAction *stopAction = new QAction(this);
    QString baseDir = (iString)Filename("$BASE/icons").Expanded();
    stopAction->setIcon(QPixmap(baseDir + "/guiStop.png"));
    stopAction->setText("&Stop");
    stopAction->setToolTip("Stop");
    QString stopWhatsThisText = "<p><b>Function: </b> \
                Stops the application from running</p> \
                <p><b>Shortcut: </b> Ctrl+E</p>";
    stopAction->setShortcut(Qt::CTRL + Qt::Key_E);
    stopAction->setWhatsThis(stopWhatsThisText);
    connect(stopAction, SIGNAL(triggered()), this, SLOT(StopProcessing()));

    return stopAction;
  }

  // Create the "SaveLog" action
  QAction *Gui::CreateSaveLogAction() {
    QAction *saveLogAction = new QAction(this);
    QString baseDir = (iString)Filename("$BASE/icons").Expanded();
    saveLogAction->setIcon(QPixmap(baseDir + "/guiSaveLog.png"));
    saveLogAction->setText("&Save Log...");
    saveLogAction->setToolTip("Save log");
    QString saveWhatsThisText = "<p><b>Function: </b> Saves the information \
           currently in the log area to a file <p><b>Shortcut: </b> Ctrl+S</p>";
    saveLogAction->setWhatsThis(saveWhatsThisText);
    saveLogAction->setShortcut(Qt::CTRL + Qt::Key_S);
    connect(saveLogAction, SIGNAL(triggered(bool)), p_log, SLOT(Save()));

    return saveLogAction;
  }

  // Create the "ClearLog" action
  QAction *Gui::CreateClearLogAction() {
    QAction *clearlogAction = new QAction(this);
    QString baseDir = (iString)Filename("$BASE/icons").Expanded();
    clearlogAction->setIcon(QPixmap(baseDir + "/guiClearLog.png"));
    clearlogAction->setText("&Clear Log");
    clearlogAction->setToolTip("Clear log");
    QString clearWhatsThisText = "<p><b>Function: </b>Clears all information \
        from the log area at the bottom of the application screen</p> \
        <p><b>Shortcut: </b> Ctrl+L</p>";
    clearlogAction->setWhatsThis(clearWhatsThisText);
    clearlogAction->setShortcut(Qt::CTRL + Qt::Key_L);
    connect(clearlogAction, SIGNAL(triggered(bool)), p_log, SLOT(Clear()));

    return clearlogAction;
  }

  // Create the "Previous History" action
  QAction *Gui::CreatePreviousHistoryAction() {
    QAction *previousHistoryAction = new QAction(this);
    QString baseDir = (iString)Filename("$BASE/icons").Expanded();
    previousHistoryAction->setIcon(QPixmap(baseDir + "/guiPrevHistory.png"));
    previousHistoryAction->setText("&Previous");
    previousHistoryAction->setToolTip("Previous parameters");
    QString previousWhatsThisText = "<p><b>Function: </b>Fills in parameter \
            values using the previous history entry</p> \
            <p><b>Shortcut: </b> F5</p>";
    previousHistoryAction->setWhatsThis(previousWhatsThisText);
    previousHistoryAction->setShortcut(Qt::Key_F5);
    connect(previousHistoryAction, SIGNAL(triggered()), this, SLOT(PreviousHistory()));

    return previousHistoryAction;
  }

  // Create the "Next History" action
  QAction *Gui::CreateNextHistoryAction() {
    QAction *nextHistoryAction = new QAction(this);
    QString baseDir = (iString)Filename("$BASE/icons").Expanded();
    nextHistoryAction->setIcon(QPixmap(baseDir + "/guiNextHistory.png"));
    nextHistoryAction->setText("&Next");
    nextHistoryAction->setToolTip("Next parameters");
    QString nextWhatsThisText = "<p><b>Function: </b>Fills in parameter \
            values using the next history entry</p> \
            <p><b>Shortcut: </b>F6</p>";
    nextHistoryAction->setWhatsThis(nextWhatsThisText);
    nextHistoryAction->setShortcut(Qt::Key_F6);
    connect(nextHistoryAction, SIGNAL(triggered()), this, SLOT(NextHistory()));

    return nextHistoryAction;
  }

  // Create the Whats Action action
  QAction *Gui::CreateWhatsThisAction() {
    QAction *action = new QAction(this);
    QString baseDir = (iString)Filename("$BASE/icons").Expanded();
    action->setIcon(QPixmap(baseDir + "/contexthelp.png"));
    action->setText("&What's This");
    action->setToolTip("What's This");
    QString whatsThisText = "<p><b>Function: </b> Use this to get longer \
         descriptions of button functions and parameter information</p> \
         <p><b>Shortcut: </b> Shift+F1</p>";
    action->setWhatsThis(whatsThisText);
    action->setShortcut(Qt::SHIFT + Qt::Key_F1);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(WhatsThis()));

    return action;
  }

  // Add a new parameter to this main window
  GuiParameter *Gui::AddParameter(Isis::UserInterface &ui, int group, int param) {
    // Create the group box if this is the first parameter in the group
    QGridLayout *gridLayout = NULL;
    if(!p_grids.contains(ui.GroupName(group))) {
      // Create a new groupbox and add it to the scroll layout
      QGroupBox *groupBox = new QGroupBox((iString)ui.GroupName(group));
      p_scrollLayout->addWidget(groupBox);
      groupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
      groupBox->setAlignment(Qt::AlignHCenter);

      // Create a gridlayout for the new groupbox and save it
      gridLayout = new QGridLayout;
      gridLayout->setColumnStretch(0, 0);
      gridLayout->setColumnStretch(1, 0);
      gridLayout->setColumnMinimumWidth(1, 10);
      gridLayout->setColumnStretch(2, 10);
      groupBox->setLayout(gridLayout);
      p_grids[ui.GroupName(group)] = gridLayout;
    }
    // Find the group box for this parameter
    else {
      gridLayout = p_grids[ui.GroupName(group)];
    }

    GuiParameter *p = GuiParameterFactory::Create(gridLayout, ui, group, param);
    connect(p, SIGNAL(ValueChanged()), this, SLOT(UpdateExclusions()));
    connect(p, SIGNAL(HelperTrigger(const QString &)),
            this, SLOT(InvokeHelper(const QString &)));
    return p;
  }

  //! Change progress text
  void Gui::ProgressText(const std::string &text) {
    p_statusText->setText((iString)text);
    qApp->processEvents();  // Needed when programs run programs
  }

  //! Update the progress bar
  void Gui::Progress(int percent) {
    p_progressBar->setValue(percent);
    qApp->processEvents();  // Needed when programs run programs
  }

  /**
   * Start the Gui and enter the main loop
   * This routine only returns when the program is ready to exit
   */
  int Gui::Exec(void (*funct)()) {
    p_funct = funct;
    return qApp->exec();
  }

  //! Add more information to the error message
  void Gui::LoadMessage(const std::string &message) {
    // Convert newlines to breaks
    std::string m = message;
    while(m.find("\n") != std::string::npos) {
      m.replace(m.find("\n"), 1, "<br>");
    }

    // If there is a set of "[]" change everything between them to red text
    if((message.find("[") >= 0) &&
        (message.rfind("]") < message.size()) &&
        (message.find("[") < message.rfind("]"))) {

      int indx = 0;
      while(m.find("[", indx) != std::string::npos) {
        m.insert(m.find("[", indx) + 1, "<font color=#ff0000>");
        m.insert(m.find("]", indx), "</font>");
        indx = m.find("]", indx) + 1;
      }
    }
    p_errorString += (QString)(iString)m;
  }

  //! Show an error message and return if the user wants to continue/abort
  int Gui::ShowWarning() {
    Isis::UserInterface &ui = Isis::iApp->GetUserInterface();
    int status = QMessageBox::warning(this,
                                      (iString)ui.ProgramName(),
                                      p_errorString,
                                      "Ok", "Abort", "", 0, 1);
    p_errorString.clear();
    return status;
  }

  //! Write text to the gui log
  void Gui::Log(const std::string &text) {
    QString s = (iString)text;
    p_log->Write(s);
  }

  //! Reset the Progress bar when the user moves the mouse onto the toolbar
  bool Gui::eventFilter(QObject *o, QEvent *e) {
    if(e->type() == QEvent::Enter) {
      if(p_processAction->isEnabled()) {
        ProgressText("Ready");
        Progress(0);
      }
    }
    return FALSE;
  }

  //! The user pressed the stop button ... see what they want to do
  void Gui::StopProcessing() {
    if(p_processAction->isEnabled()) return;

    Isis::UserInterface &ui = Application::GetUserInterface();
    switch(QMessageBox::information(this,
                                    (iString)ui.ProgramName(),
                                    QString("Program suspended, choose to ") +
                                    QString("continue processing, stop ") +
                                    QString("processing or exit the program"),
                                    "Continue",
                                    "Stop",
                                    "Exit", 0, 2)) {
      case 0: // Pressed continue
        break;

      case 1: // Pressed stop
        p_stop = true;
        break;

      case 2: // Pressed exit
        p_stop = true;
        qApp->quit();
    }
  }

  /** Let the event loop have some time to see if we need to cancel.
   * This is normally called by the Isis::Progress class.
   */
  bool Gui::ProcessEvents() {
    qApp->processEvents();
    return p_stop;
  }

  //! Reset the parameters fields to the defaults
  void Gui::ResetParameters() {
    // Clear the AML to default values
    Isis::UserInterface &ui = Application::GetUserInterface();
    for(int p = 0; p < (int)p_parameters.size(); p++) {
      GuiParameter &param = *(p_parameters[p]);
      ui.Clear(param.Name());
    }

    // Display the updated parameters
    for(int p = 0; p < (int)p_parameters.size(); p++) {
      GuiParameter &param = *(p_parameters[p]);
      param.Update();
    }
  }

  //! Goto the next history entry
  void Gui::NextHistory() {
    p_historyEntry--;
    UpdateHistory();
  }

  //! Goto the previous history entry
  void Gui::PreviousHistory() {
    p_historyEntry++;
    UpdateHistory();
  }

  //! Changed the parameters based on the history pointer
  void Gui::UpdateHistory() {
    if(p_historyEntry < -1) {
      p_historyEntry = -1;
      QApplication::beep();
      return;
    }

    if(p_historyEntry == -1) {
      ResetParameters();
      return;
    }

    // Find out if this application has a history file
    Isis::UserInterface &ui = Application::GetUserInterface();
    Preference &p = Preference::Preferences();

    PvlGroup &grp = p.FindGroup("UserInterface", Isis::Pvl::Traverse);
    Isis::Filename progHist(grp["HistoryPath"][0] + "/" + ui.ProgramName() + ".par");

    if(!progHist.Exists()) {
      p_historyEntry = -1;
      QApplication::beep();
      return;
    }

    Isis::Pvl hist;

    try {
      hist.Read(progHist.Expanded());
    }
    catch(...) {
      p_historyEntry =  -1;
      std::string msg = "A corrupt parameter history file [" + progHist.Expanded() +
                        "] has been detected. Please fix or remove this file";
      LoadMessage(msg);
      if(ShowWarning()) exit(0);
      return;
    }

    int entries = 0;
    for(int i = 0; i < hist.Groups(); i++) {
      if(hist.Group(i).IsNamed("UserParameters")) entries++;
    }

    // If we are past the last entry ring the bell
    if(p_historyEntry == entries) {
      p_historyEntry = entries - 1;
      QApplication::beep();
      return;
    }

    int useEntry = entries - p_historyEntry - 1;

    try {
      Isis::PvlGroup &up = hist.Group(useEntry);
      for(int k = 0; k < up.Keywords(); k++) {
        std::string key = up[k].Name();
        std::string val = up[k];
        ui.Clear(key);
        ui.PutAsString(key, val);
      }

      for(unsigned int p = 0; p < p_parameters.size(); p++) {
        GuiParameter &param = *(p_parameters[p]);
        param.Update();
      }

    }
    catch(Isis::iException &e) {
      p_historyEntry = entries - 1;
      QApplication::beep();
      return;
    }
  }

  /** Grey out parameters that should be excluded for radio buttons and
   * checkboxes
   */
  void Gui::UpdateExclusions() {
    // First enable everything
    for(unsigned int p = 0; p < p_parameters.size(); p++) {
      GuiParameter &param = *(p_parameters[p]);
      param.SetEnabled(true);
    }

    // Now disable things
    for(unsigned int p = 0; p < p_parameters.size(); p++) {
      GuiParameter &param = *(p_parameters[p]);
      std::vector<std::string> excludeList = param.Exclusions();
      for(int i = 0; i < (int)excludeList.size(); i++) {
        for(unsigned int e = 0; e < p_parameters.size(); e++) {
          GuiParameter &exclude = *(p_parameters[e]);
          if(exclude.Name() != excludeList[i]) continue;
          exclude.SetEnabled(false);
        }
      }
    }
  }

  //! Update the command line toolbar
  void Gui::UpdateCommandLine() {
    std::string cline = Isis::Application::GetUserInterface().ProgramName();
    for(int p = 0; p < (int)p_parameters.size(); p++) {
      GuiParameter &param = *(p_parameters[p]);
      if(param.IsEnabled() && param.IsModified()) {
        cline += " ";
        iString name = param.Name();
        name.DownCase();
        cline += name;
        cline += "=";
        iString value = param.Value();
        if(param.Type() == GuiParameter::StringWidget) {
          if(value.find(" ") != std::string::npos) {
            cline += "\"" + value + "\"";
          }
          else {
            cline += value;
          }
        }
        else if(param.Type() == GuiParameter::FilenameWidget ||
                param.Type() == GuiParameter::CubeWidget) {
          cline += value;
        }
        else {
          value.DownCase();
          cline += value;
        }
      }
    }
    p_commandLineEdit->setText(cline.c_str());
  }

  //! Update Parameters
  void Gui::UpdateParameters() {
    for(unsigned int p = 0; p < p_parameters.size(); p++) {
      GuiParameter &param = *(p_parameters[p]);
      param.Update();
    }
  }

  // Enter into what's this mode
  void Gui::WhatsThis() {
    QWhatsThis::enterWhatsThisMode();
  }

  // Show help for Isis
  void Gui::AboutIsis() {
    Isis::PvlGroup &uig = Isis::Preference::Preferences().FindGroup("UserInterface");
    std::string command = (std::string) uig["GuiHelpBrowser"] +
                          " http://isis.astrogeology.usgs.gov >> /dev/null &";
    system(command.c_str());
  }

  // Show help for the current app
  void Gui::AboutProgram() {
    Isis::Filename file((std::string)
                        "$ISISROOT/doc/Application/presentation/PrinterFriendly/" +
                        Isis::Application::GetUserInterface().ProgramName() +
                        "/" +
                        Isis::Application::GetUserInterface().ProgramName() +
                        ".html");

    Isis::PvlGroup &uig = Isis::Preference::Preferences().FindGroup("UserInterface");
    std::string command = (std::string) uig["GuiHelpBrowser"] +
                          (std::string)" file:" + file.Expanded() + " &";
    system(command.c_str());
  }

  //! Activate helper buttons
  void Gui::InvokeHelper(const QString &funct) {
    p_processAction->setEnabled(false);
    try {
      Isis::UserInterface &ui = Application::GetUserInterface();

      // Pull the values from the parameters and put them into the Aml
      for(int p = 0; p < (int)p_parameters.size(); p++) {
        GuiParameter &param = *(p_parameters[p]);
        ui.Clear(param.Name());
        if(param.IsEnabled() && param.IsModified()) {
          Isis::iString value = param.Value();
          value.ConvertWhiteSpace();
          value.Compress();
          value.Trim(" ");
          if(value.length() > 0) {
            ui.PutAsString(param.Name(), value);
          }
        }
      }

      // Get the helper function and run
      void *ptr = Isis::iApp->GetGuiHelper(funct.toStdString());
      void (*helper)();
      helper = (void ( *)())ptr;
      helper();
    }
    catch(Isis::iException &e) {
      Isis::iApp->GuiReportError(e);
    }

    // Update parameters in GUI
    UpdateParameters();
    p_processAction->setEnabled(true);
  }

}
