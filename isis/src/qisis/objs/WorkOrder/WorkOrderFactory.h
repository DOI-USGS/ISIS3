#ifndef WorkOrderFactory_H
#define WorkOrderFactory_H
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

#include <QObject>

class QString;

namespace Isis {
  class Project;
  class WorkOrder;

  /**
   * @brief Instantiate work orders from QString versions of the class name
   *
   * This factory creates work orders. To create a work order, you need a Project and the name
   *   of the work order. Names of work orders are strings, such as "Isis::ImportImagesWorkOrder"
   *
   * @author 2012-08-22 Steven Lambright
   *
   * @internal
   *   @history 2012-10-19 Steven Lambright - Added tryType() to reduce the chances of programmer
   *                         mistakes and to reduce duplicate code.
   *   @history 2014-07-14 Kimberly Oyama - Added support for correlation matrix.
   *   @history 2015-08-14 Jeannie Backer - Added jigsaw work order.
   *   @history 2015-09-05 Ken Edmundson - Added support for new WorkOrders: SaveProjectWorkOrder,
   *                         SensorGetInforWorkOrder, and TargetGetInfoWorkOrder.
   *   @history 2016-06-09 Makayla Shepherd - Updated documentation. Fixes #3957.
   *   @history 2016-07-26 Tracie Sucharski - Added support for Importing shapes.
   *   @history 2016-11-07 Tracie Sucharski - Added the CubeDnViewWorkOrder,
   *                         SetActiveControlWorkOrder, SetActiveImageListWorkOrder.  Fixes #4485.
   *   @history 2017-05-04 Tracie Sucharski - Added BundleObservationViewWorkOrder.  Fixes #4840.
   *   @history 2017-08-11 Christopher Combs - Added ImportTemplateWorkOrder. Fixes #5086.
   *   @history 2017-11-02 Tyler Wilson - Added the OpenRecentProjectsWorkOrder. Fixes #4492.
   *   @history 2017-11-09 Tyler Wilson - Merged the functionality of OpenRecentProjectsWorkOrder
   *                              into OpenProjectWorkOrder to reduce unnecessary code duplication,
   *                              and removed references to OpenRecentProjectWorkOrder in this
   *                              class.  Fixes #5220.
   *   @history 2018-06-18 Adam Goins - Added ControlHealthMonitorWorkOrder to WorkOrderFactory.
   *                           Fixes #5435.
   *   @history 2018-07-07 Summer Stapleton - Removed importTemplateWorkOrder and replaced it with 
   *                           the new map and registration import work orders to reflect changes
   *                           in Project.cpp for handling these differently.
   */
  class WorkOrderFactory {
    public:
      static WorkOrder *create(Project *project, QString type);

    private:
      template<typename ClassType>
      static void tryType(QString type, Project *project, WorkOrder * &result) {
        if (ClassType::staticMetaObject.className() == type) {
          result = new ClassType(project);
        }
      }
      /**
       * This class cannot be instantiated. The constructor is not implemented.
       */
      WorkOrderFactory();
      /**
       * Since this class cannot be instantiated, it also cannot be destroyed. The destructor is
       * not implemented.
       */
      ~WorkOrderFactory();

      Q_DISABLE_COPY(WorkOrderFactory);
  };
}

#endif // WorkOrderFactory_H
