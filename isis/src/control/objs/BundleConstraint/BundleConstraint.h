#ifndef BundleConstraint_h
#define BundleConstraint_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

namespace Isis {

  /**
   * @brief Base class for all BundleConstraint utility child classes.
   *
   * This abstract class serves as the parent for all child classes implementing constraints in the
   * bundle adjustment.
   *
   * @ingroup ControlNetworks
   *
   * @author 2017-03-03 Ken Edmundson
   *
   * @internal
   *   @history 2017-03-03 Ken Edmundson - Original version.
   */
  class BundleConstraint {

    public:
      BundleConstraint();

      virtual ~BundleConstraint();

    private:
  };
};

#endif
