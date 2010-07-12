#ifndef ViewportBuffer_h
#define ViewportBuffer_h

/**
 * @file
 * $Date: 2010/06/03 18:36:14 $
 * $Revision: 1.14 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <vector>

#include <QRect>

#include "Cube.h"
#include "Brick.h"

// parent
#include <QObject>

template<class T > class QQueue;
class QRect;

namespace Isis {
  class CubeDataThread;
  class Brick;
}

namespace Qisis {
  class CubeViewport;
  class ViewportBufferAction;
  class ViewportBufferFill;
  class ViewportBufferStretch;
  class ViewportBufferTransform;

  /**
   * @brief Reads and stores visible DN values
   *
   * This class manages visible pixels in a CubeViewport. This class is responsible
   * for reading from the Cube only what is necessary and gives fast access to
   * visible DNs.
   *
   * @ingroup Visualization Tools
   *
   * @author 2009-03-26 Steven Lambright and Noah Hilt
   *
   * @internal
   *   @history 2009-04-21 Steven Lambright - Fixed problem with only half of the
   *                          side pixels being loaded in
   *   @history 2010-04-08 Steven Lambright and Eric Hyer - Added support for
   *                          using CubeDataThread for reads.
   *   @history 2010-05-11 Eric Hyer - Fixed issue with invalid bounding rects
   *                          in resizedViewport()
   *   @history 2010-05-24 Eric Hyer - Fixed bug in previous bug fix (checks
   *                          were in wrong order)
   *   @history 2010-07-12 Jeannie Walldren - Added exceptiona to help track
   *                          errors.
   */
  class ViewportBuffer : public QObject {
      Q_OBJECT

    public:
      ViewportBuffer(CubeViewport *viewport, Isis::CubeDataThread *cubeData,
                     int cubeId);
      virtual ~ViewportBuffer();

      const std::vector<double> &getLine(int line);

      void resizedViewport();
      void pan(int deltaX, int deltaY);

      void scaleChanged();

      void fillBuffer(QRect rect);
      void fillBuffer(QRect rect, const Isis::Brick *data);

      void emptyBuffer(bool force = false);

      QRect bufferXYRect();

      void setBand(int band);

      //! Return the band associated with this viewport buffer
      int getBand() {
        return p_band;
      }

      void enable(bool enabled);

      void addStretchAction();

      double currentProgress();
      double totalUnfilledArea();

      /**
       * Returns whether the buffer is enabled (reading data) or not.
       *
       *
       * @return bool
       */
      bool enabled() {
        return p_enabled;
      }
      bool working();

      bool hasEntireCube();

    public slots:
      void DataReady(void *requester, int cubeId, const Isis::Brick *brick);

    signals:
      /**
       * Ask the cube data thread for data
       *
       * @param cubeId
       * @param startSample
       * @param startLine
       * @param endSample
       * @param endLine
       * @param band
       * @param caller
       */
      void ReadCube(int cubeId, int startSample, int startLine,
                    int endSample, int endLine, int band, void *caller);

      //! Tell cube data thread we're done with a brick
      void DoneWithData(int, const Isis::Brick *);

    private:
      QRect getXYBoundingRect();
      QList<double> getSampLineBoundingRect();
      void updateBoundingRects();
      void doQueuedActions();
      void doTransformAction(ViewportBufferTransform *action);
      void doStretchAction(ViewportBufferStretch *action);
      void startFillAction(Qisis::ViewportBufferFill *action);

      ViewportBufferFill *createViewportBufferFill(QRect, bool);

      void requestCubeLine(ViewportBufferFill *fill);

      void resizeBuffer(unsigned int width, unsigned int height);
      void shiftBuffer(int deltaX, int deltaY);
      void reinitialize();
      bool actionsPreserveData();
      bool reinitializeActionExists();
      void enqueueAction(ViewportBufferAction *);

      CubeViewport *p_viewport;  //!< The CubeViewport which created this buffer
      int p_cubeId; //!< Id associated with the cube in this viewport buffer
      Isis::CubeDataThread *p_dataThread;  //!< manages cube io

      int p_band; //!< The band to read from

      bool p_enabled; //!< True if reading from cube (active)
      std::vector< std::vector<double> > p_buffer; //!< The buffer to hold cube dn values
      bool p_bufferInitialized; //!< True if the buffer has been initialized

      /**
       * This rect is in viewport pixels and represents the area that
       * this viewport buffer defines in the viewport.
       */
      QRect p_XYBoundingRect;
      QRect p_oldXYBoundingRect; //!< The previous bounding rect

      /**
       * This rect is in cube pixels and represents the area that
       * this viewport buffer defines in the viewport. To get a
       * particular corner use the sampLineRectPosition enumeration as
       * indices for a particular corner.
       */
      QList< double > p_sampLineBoundingRect;
      QList< double > p_oldSampLineBoundingRect; //!< Previous bounding rect
      int p_viewportHeight; //!< Current viewport height
      int p_oldViewportHeight; //!< Previous viewport height
      int p_vertScrollBarPos; //!< Current vertical scroll bar position
      int p_oldVertScrollBarPos; //!< Previous vertical scroll bar position
      bool p_initialStretchDone; //!< True if a stretch action has occurred
      double p_requestedFillArea; //!< Sum of the requested area to be filled

      /**
       * Enumeration for accessing sample/line bounding rectangles
       */
      enum sampLineRectPosition {
        rectLeft = 0, //!< QRect.left()
        rectTop,  //!< QRect.top()
        rectRight, //!< QRect.right()
        rectBottom //!< QRect.bottom()
      };

      /**
       * This is the set of actions we wish to perform on the buffer.
       * This is queued because we need to wait for other threads to
       * give us cube data before we progress through the queue.
       */
      QQueue< ViewportBufferAction * > * p_actions;
  };
}

#endif
