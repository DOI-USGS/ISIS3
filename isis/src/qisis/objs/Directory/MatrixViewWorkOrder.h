#ifndef MatrixViewWorkOrder_H
#define MatrixViewWorkOrder_H
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

namespace Isis {
  class CorrelationMatrix;

  /**
   * This work order will open a MatrixSceneWidget and display the correlation matrix. 
   *
   * @author 2014-07-14 Kimberly Oyama
   *
   * @internal
   *   @history 2014-07-14 Kimberly Oyama - Adapted from Mosaic2DViewWorkOrder.
   *   @history 2016-06-06 Makayla Shepherd - Updated documentation. Fixes #3993.
   *   @history 2017-04-10 Makayla Shepherd - Renamed syncRedo() to execute() and syncUndo() to 
   *                           undoExecution() according to the WorkOrder redesign.
   */
  class MatrixViewWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      MatrixViewWorkOrder(Project *project);
      MatrixViewWorkOrder(const MatrixViewWorkOrder &other);
      ~MatrixViewWorkOrder();

      virtual MatrixViewWorkOrder *clone() const;

      virtual bool isExecutable(CorrelationMatrix matrix);
      bool setupExecution();
      
      void execute();
      void undoExecution();

    protected:
      bool dependsOn(WorkOrder *other) const;
      ;

    private:
      MatrixViewWorkOrder &operator=(const MatrixViewWorkOrder &rhs);
  };
}
#endif

