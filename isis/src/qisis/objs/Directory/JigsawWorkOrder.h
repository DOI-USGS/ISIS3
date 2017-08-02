#ifndef JigsawWorkOrder_H
#define JigsawWorkOrder_H
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

template <class T>
class QSharedPointer;

namespace Isis {
  class BundleSettings;
  typedef QSharedPointer<BundleSettings> BundleSettingsQsp;
  /**
   * @brief This work order allows the user to run a bundle adjustment (jigsaw).
   * This workorder is synchronous and undoable.
   *
   * @author 2014-04-03 Ken Edmundson
   *
   * @internal
   *   @history 2014-06-04 Jeannie Backer - Fixed JigsawWorkOrder error.
   *   @history 2015-09-05 Ken Edmundson - Added preliminary target body functionality to IPCE.
   *   @history 2016-06-06 Makayla Shepherd - Updated documentation. Fixes #3993.
   *   @history 2017-04-17 Ian Humphrey - Updated documentation and methods for accommodating the
   *                           changes to work order. References #4748.
   *   @history 2017-04-17 Ian Humphrey - Added what's this and tool tip text to this work order
   *                           so the user knows that this work order is disabled until an
   *                           active cnet and image list have been set. Fixes #4749.
   *   @history 2017-04-25 Ian Humphrey - Modified tool tip text. Fixes #4819.
   *   @history 2017-07-25 Cole Neubauer - Added project()->setClean call #4969
   *   @history 2017-07-25 Cole Neubauer - Moved project()->setClean call to JigsawDialog because
   *                            the workorder does not actually execute the bundle adjustment #4960
   */
  class JigsawWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      JigsawWorkOrder(Project *project);
      JigsawWorkOrder(const JigsawWorkOrder &other);
      ~JigsawWorkOrder();

      virtual JigsawWorkOrder *clone() const;

      virtual bool isExecutable();
      virtual bool setupExecution();
      virtual void execute();

    protected:
      bool dependsOn(WorkOrder *other) const;

    private:
      JigsawWorkOrder &operator=(const JigsawWorkOrder &rhs);
      BundleSettingsQsp m_bundleSettings; /**< BundleSettings shared betweeen setup and execute. */
  };
}
#endif
