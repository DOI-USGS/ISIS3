#ifndef FileTool_h
#define FileTool_h

#include "Tool.h"

#include <QDir>
#include <QStringList>

class QAction;


namespace Isis {
  class Brick;
  class Buffer;
}


namespace Qisis {
  class MdiCubeViewport;

  /**
   *
   * @internal
   *   @history 2008-12-10 Jeannie Walldren - Added "What's this?"
   *              and shortcut to "Save" action
   *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport
   *   @history 2011-04-05 Sharmila Prasad - Added SaveInfo option to save
   *              the current cubeviewport's whatsthis info
   */
  class FileTool : public Tool {
      Q_OBJECT

    signals:
      void fileSelected(QString); //!< This signal is called when a file is selected
      /**
       * This signal is called when changes should be saved
       *
       * @param vp
       */
      void saveChanges(MdiCubeViewport *vp);
      /**
       * This signal is called when changes should be discarded
       *
       * @param vp
       */
      void discardChanges(MdiCubeViewport *vp);

    public:
      FileTool(QWidget *parent);
      void addTo(QMenu *menu);
      void addTo(Workspace *ws);
      void addToPermanent(QToolBar *perm);
      //! Returns the open action
      QAction *openAction() {
        return p_open;
      };
      //! Returns the save as action
      QAction *saveAction() {
        return p_saveAs;
      };
      QStringList p_filterList; //!< Filter List
      QDir p_dir;               //!< Directory
      QStringList p_fileList;   //!< File list

    public slots:
      virtual void open();
      virtual void browse();
      void print();
      void confirmSave();
      virtual void save();
      virtual void saveAs();
      virtual void saveInfo(); //!< Saves the whatsthis info of the cubeviewport
      virtual void exportView();
      virtual bool closeAll();
      virtual void exit();
      void enableSave(bool enable);
      void discard();

    protected:
      //! Returns the menu name for the file tool
      QString menuName() const {
        return "&File";
      };
      //! Updates the tool
      void updateTool();

    private:
      static void copy(Isis::Buffer &in, Isis::Buffer &out);
      QAction *p_open;   //!< Action to open a file
      QAction *p_browse; //!< Action to browse and open files
      QAction *p_print;  //!< Action to print the current view
      QAction *p_save;   //!< Action to save the current cube
      QAction *p_saveAs; //!< Action save the current cube as a user specified file
      QAction *p_saveInfo;   //!< Action to save the current cube's Whatsthis info
      QAction *p_exportView; //!< Action to export the view as a picture
      QAction *p_closeAll;
      QAction *p_exit;       //!< Action to exit qview
      QWidget *p_parent;     //!< The parent widget of this object
      QString p_lastDir;     //!< The last directory opened
      Workspace *p_workSpace;          //!< The workspace being used
      MdiCubeViewport *p_lastViewport; //!< The last cubeviewport that was used
  };
};

#endif
