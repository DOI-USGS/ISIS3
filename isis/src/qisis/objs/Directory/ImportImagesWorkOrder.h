#ifndef ImportImagesWorkOrder_H
#define ImportImagesWorkOrder_H
/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
 *
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
#include "WorkOrder.h"

#include <functional>

#include <QDir>
#include <QMutex>

#include "IException.h"

class QString;

namespace Isis {
  class Cube;
  class FileName;

  /**
   * @brief Add cubes to a project
   *
   * Asks the user for a list of cube file names and whether they should be copied into
   * the project. The cubes are then converted to external cube label files inside the project (and
   * cube files if the user said to copy the DN data). These files are then handed off to the
   * project.
   *
   * @author 2012-??-?? Steven Lambright and Stuart Sides
   *
   * @internal
   *   @history 2012-08-24 Steven Lambright and Stuart Sides - Removed GUI-thread slowdown of
   *                           image verification
   *   @history 2012-08-29 Steven Lambright - This work order should now work correctly even when
   *                           just 1 QtConcurrent thread is enabled.
   *   @history 2012-10-02 Steven Lambright - Image ID's are now guaranteed to be the same after
   *                           undo/redo. This is a beginning step for making work orders whose
   *                           images are freed from memory undo/redo correctly.
   *   @history 2012-10-29 Steven Lambright - Added a prompt to save the project if importing a lot
   *                           of images to a temporary project.
   *   @history 2017-04-05 Ian Humphrey and Makayla Shepherd - Renamed the following: execute() to
   *                           setupExeuction(), asyncRedo() to execute(), syncUndo() to
   *                           undoExecution(), postSyncRedo() to postExecution(), and
   *                           postSyncUndo() to postUndoExecution(). Added isSynchronous(). This is
   *                           related to the WorkOrder redesign. Fixes #4732.
   *   @history 2017-04-11 Ian Humphrey - Removed isSynchronous() and instead set inherited member
   *                           m_isSynchronous to false in constructor to indicate this is an
   *                           asynchronous work order. Updated documentation. References #4732.
   *   @history 2017-05-01 Ian Humphrey - Updated undoExecution() so that when undo, imported
   *                           images are removed from the project tree. Fixes #4597.
   *   @history 2017-07-06 Cole Neubauer - Added ability to have cube lists without full file path
   *                           Fixes #4956
   *   @history 2017-07-14 Cole Neubauer - Added ability successfully import after failing to import
   *                           a set or list of images Fixes #5015
   *   @history 2017-07-14 Cole Neubauer - Made import statement more descriptive when importing a
   *                           cube list Fixes #5042
   *   @history 2017-07-17 Makayla Shepherd - Added isExecutable(ProjectItem) to allow for importing
   *                           in the context menu. Fixes #4968.
   *   @history 2017-07-25 Cole Neubauer - Added project()->setClean call #4969
   *   @history 2017-07-26 Makayla Shepherd - Fixed a crash that occurs when a failed image import
   *                           is undone. Fixes #5043.
   *   @history 2017-07-28 Cole Neubauer - Added a pointer to the project item added by the work
   *                           order. This pointer is used in the Undo funtions #5064
   *   @history 2017-08-11 Cole Neubauer - Created a try catch around a previously unprotected error
   *                           to handle errors thrown in the workorder that halted execution.
   *                           Fixes #5026.
   *   @history 2017-11-02 Tyler Wilson - Added a null pointer check around ProjectItem *item pointer
   *                           in isExecutable to prevent potential seg faults.  References #4492.
   *
   *   @history 2017-11-13 Cole Neubauer - Fixed apsolute paths not being read correctly in cubelis
   *                           Fixes #4956
   *   @history 2017-09-26 Tracie Sucharski - In ::importConfirmedImages, pass in the cube pointer
   *                           to Image constructor rather than the future result.  Null out the
   *                           cube pointer after it is closed in the Image.  After output ecub is
   *                           created reopen created ecub as readOnly.  When closing cube, the
   *                           labels were being re-written because the cube was read/write. This
   *                           caused a segfault when imported large number of images because of a
   *                           label template file being opened too many times.  Uncommented error
   *                           checking in functor operator method. Fixes #4955.
   *   @history 2018-01-18 Tracie Sucharski - Add the targets and gui cameras to the project.
   *                           Fixes #5181.
   *   @history 2018-07-12 Summer Stapleton - Updated how adding the targets and the gui cameras to
   *                           the project is handled to address segfault. Instead of creating a 
   *                           camera and a target with every new image imported and adding them if
   *                           needed, they are now no longer created in the first place except 
   *                           when needed. Fixes #5460.
   *   @history 2018-07-19 Tracie Sucharski - Changed default button for importing DN data to "No". 
   *            
   */
  class ImportImagesWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      ImportImagesWorkOrder(Project *project);
      ImportImagesWorkOrder(const ImportImagesWorkOrder &other);
      ~ImportImagesWorkOrder();

      virtual ImportImagesWorkOrder *clone() const;

      virtual bool isExecutable(ProjectItem *item);
      virtual bool setupExecution();

      virtual void execute();

    protected:
      virtual void undoExecution();
      virtual void postExecution();
      virtual void postUndoExecution();

    private:
      ImportImagesWorkOrder &operator=(const ImportImagesWorkOrder &rhs);

      /**
       * This copies the given cube(s) into the project. This is designed to work with
       *   QtConcurrentMap.
       *
       * @author 2012-??-?? ???
       *
       * @internal
       */
      class OriginalFileToProjectCubeFunctor :
          public std::function<Cube *(const FileName &)> {
        public:
          OriginalFileToProjectCubeFunctor(QThread *guiThread,
                                           QDir destinationFolder, bool copyDnData);
          OriginalFileToProjectCubeFunctor(const OriginalFileToProjectCubeFunctor &other);
          ~OriginalFileToProjectCubeFunctor();

          Cube *operator()(const FileName &original);

          IException errors() const;

        private:
          //! Not implemented
          OriginalFileToProjectCubeFunctor &operator=(const OriginalFileToProjectCubeFunctor &rhs);

          QDir m_destinationFolder; //!< Directory where to import the images to.
          bool m_copyDnData; //!< Indicates whether the cube data will be copied to the project.
          QThread *m_guiThread; //!< Pointer to the GUI thread. Not used?

          QMutex m_errorsLock; //!< Mutex lock for appending errors and incrementing error count.
          QSharedPointer<IException> m_errors; //!< Stores any errors that occur during import.
          QSharedPointer<int> m_numErrors; //!< Number of errors that occur during import.
      };

    private:
      void importConfirmedImages(QStringList confirmedImages, bool copyDnData);

    private:
      ImageList *m_newImages; //!< List of images that are being imported in this work order.
      ImageList *m_list; //!< List of images that was succesfully imported into project.
      QString m_warning; //!< String of any errors/warnings that occurred during import.
  };
}
#endif // ImportImagesWorkOrder_H
