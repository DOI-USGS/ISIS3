#ifndef _GuiEditFile_h_
#define _GuiEditFile_h_
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <QAction>
#include <QFile>
#include <QObject>
#include <QMainWindow>
#include <QString>
#include <QTextEdit>
#include <QWidget>

/**
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

namespace Isis {
  class UserInterface;
 /**
  * @brief  Opens a window in Gui Application to be able to 
  * edit, save and create text files. Creates a <b>singleton</b> window 
  * to edit files.
  *  
  * Example can be used to edit DefFiles in control net applications. 
  * Refer to cnetedit Application
  *
  * @author 2011-05-21 Sharmila Prasad
  *
  * @internal
  * @history 2011-05-21  Sharmila Prasad - Initial Version 
  * @history 2011-10-07  Sharmila Prasad - Added message if the file does not have  
  *                      write permissions and is unable to load it. 
  */
  
  class GuiEditFile : public QObject {
    Q_OBJECT
    public:
      //! Creates a single instance of the GuiEditFile
      static void EditFile(UserInterface & pUI, QString psFile="");

      //! If there is already an instance of this object, then display the window
      void showWindow(QString psFile="");
      
      //! Destructor
      ~GuiEditFile();
      
      //! Delete the contents of a file
      void clearFile();

    public slots:
      void open();             //!< For action File->Open
      void OpenFile(QString);  //!< To display the contents of an opened file
      void setTextChanged();   //!< Indicator that the text has changed
      void saveAs();           //!< For action File->Save As
      void saveAsFile(QString);//!< Save the contents of text editor to another file
      void saveFile();         //!< For action File->Save
      void closeFile();        //!< For action File->Close
      void closeWin();         //!< For action Exit (close the window)

    private:
      //! Constructor
      GuiEditFile(UserInterface & pUI, QString psFile="");
      void windowTitle(QString & psfile); //!< display only the file base name
      
      static GuiEditFile* m_instance; //!< Instance of this object - singleton
      QWidget *m_parent;              //!< Parent widget
      QString m_fileName;             //!< Current file open
      QMainWindow *m_editWin;         //!< Editor window
      QTextEdit *m_txtEdit;           //!< Text Editor
      QFile *m_editFile;              //!< File pointer to current file 
      bool m_textChanged;             //!< Flag to indicate text changed
      
      //! Actioons
      QAction *m_open;    //!< Action Open
      QAction *m_save;    //!< Action Save
      QAction *m_saveAs;  //!< Action Save As
      QAction *m_close;   //!< Action Close
      QAction *m_exit;    //!< Action Exit
  };

};
#endif

