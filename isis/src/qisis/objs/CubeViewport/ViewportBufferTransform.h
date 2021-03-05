#ifndef ViewportBufferTransform_h
#define ViewportBufferTransform_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ViewportBufferAction.h"


namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class ViewportBufferTransform : public ViewportBufferAction {
    public:
      ViewportBufferTransform();

      //! Returns the instance type
      virtual ActionType getActionType() {
        return transform;
      }
      void setTranslation(int x, int y);


      /**
       * Gets the amount the buffer should be translated in X
       *
       * @return int
       */
      int getXTranslation() {
        return p_xTranslation;
      }


      /**
       * Gets the amount the buffer should be translated in Y
       *
       * @return int
       */
      int getYTranslation() {
        return p_yTranslation;
      }

      void setResize(int width, int height);

      /**
       * Returns the new buffer width
       *
       * @return const int&
       */
      const int &getBufferWidth() {
        return p_newBufferWidth;
      }

      /**
       * Returns the new buffer height
       *
       * @return const int&
       */
      const int &getBufferHeight() {
        return p_newBufferHeight;
      }

      /**
       * Returns true if the resize should happen before the
       * translation
       *
       * @return bool
       */
      bool resizeFirst() {
        return p_resizeFirst;
      }

      /**
       * Sets whether the resize should happen before the translation
       *
       * @param resizeFirst
       */
      void resizeFirst(bool resizeFirst) {
        p_resizeFirst = resizeFirst;
      }


    private:
      int p_xTranslation; //!< How far to translate in X
      int p_yTranslation; //!< How far to translate in Y
      int p_newBufferWidth; //!< New width
      int p_newBufferHeight; //!< New height
      bool p_resizeFirst; //!< Do the resize before the translation?
  };
}
#endif
