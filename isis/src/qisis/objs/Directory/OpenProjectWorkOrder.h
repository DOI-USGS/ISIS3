#ifndef OpenProjectWorkOrder_H
#define OpenProjectWorkOrder_H
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

class QString;

namespace Isis {

  /**
   * This opens a project that's saved on disk.
   *
   * @author 2012-??-?? ???
   *
   * @internal
   *   @history 2014-09-05 Kim Oyama - Removed the connection for opening a project from the
   *                           command line.
   *   @history 2016-06-10 Ian Humphrey - Minor updates to documentation and coding standards.
   *                           Fixes #3952.
   *   @history 2017-03-28 Tracie Sucharski - Changed user prompt to indicating a directory is to
   *                           be selected, not a project file.
   *   @history 2017-04-06 Tracie Sucharski - Refactor for the new WorkOrder design, renaming
   *                           execute to setupExecution, and moving the actual work to the execute
   *                           method.
   *   @history 2017-07-12 Cole Neubauer - In setupExecution added functionallity to open a new
   *                           project while there is a project currently open. Fixes #4969
   */
  class OpenProjectWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      OpenProjectWorkOrder(Project *project);
      OpenProjectWorkOrder(const OpenProjectWorkOrder &other);
      ~OpenProjectWorkOrder();

      virtual OpenProjectWorkOrder *clone() const;

      bool setupExecution();
      void execute();

    signals:
      void openProjectFromCommandLine(QString);

    private:
      OpenProjectWorkOrder &operator=(const OpenProjectWorkOrder &rhs);

      bool m_startingState;
      QString m_projectName;
  };
}

#endif // OpenProjectWorkOrder_H
