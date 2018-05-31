#ifndef ImportShapesWorkOrder_H
#define ImportShapesWorkOrder_H
/**
 * @file
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
  class ShapeList;

  /**
   * @brief Add shape model cubes to a project
   *
   * Asks the user for a list of cube file names. The cubes are then converted to external cube
   * label files inside the project. These files are then handed off to the project.
   *
   * @author 2016-07-06 Tracie Sucharski
   *
   * @internal
   *   @history 2017-04-12 JP Bonn - Updated to new workorder design.
   *   @history 2017-05-01 Ian Humphrey - Updated undoExecution() so that when undone, imported
   *                           shapes are removed from the project tree. Fixes #4597.
   *   @history 2017-07-13 Makayla Shepherd - Added isExecutable(ProjectItem) to allow for importing
   *                           in the context menu. Fixes #4968.
   *   @history 2017-07-25 Cole Neubauer - Added project()->setClean call #4969
   *   @history 2017-07-26 Makayla Shepherd - Fixed a crash that occurs when a failed image import
   *                           is undone. Fixes #5043.
   *   @history 2017-11-02 Tyler Wilson - Added a  null pointer check on the ProjectItem *item
   *                           pointer in isExecutable to prevent potential seg faults.
   *                           References #4492.
   *   @history 2018-04-19 Tracie Sucharski - Fixed bug when importing shapes without DN data. Ecub
   *                           labels were not complete due to a resulting ecub not being closed
   *                           properly in thread.  The resulting ecub needs to be re-opened as
   *                           readOnly to prevent this problem.  Fixes #5274.
   */
  class ImportShapesWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      ImportShapesWorkOrder(Project *project);
      ImportShapesWorkOrder(const ImportShapesWorkOrder &other);
      ~ImportShapesWorkOrder();

      virtual ImportShapesWorkOrder *clone() const;

      virtual bool isExecutable(ProjectItem *item);
      bool setupExecution();

      void execute();
      void postExecution();
      void undoExecution();
      void postUndoExecution();

    private:
      ImportShapesWorkOrder &operator=(const ImportShapesWorkOrder &rhs);

      /**
       * This copies the given shape model cube(s) into the project. This is designed to work with
       *   QtConcurrentMap.  TODO::  TLS 2016-07-13  If large DEM, do not allow DN data to be
       *   copied??
       *
       * @author 2016-07-06 Tracie Sucharski
       *
       * @internal
       */
      class OriginalFileToProjectCubeFunctor :
          public std::unary_function<const FileName &, Cube *> {
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

          QDir m_destinationFolder; //!< Directory where the DN data is going to be stored
          bool m_copyDnData; //!< Stores if the user wants to copy the DN data or not
          QThread *m_guiThread; //!< The GUI thread

          QMutex m_errorsLock; //!< Mutex lock for errors
          QSharedPointer<IException> m_errors; //!< Shared pointers for errors
          QSharedPointer<int> m_numErrors; //!< Number of errors that have occured
      };

    private:
      void importConfirmedShapes(QStringList confirmedShapes, bool copyDnData);

    private:
      ShapeList *m_newShapes; //!< List of shapes
      ShapeList *m_list; //!< List of shapes this workorder added to the project
      QString m_warning; //!< QString of warning text
  };
}
#endif // ImportShapesWorkOrder_H
