#ifndef ImageListActionWorkOrder_H
#define ImageListActionWorkOrder_H

#include "WorkOrder.h"

class QStringList;
class QXmlStreamWriter;

namespace Isis {
  /**
   * @brief Work orders that can be performed on an image list that modifies internal state
   *
   * This encapsulates the set of image list work orders that modifies internal state to the image
   *   list. This means these work orders typically modify the ImageDisplayProperties on the images.
   *
   * These work orders rely on the internal data to know the action to perform and what the expected
   *   state is. The internal data is stored like this:
   *
   * internalData = (ActionString,
   *                 UserInputValue (optional),
   *                 OriginalValue1 (optional) SPACE NewValue1 (optional),
   *                 OriginalValue2 (optional) SPACE NewValue2 (optional),
   *                 OriginalValue3 (optional) SPACE NewValue3 (optional),
   *                 OriginalValue4 (optional) SPACE NewValue4 (optional),
   *                 ...
   *                 )
   *
   * For example, if we're changing the colors of the images, the internal data would be:
   * internalData = (Change Color,
   *                 NewColor,
   *                 File1OriginalColor,
   *                 File2OriginalColor,
   *                 File3OriginalColor,
   *                 ...
   *                 )
   *
   * The original colors will let us undo this action. A second example (randomize colors):
   * internalData = (Random Color,
   *                 File1OriginalColor File1NewColor,
   *                 File2OriginalColor File2NewColor,
   *                 File3OriginalColor File3NewColor,
   *                 ...
   *                 )
   *
   * @see ImageList::supportedActions()
   *
   * @author 2012-08-28 Steven Lambright
   *
   * @internal
   *   @history 2012-09-12 Stuart Sides and Steven Lambright - Added temporary work-around to Z
   *                           order actions until we get around to implementing them properly.
   *                           They cannot undo, but they redo just fine right now.
   *   @history 2012-09-17 Steven Lambright - Added warning to show labels
   *   @history 2012-10-02 Stuart Sides and Steven Lambright - Removed ZoomFit Action
   *   @history 2016-06-08 Jesse Mapel - Added documentation.  Fixes #3995.
   */
  class ImageListActionWorkOrder : public WorkOrder {
    Q_OBJECT

    public:
      /**
       * Type of action to be performed by the work order
       */
      enum Action {
        UnknownAction = 0,  //!< Unkown action
        ChangeTransparency, //!< Change the alpha values of the image list
        ChangeColor,        //!< Change the color values of the image list
        RandomizeColor,     //!< Set each image in the list to a random color
        ToggleShowLabel,    //!< Show or hide each image's display name
        ToggleShowFilled,   //!< Show or hide each image's fill area
        ToggleShowCubeData, //!< Show or hide each image's DNs
        ToggleShowOutline   //!< Show or hide each image's outline
      };

      ImageListActionWorkOrder(Project *project);
      ImageListActionWorkOrder(Action action, Project *project);
      ImageListActionWorkOrder(const ImageListActionWorkOrder &other);
      ~ImageListActionWorkOrder();

      ImageListActionWorkOrder *clone() const;

      bool isExecutable(ImageList *images);
      void setData(ImageList *images);

      bool execute();
      void syncRedo();
      void syncUndo();

      static QString qualifyString(QString unqualifiedString, ImageList *);
      static QString toString(Action);
      static Action fromActionString(QString);

    signals:
      void bringToFront();

    private:
      ImageListActionWorkOrder &operator=(const ImageListActionWorkOrder &rhs);
  };
}

#endif
