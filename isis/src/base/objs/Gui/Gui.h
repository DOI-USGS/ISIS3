#ifndef Isis_GuiMainWindow_h
#define Isis_GuiMainWindow_h

/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2010/03/17 15:50:24 $

 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such

 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
   */

  class Gui : public QMainWindow {
      Q_OBJECT

    public:
      static void checkX11();
      
      static Gui *Create(Isis::UserInterface &ui, int &argc, char *argv[]);

      void ProgressText(const std::string &text);
      void Progress(int percent);

      int Exec(void (*funct)());
      bool ProcessEvents();

      void LoadMessage(const std::string &message);
      int ShowWarning();

      void ShowLog() {};
      void Log(const std::string &text);

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

      QMap<std::string, QGridLayout *> p_grids;

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
