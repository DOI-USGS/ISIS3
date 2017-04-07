#ifndef ImageFileListViewWorkOrder_H
#define ImageFileListViewWorkOrder_H
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

#include "FileName.h"

class QString;

namespace Isis {
  class FileName;

  /**
   * View an image list in an image file list widget.
   *
   * @author 2012-??-?? ???
   *
   * @internal
   *   @history 2017-04-07 Makayla Shepherd - Removed syncUndo(), added isUndoable(), and renamed 
   *                           syncRedo() to execute() according to the WorkOrder redesign.
   */
  class ImageFileListViewWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      ImageFileListViewWorkOrder(Project *project);
      ImageFileListViewWorkOrder(const ImageFileListViewWorkOrder &other);
      ~ImageFileListViewWorkOrder();

      virtual ImageFileListViewWorkOrder *clone() const;

      bool isExecutable(ImageList *images);
      
      bool isUndoable() const;
      
      bool setupExecution();
      void execute();

    protected:
      

    private:
      ImageFileListViewWorkOrder &operator=(const ImageFileListViewWorkOrder &rhs);
  };
}

#endif // ImageFileListViewWorkOrder_H
