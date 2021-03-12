#ifndef Isis_GuiMainWindow_h
#define Isis_GuiMainWindow_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <iostream>
#include <string>
#include <vector>

#include <QAction>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMainWindow>
#include <QProgressBar>
#include <QScrollArea>
#include <QWidget>

#include "PvlObject.h"
#include "PvlGroup.h"

#include "GuiLog.h"
#include "GuiParameter.h"
#include "GuiParameterFactory.h"

namespace Isis {
  /**
   * @brief Gui for Isis Applications
   *
   * This is the main GUI for all Isis Applications.
   *
   * @author  2003-01-01 Stuart Sides
   *
   * @internal
   *  @history 2007-07-19  Steven Lambright Fixed bugs: Command Line not checked
   *                          but displayed initially and Command Line edit was
   *                          not read only.
   *  @history 2008-06-06  Steven Lambright Fixed bug with corrupt parameter
   *                          history file causing the program to crash
   *  @history 2008-07-29  Steven Lambright Fixed memory leaks and naming
   *                          convention
   *  @history 2009-11-19 Kris Becker - Made argc pass by reference since Qt's
   *                         QApplication/QCoreApplication requires it
   *  @history 2010-03-17 Stuart Sides - Added the location of the Qt plugins
   *                                     into the library path
   *  @history 2010-07-19 Jeannie Walldren - Modified SelectFile() in
   *                         GuiFileNameParameter.cpp and removed SelectFile()
   *                         from GuiCubeParameter. Cube and File customization
   *                         preferences are now being handled in Cube's
   *                         CubeIoHandler and IsisAml classes.
   *  @history 2012-11-08 Janet Barrett - Modified the AddParameter method to
   *                         only update exclusions for list and combobox widgets.
   *                         Fixes #624.
   *  @history 2013-01-28 Janet Barrett - Modified the AddParameter method to
   *                         also update exclusions for boolean widgets. References
   *                         #1452.
   *  @history 2016-06-28 Adam Paquette - Modified UpdateHistory to appropriately
   *                         retrieve lists from the history pvl.
   *  @history 2016-08-15 Adam Paquette - Reset locale after QApplication is
   *                         instantiated. Fixes #3908.
   *  @history 2017-05-16 Cole Neubauer - Fixed Title not showing in Putty/Xming
   *                         setup. Fixes #4731.
   *  @history 2017-05-19 Marjorie Hahn - Applied font style and font size from the 
   *                         IsisPreferences file. Fixes #198.
   */

  class Gui : public QMainWindow {
      Q_OBJECT

    public:
      static void checkX11();

      static Gui *Create(Isis::UserInterface &ui, int &argc, char *argv[]);

      void ProgressText(const QString &text);
      void Progress(int percent);

      int Exec(void (*funct)());
      bool ProcessEvents();

      void LoadMessage(const QString &message);
      int ShowWarning();

      void ShowLog() {};
      void Log(const QString &text);

      Gui(Isis::UserInterface &ui);
      ~Gui();

      bool eventFilter(QObject *o, QEvent *e);

    public slots:

    private:
      static Gui *p_gui;

      GuiParameter *AddParameter(Isis::UserInterface &ui, int group, int param);
      void Preferences();
      void CreateAreas();

      QAction *CreateProcessAction();
      QAction *CreateStopAction();
      QAction *CreateExitAction();
      QAction *CreateResetAction();
      QAction *CreatePreviousHistoryAction();
      QAction *CreateNextHistoryAction();
      QAction *CreateSaveLogAction();
      QAction *CreateClearLogAction();
      QAction *CreateWhatsThisAction();

      void (*p_funct)(); // Function to be called for procesing phase

      QScrollArea *p_scrollArea;   // Scrolling area for parameter group boxes
      QWidget *p_scrollWidget;     // Widget with layout for group boxes
      QVBoxLayout *p_scrollLayout; // Layout for all parameter group boxes

      QLabel *p_statusText;
      QProgressBar *p_progressBar;
      GuiLog *p_log;

      QAction *p_processAction;
      QAction *p_exitAction;
      QAction *p_stopAction;
      QAction *p_resetAction;
      QAction *p_saveLogAction;
      QAction *p_clearLogAction;
      QAction *p_previousHistoryAction;
      QAction *p_nextHistoryAction;

      QMap<QString, QGridLayout *> p_grids;

      std::vector<GuiParameter *> p_parameters;

      QString p_errorString;
      bool p_stop;

      void UpdateHistory();
      int p_historyEntry;

      QLineEdit *p_commandLineEdit;

    private slots:
      void StartProcess();
      void StopProcessing();

      void ResetParameters();

      void NextHistory();
      void PreviousHistory();

      void UpdateExclusions();
      void UpdateCommandLine();
      void UpdateParameters();

      void WhatsThis();
      void AboutProgram();
      void AboutIsis();

      void InvokeHelper(const QString &funct);
  };
};



#endif
