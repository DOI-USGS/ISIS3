#ifndef ViewportBufferAction_h
#define ViewportBufferAction_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class ViewportBufferAction {
    public:
      ViewportBufferAction();
      virtual ~ViewportBufferAction();

      /**
       * This uniquely identifies which child is instantiated.
       */
      enum ActionType {
        none, //!< Parent was instantiated
        transform, //!< ViewportBufferTransform
        fill, //!< ViewportBufferFill
        stretch //!< ViewportBufferStretch
      };

      //! Returns the instantiated type
      virtual ActionType getActionType() {
        return none;
      }

      //! Returns true if this is an action that takes time and has begun
      bool started() {
        return p_started;
      };

      /**
       * Sets started
       *
       * @param started True if starting
       */
      void started(bool started) {
        p_started = started;
      };

      //! Cancels the process, used if reinitialize requested for example
      virtual void stop() {};

    private:
      /**
       * No copying these.
       *
       * @param other
       */
      ViewportBufferAction(const ViewportBufferAction &other);

      /**
       * No assigning these.
       *
       * @param other
       *
       * @return ViewportBufferAction&
       */
      ViewportBufferAction &operator=(const ViewportBufferAction &other);

    private:
      bool p_started; //!< True if this action has begun
  };
}

#endif
