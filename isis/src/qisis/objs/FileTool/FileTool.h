#ifndef FileTool_h
#define FileTool_h

#include "Tool.h"

#include <QDir>
#include <QPointer>
#include <QStringList>

class QAction;

namespace Isis {
  class Brick;
  class Buffer;
  class Cube;
  class CubeAttributeOutput;
  class MdiCubeViewport;
  class SaveAsDialog;

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2008-12-10 Jeannie Walldren - Added "What's this?"
   *              and shortcut to "Save" action
   *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport
   *   @history 2011-04-05 Sharmila Prasad - Added SaveInfo option to save
   *              the current cubeviewport's whatsthis info
   *   @history 2011-05-11 Sharmila Prasad - Added SaveAsDialog to select the output
   *                   cube and options to save FullImage, ExportAsIs, ExportFullRes
   *   @history  2012-02-01 Sharmila Prasad - Fixed bug #0000681 - reduce in ISIS 3.3.0
   *                   is ignoring the +N band identifier
   *   @history  2012-05-24 Steven Lambright - Minor changes to support prompting to save on exit
   *                            once again (this has been broken for a very long time). The prompt
   *                            now appears if you have edited your file but not saved it - not when
   *                            clicking "Save." This was a minimal fix (I left a lot of problems
   *                            to be solved at a later date). Fixes #854.
   *   @history  2012-06-04 Steven Lambright - Fixed a problem with the save button. References
   *                            #854.
   *   @history  2013-01-07 Tracie Sucharski - Added AlphaCube to output cube for saving the
   *                            full resolution viewport image.  Fixes # 706.
   *   @history  2013-01-09 Tracie Sucharski - Fixed saveAsCubeByOption which was not creating the
   *                            cube for the case where the scale was 1 and saveAs_FullResolution
   *                            was being called without an initialized ocube.  Fixes #1386.
   *   @history  2013-01-17 Tracie Sucharski - Fixed some round-off errors when calculating
   *                            output lines/samples.  Fixed printing of Results pvl group where
   *                            samples and lines were swapped.  References #1385.
   *   @history 2016-04-21 Makayla Shepherd - Added UnsignedWord pixel type handling.
   *   @history 2016-09-14 Ian Humphrey - Modified exportView() and print()- replaced deprecated
   *                           static QPixmap::grabWidget with QWidget::grab to remove deprecation
   *                           warnings from stdout when using File->Print or File->Export View.
   *                           References #4304.
   *   @history 2017-06-07 Christopher Combs - Changed saveAsEnlargedCube's catch block to stack
   *                           errors and give a more accurate error message.
   *   @history 2017-09-11 Adam Goins - Added the ExportToList menu item to allow all open cubes
   *                           to be exported into a cubelist. References #5097
   *  @history 2018-07-27 Kaitlyn Lee - Added unsigned/signed integer pixel type handling.
   */
  class FileTool : public Tool {
      Q_OBJECT

    public:
      FileTool(QWidget *parent);
      void addTo(QMenu *menu);
      void addTo(Workspace *ws);
      void addToPermanent(QToolBar *perm);
      //! @return the open action
      QPointer<QAction> openAction() {
        return p_open;
      };
      //! @return the save as action
      QPointer<QAction> saveAction() {
        return p_saveAs;
      };
      QStringList p_filterList; //!< Filter List
      QDir p_dir;               //!< Directory
      QStringList p_fileList;   //!< File list


      //! @return the menu name for the file tool
      QString menuName() const {
        return "&File";
      }

    signals:
      void fileSelected(QString); //!< This signal is called when a file is selected
      /**
       * This signal is called when changes should be saved
       *
       * @param vp
       */
      void saveChanges(CubeViewport *vp);
      /**
       * This signal is called when changes should be discarded
       *
       * @param vp
       */
      void discardChanges(CubeViewport *vp);

    public slots:
      virtual void open();
      virtual void browse();
      void print();
      virtual void save();
      virtual void saveAs();
      virtual void saveInfo(); //!< Saves the whatsthis info of the cubeviewport
      virtual void exportView();
      virtual void exportToList();
      virtual bool closeAll();
      virtual void exit();
      void enableSave(bool enable);
      void discard();
      void saveAsCubeByOption(QString); //!< Save as Isis Cube (FullImage, AsIs, FullRes)

    protected:
      //! Updates the tool
      void updateTool();

    private:
      static void copy(Buffer &in, Buffer &out);
      QPointer<QAction> p_open;   //!< Action to open a file
      QPointer<QAction> p_browse; //!< Action to browse and open files
      QPointer<QAction> p_print;  //!< Action to print the current view
      QPointer<QAction> p_save;   //!< Action to save the current cube
      QPointer<QAction> p_saveAs; //!< Action save the current cube as a user specified file
      QPointer<QAction> p_saveInfo;     //!< Action to save the current cube's Whatsthis info
      QPointer<QAction> p_exportView;   //!< Action to export the view as a picture
      QPointer<QAction> p_exportToList; //!< Action to export active cubes to a cube list
      QPointer<QAction> p_closeAll;     //!< Action to close all windows
      QPointer<QAction> p_exit;         //!< Action to exit qview
      QPointer<QWidget> p_parent;       //!< The parent widget of this object
      QString p_lastDir;     //!< The last directory opened
      QPointer<Workspace> p_workSpace;          //!< The workspace being used
      QPointer<MdiCubeViewport> p_lastViewport; //!< The last cubeviewport that was used
      QPointer<SaveAsDialog> p_saveAsDialog;    //!< SaveAs Dialog with different save options

      /**
       * Saves the cube as a full image
       *
       * @param icube The input Cube
       * @param ocube The output Cube
       */
      void saveAsFullImage(Cube *icube, Cube *ocube);

      /** Copy input cube details into output file given its dimensions
       *
       *
       * @param psOutFile The psFileName
       * @param icube The input cube
       * @param ocube The output cube
       * @param piNumSamples The number samples
       * @param piNumLines The number of lines
       * @param piNumBands The number of bands
       */
      void copyCubeDetails(const QString & psFileName, Cube *icube,
           Cube *ocube, int piNumSamples, int piNumLines, int piNumBands);

      /** Save image AsIs (As viewed in the viewport window) into output file
       *
       * @param icube The input Cube
       * @param psOutFile The output file
       */
      void saveAs_AsIs(Cube *icube, const QString & psOutFile);

      /** Save image Full Resolution (image viewed in the viewport window) into output
       *
       * @param pInCube The input cube.
       * @param pOutCube The output cube.
       * @param pNumSamples The number of samples
       * @param pNumLines The number of lines
       */
      void saveAs_FullResolution(Cube *pInCube, Cube *pOutCube,
                                 int pNumSamples, int pNumLines);

      /** Save image AsIs Enlarged into output
       *
       * @param icube The input cube
       * @param psOutFile The output file
       */
      void saveAsEnlargedCube(Cube *icube, const QString & psOutFile);

      /**
       *
       * @param icube The input cube
       * @param psOutFile The output file
       */
      void saveAsReducedCube (Cube *icube, const QString & psOutFile);
  };
};

#endif
