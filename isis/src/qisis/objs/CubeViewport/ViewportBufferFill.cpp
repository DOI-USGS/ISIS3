/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ViewportBufferFill.h"

#include <algorithm>
#include <iostream>

#include <QRect>
#include <QPoint>

using namespace std;

namespace Isis {
  /**
   * Constructor.
   *
   * @param rect
   * @param xCoef
   * @param xScale
   * @param yCoef
   * @param yScale
   * @param topLeftPixel
   */
  ViewportBufferFill::ViewportBufferFill(const QRect &rect,
      const int &xCoef, const double &xScale, const int &yCoef,
      const double &yScale, const QPoint &topLeftPixel)
      : ViewportBufferAction() {
    p_rect = NULL;
    p_topLeftPixel = NULL;

    p_rect = new QRect(rect);
    p_topLeftPixel = new QPoint(topLeftPixel);
    p_requestPosition = rect.top();
    p_readPosition = rect.top();
    p_xCoef = xCoef;
    p_xScale = xScale;
    p_yCoef = yCoef;
    p_yScale = yScale;
  }

  /**
   * Destructor
   */
  ViewportBufferFill::~ViewportBufferFill() {
    if(p_rect) {
      delete p_rect;
      p_rect = NULL;
    }

    if(p_topLeftPixel) {
      delete p_topLeftPixel;
      p_topLeftPixel = NULL;
    }
  }

  /**
   * Returns the top of the X/Y bounding rect for this fill
   *
   * @return int
   */
  int ViewportBufferFill::getTopmostPixelPosition() {
    return p_topLeftPixel->y();
  }


  /**
   * Returns the left of the X/Y bounding rect for this fill
   *
   * @return int
   */
  int ViewportBufferFill::getLeftmostPixelPosition() {
    return p_topLeftPixel->x();
  }


  /**
   * Returns true if read position is past the end of the fill
   *
   * @return bool
   */
  bool ViewportBufferFill::doneReading() {
    return p_readPosition >= (unsigned)(p_rect->top() + p_rect->height());
  }


  /**
   * Returns true if request position is past the end of the fill
   *
   * @return bool
   */
  bool ViewportBufferFill::shouldRequestMore() {
    return p_requestPosition < (unsigned)(p_rect->top() + p_rect->height());
  }


  /**
   * Cancels the current operation. Stops filling ASAP.
   */
  void ViewportBufferFill::stop() {
    p_rect->setBottom(p_requestPosition - 1);
  }


  /**
   * Returns true if it is recommended to paint the fill area so
   * far.
   *
   * @param linesToPaint
   *
   * @return bool
   */
  bool ViewportBufferFill::shouldPaint(int &linesToPaint) {
    if(p_rect->height() < STEPSIZE)
      linesToPaint = p_rect->height();
    else
      linesToPaint = STEPSIZE;

    return p_readPosition % STEPSIZE == 0 || doneReading();
  }
}
