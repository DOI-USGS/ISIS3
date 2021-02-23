#ifndef Quaternion_h
#define Quaternion_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

namespace Isis {
  /**
   * @brief Provide operations for quaternion arithmetic
   *
   * This class provides a wrapper for existing Naif quaternion functions and
   * also includes other operators and methods useful for working with
   * quaternions.
   *
   * @ingroup Rotations
   *
   * @author 2005-12-07 Debbie A. Cook
   *
   * @internal
   *   @history ????-??-?? Steven Lambright Fixed documentation
   *   @history 2015-07-21 Kristin Berry - Added NaifStatus::CheckErrors() to see if any NAIF errors
   *            were signaled. References #2248.
   */

  class Quaternion {
    public:
      // constructors
      Quaternion();
      Quaternion(const std::vector<double> matrix);

      // destructor
      ~Quaternion();

      //Methods

      std::vector<double> ToMatrix();

      std::vector<double> ToAngles(int axis3, int axis2, int axis1);

      void Set(std::vector<double>);

      //! Return the quaternion as a vector
      std::vector<double> GetQuaternion() const {
        return p_quaternion;
      }
      ;
      Quaternion &operator=(const Quaternion &quat);

      Quaternion &operator*=(const Quaternion &quat);

      Quaternion operator*(const Quaternion &quat) const;

      Quaternion operator*(const double &scalar);

      /**
       * Return a member of a quaternion.  For example,
       * @code
       *  Quaternion q();
       * ...
       *  double angle = q[0];
       * @endcode
       *
       * @param[in] index (const int &) quaternion member to return
       *
       * @return (double&) value pointed to by iter
       *
       */
      double &operator[](int index) {
        return p_quaternion.at(index);
      };



      std::vector<double> Qxv(const std::vector<double> &vin);

      Quaternion Conjugate();




    private:
      std::vector<double> p_quaternion;     //!< Quaternion
      void Polish(Quaternion &quat);
  };
};

#endif

