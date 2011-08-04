
#include "ViewportBufferTransform.h"

#include <QRect>

namespace Isis {
  /**
   * Constructor
   */
  ViewportBufferTransform::ViewportBufferTransform() {
    p_xTranslation = 0;
    p_yTranslation = 0;
    p_newBufferWidth = -1;
    p_newBufferHeight = -1;
    p_resizeFirst = false;
  }

  /**
   * Sets the translation amount in x and y
   *
   * @param x
   * @param y
   */
  void ViewportBufferTransform::setTranslation(int x, int y) {
    p_xTranslation = x;
    p_yTranslation = y;
  }


  /**
   * Sets the size the buffer should be resized to
   *
   * @param width
   * @param height
   */
  void ViewportBufferTransform::setResize(int width, int height) {
    p_newBufferWidth = width;
    p_newBufferHeight = height;
  }
}
