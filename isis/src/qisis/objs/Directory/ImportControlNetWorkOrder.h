#ifndef ImportControlNetWorkOrder_H
#define ImportControlNetWorkOrder_H
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

#include <QDir>
#include <QFutureWatcher>

namespace Isis {
  class Control;
  class FileName;
  class Progress;
  class Project;

  /** 
   * @brief Add control networks to a project 
   *  
   * Asks the user for a list of control nets and copies them into the project.
   * 
   * @author 2012-06-05 Ken Edmundson and Tracie Sucharski
   *
   * @internal 
   *   @history 2012-09-11 Tracie Sucharski and Steven Lambright - Added asynchronous functionality
   *                           for redo.
   *   @history 2017-04-04 Makayla Shepherd - Combined syncRedo and asyncRedo into execute, changed
   *                           execute to setupExecution, and renamed postSyncRedo to postExecution
   *                           and undoSyncRedo to undoExecution. This was done to match the 
   *                           WorkOrder redesign. Fixes #4716.
   *   @history 2017-05-01 Ian Humphrey - Updated undoExecution() so when undone, the imported
   *                           cnet(s) are removed from the project tree. Fixes #4597. 
   */
  class ImportControlNetWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      ImportControlNetWorkOrder(Project *project);
      ImportControlNetWorkOrder(const ImportControlNetWorkOrder &other);
      ~ImportControlNetWorkOrder();

      virtual ImportControlNetWorkOrder *clone() const;
      
      bool setupExecution();
      void execute();

    protected:
      void undoExecution();
      void postExecution();

    private slots:
      void cnetReady(int ready);

    private:
      ImportControlNetWorkOrder &operator=(const ImportControlNetWorkOrder &rhs);
//    Control *createControls(const QString &cnetFilename);

      /**
       *
       * @author ????-??-?? ???
       *
       * @internal
       */
      class CreateControlsFunctor : public std::unary_function<
          const QPair<FileName, Progress *> &, Control *> {
        public:
          CreateControlsFunctor(Project *project, QDir destinationFolder);
          Control *operator()(const QPair<FileName, Progress *> &cnetFilename);

        private:
          Project *m_project;
          QDir m_destinationFolder;
      };

    private:
      QFutureWatcher<Control *> *m_watcher;
      QList<Progress *> m_readProgresses;
  };
}
#endif // ImportControlNetWorkOrder_H
