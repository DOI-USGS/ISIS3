/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "NumericalApproximation.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include "SpecialPixel.h"
#include "IException.h"
#include "IString.h"

using namespace std;
namespace Isis {
  NumericalApproximation::FunctorList NumericalApproximation::p_interpFunctors;

  /**
   * Default constructor creates NumericalApproximation object.
   * Sets the NumericalApproximation::InterpType to the
   * enumerated value of @a itype. Default is @a CubicNatural.
   *
   * @param itype NumericalApproximation::InterpType enum value to
   *              be assigned to this object (Default:
   *              @a CubicNatural)
   * @throws Isis::IException::Programmer "Unable to construct
   *             NumericalApproximation object"
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class. Original name DataInterp().
   *   @history 2008-11-05 Jeannie Walldren - Changed name and
   *            modified to allow for interpolation types other than
   *            those supported by GSL.
   */
  NumericalApproximation::NumericalApproximation(const NumericalApproximation::InterpType &itype) {
    // Validate parameter and initialize class variables
    try {
      Init(itype);
      p_dataValidated = false;
    }
    catch(IException &e) {  // catch exception from Init()
      throw IException(e, e.errorType(),
                       "NumericalApproximation() - Unable to construct NumericalApproximation object",
                       _FILEINFO_);
    }
  }

  /**
   * @brief Constructs NumericalApproximation object, sets
   *        interpolation type, populates the data set with
   *        pointer values.
   *
   * This constructor verifies the
   * NumericalApproximation::InterpType passed in as a
   * parameter.  If it passes, it assigns this type to the object
   * and @a x and @a y are added to the data set and validated. If
   * the interpolation type is @a CubicClamped, first derivatives
   * of the endpoints are required before Evaluate() may be
   * called. If no value for itype is given, the default is
   * @a CubicNatural.
   *
   * @param n     Number of data points to be used.
   * @param x     Array of domain values to add to data set
   * @param y     Array of range values corresponding to each
   *              domain value in @a x
   * @param itype NumericalApproximation::InterpType enum
   *              value to be assigned to this object (Default:
   *              @a CubicNatural)
   * @throws Isis::IException::Programmer "Unable to construct
   *             NumericalApproximation object"
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class. Original name DataInterp().
   *   @history 2008-11-05 Jeannie Walldren - Changed name and
   *            modified to allow for interpolation types other than
   *            those supported by GSL.
   *   @history 2008-12-18 Jeannie Walldren - Added address
   *            operator (&) to input variables of type
   *            NumericalApproximation::InterpType.
   */
  NumericalApproximation::NumericalApproximation(unsigned int n, double *x, double *y,
      const NumericalApproximation::InterpType &itype) {
    try {
      Init(itype);
      AddData(n, x, y);
      ValidateDataSet();
    }
    catch(IException &e) { // catch exception from Init(), AddData(), ValidateDataSet()
      throw IException(e, e.errorType(),
                       "NumericalApproximation() - Unable to construct object using the given arrays, size and interpolation type",
                       _FILEINFO_);
    }
  }

  /**
   * @brief Constructs NumericalApproximation object, sets
   *        interpolation type, populates the data set with vector
   *        values.
   *
   * This constructor verifies the interpolation type and checks
   * that the @a x and @a y vectors have the same number of
   * elements. If so, the object is assigned the
   * NumericalApproximation::InterpType passed in as a
   * parameter and @a x and @a y are added
   * to the data set and validated.  If the interpolation type is
   * @a CubicClamped, first derivatives of the
   * endpoints are required before evaluation is allowed. If no
   * value for itype is given, the default
   * is @a CubicNatural.
   *
   * @param x     Vector of domain values to add to data set
   * @param y     Vector of range values corresponding to each
   *              domain value in @a x
   * @param itype NumericalApproximation::InterpType enum
   *              value to be assigned to this object (Default:
   *              @a CubicNatural)
   * @throws Isis::IException::Programmer "Unable to construct
   *             NumericalApproximation object"
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   *   @history 2008-12-18 Jeannie Walldren - Added address
   *            operator (&) to input variables of type vector and
   *            NumericalApproximation::InterpType.
   */
  NumericalApproximation::NumericalApproximation(const vector <double> &x, const vector <double> &y,
      const NumericalApproximation::InterpType &itype) {
    try {
      Init(itype);
      AddData(x, y); // size of x = size of y validated in this method
      ValidateDataSet();
    }
    catch(IException &e) { // catch exception from Init(), AddData(), ValidateDataSet()
      throw IException(e, e.errorType(),
                       "NumericalApproximation() - Unable to construct an object using the given vectors and interpolation type",
                       _FILEINFO_);
    }
  }

  /**
   * @brief Copy constructor creates NumericalApproximation object
   *        and sets the copies the class variable values and
   *        state of input NumericalApproximation object to the
   *        new object.
   *
   * Copying of class objects must be handled explicitly in order
   * for it to be completed properly.  The data, interpolator type
   * and other class variables are copied from the supplied
   * object.
   *
   * @param oldObject NumericalApproximation object to gleen data
   *          from for creation of this object
   * @throws Isis::IException::Programmer "Unable to construct
   *             NumericalApproximation object"
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version
   *            created in DataInterp class. Original name
   *            DataInterp().
   *   @history 2008-11-05 Jeannie Walldren - Changed name and
   *            modified to allow for interpolation types other than
   *            those supported by GSL.
   *   @history 2009-06-10 Jeannie Walldren - Set new variable,
   *            p_fprimeOfx.
   */
  NumericalApproximation::NumericalApproximation(const NumericalApproximation &oldObject) {
    try {
      // initialize new object and set type to that of old object
      Init(oldObject.p_itype);
      // fill data set of new object
      p_x = oldObject.p_x;
      p_y = oldObject.p_y;
      // if this data was previously validated, no need to do this again
      p_dataValidated = oldObject.p_dataValidated;
      // copy values for interpolation-specific variables
      p_clampedComputed = oldObject.p_clampedComputed;
      p_clampedEndptsSet = oldObject.p_clampedEndptsSet;
      p_clampedSecondDerivs = oldObject.p_clampedSecondDerivs;
      p_clampedDerivFirstPt = oldObject.p_clampedDerivFirstPt;
      p_clampedDerivLastPt = oldObject.p_clampedDerivLastPt;
      p_polyNevError = oldObject.p_polyNevError;
      p_fprimeOfx = oldObject.p_fprimeOfx;
    }
    catch(IException &e) { // catch exception from Init()
      throw IException(e,
                       e.errorType(),
                       "NumericalApproximation() - Unable to copy the given object",
                       _FILEINFO_);
    }
  }

  /**
   * @brief NumericalApproximation assigmment operator sets this
   *        object "equal to" another
   *
   * Assigning one object to the other requires a deallocation of
   * the current state of this object and reinitialization of data
   * from the right hand object. This may be a bit costly if the
   * number of values is large.
   *
   * @param oldObject NumericalApproximation object to copy data
   *          from for initialization
   * @throws Isis::IException::Programmer "Unable to copy
   *             NumericalApproximation object"
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class.
   *   @history 2008-11-05 Jeannie Walldren - Changed name and
   *            modified to allow for interpolation types other
   *            than those supported by GSL.
   *   @history 2009-06-10 Jeannie Walldren - Set new variable,
   *            p_fprimeOfx.
   *   @history 2010-06-25 Eric Hyer - Returns a NumericalApproximation now
   *            instead of a NumericalApproximation constructor
   */
  NumericalApproximation &NumericalApproximation::operator=(const NumericalApproximation &oldObject) {
    try {
      if(&oldObject != this) {
        if(GslInterpType(p_itype)) GslDeallocation();
        SetInterpType(oldObject.p_itype);
        // set class variables
        p_x = oldObject.p_x;
        p_y = oldObject.p_y;
        p_dataValidated = oldObject.p_dataValidated;
        p_clampedSecondDerivs = oldObject.p_clampedSecondDerivs;
        p_clampedDerivFirstPt = oldObject.p_clampedDerivFirstPt;
        p_clampedDerivLastPt = oldObject.p_clampedDerivLastPt;
        p_polyNevError = oldObject.p_polyNevError;
        p_fprimeOfx = oldObject.p_fprimeOfx;
      }
      return (*this);
    }
    catch(IException &e) { // catch exception from SetInterpType()
      throw IException(e,
                       e.errorType(),
                       "operator=() - Unable to copy the given object",
                       _FILEINFO_);
    }

  }

  /**
   * @brief Destructor deallocates memory being used
   *
   * Explicit operations are required to free resources of the
   * interpolation
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class. Original name ~DataInterp().
   *   @history 2008-11-05 Jeannie Walldren - Changed name.
   */
  NumericalApproximation::~NumericalApproximation() {
    if(GslInterpType(p_itype)) GslDeallocation();
  }

  /**
   * @brief Get name of interpolating function assigned to object.
   *
   * This method returns the name of the interpolation type
   * that is currently assigned to this object as a
   * @b std::string. This may be called without adding any
   * points. If called before computation, the result reflects the
   * name of the function chosen at instantiation.
   *
   * @return @b std::string Name of the interpolation function
   *         used/to be used.
   * @throws Isis::IException::Programmer "Unable to retrieve
   *                           numerical approximation name",
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class.
   *   @history 2008-11-05 Jeannie Walldren - Modified to allow for
   *            interpolation types other than those supported by
   *            GSL.
   *   @history 2009-06-10 Jeannie Walldren - Added CubicHermite
   *            name.
   */
  string NumericalApproximation::Name() const {
    if(p_itype == NumericalApproximation::CubicNeighborhood) {
      return "cspline-neighborhood";
    }
    else if(p_itype == NumericalApproximation::CubicClamped) {
      return "cspline-clamped";
    }
    else if(p_itype == NumericalApproximation::PolynomialNeville) {
      return "polynomial-Neville's";
    }
    else if(p_itype == NumericalApproximation::CubicHermite) {
      return "cspline-Hermite";
    }
    try {
      string name = (string(GslFunctor(p_itype)->name));
      if(p_itype == NumericalApproximation::CubicNatural) {
        return name + "-natural";
      }
      else return name;
    }
    catch(IException &e) { // catch exception from GslFunctor()
      throw IException(e,
                       e.errorType(),
                       "Name() - GSL interpolation type not found",
                       _FILEINFO_);
    }
  }

  /**
   * @brief Minimum number of points required by interpolating
   *        function
   *
   * This method returns the minimum number of points that are
   * required by the interpolating function in order for it to be
   * applied to a data set.  It returns the number of the of the
   * currently selected/active interpolation function.
   *
   * @return @b int Minimum number of data points required for the
   *         interpolation function
   * @throws Isis::IException::Programmer "Unable to calculate
   *             minimum points."
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class.
   *   @history 2008-11-05 Jeannie Walldren - Modified to allow for
   *            interpolation types other than those supported by
   *            GSL.
   *   @history 2009-06-10 Jeannie Walldren - Added min points for
   *            CubicHermite.
   *
   */
  int NumericalApproximation::MinPoints() {
    if(p_itype == NumericalApproximation::CubicNeighborhood) {
      return 4;
    }
    else if(p_itype == NumericalApproximation::CubicClamped) {
      return 3;
    }
    else if(p_itype == NumericalApproximation::PolynomialNeville) {
      return 3;
    }
    else if(p_itype == NumericalApproximation::CubicHermite) {
      return 2;
    }
    try {
      return (GslFunctor(p_itype)->min_size);
    }
    catch(IException &e) { // catch exception from GslFunctor()
      throw IException(e,
                       e.errorType(),
                       "MinPoints() - GSL interpolation not found",
                       _FILEINFO_);
    }
  }

  /**
   * @brief Minimum number of points required by interpolating
   *        function
   *
   * This method returns the minimum number of points that are
   * required by the interpolating function in order for it to be
   * applied to a data set.  It returns the minimum number for the
   * specifed interpolation function as specified by the caller.
   *
   * @param  itype Type of interpolation function for which to
   *               return minimum points
   * @return @b int Minimum number of data points required for the
   *         interpolation function
   *
   * @throws Isis::IException::Programmer "Invalid argument.
   *             Unknown interpolation type"
   * @throws Isis::IException::Programmer "Unable to calculate
   *             minimum points"
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class.
   *   @history 2008-11-05 Jeannie Walldren - Modified to allow for
   *            interpolation types other than those supported by
   *            GSL.
   *   @history 2009-06-10 Jeannie Walldren - Added min points for
   *            CubicHermite.
   */
  int NumericalApproximation::MinPoints(NumericalApproximation::InterpType itype) {
    //Validates the parameter
    if(GslInterpType(itype)) {
      try {
        //GslFunctor() Validates parameter for Gsl interp types
        NumericalApproximation nam(itype);
        return (nam.GslFunctor(itype)->min_size);
      }
      catch(IException &e) { // catch exception from GslFunctor()
        throw IException(e,
                         e.errorType(),
                         "MinPoints() - GSL interpolation type not found",
                         _FILEINFO_);
      }
    }
    else if(itype == NumericalApproximation::CubicNeighborhood) {
      return 4;
    }
    else if(itype == NumericalApproximation::CubicClamped) {
      return 3;
    }
    else if(itype == NumericalApproximation::PolynomialNeville) {
      return 3;
    }
    else if(itype == NumericalApproximation::CubicHermite) {
      return 2;
    }
    throw IException(IException::Programmer,
                     "MinPoints() - Invalid argument. Unknown interpolation type: "
                     + IString(NumericalApproximation::InterpType(itype)),
                     _FILEINFO_);
  }

  /**
   * @brief Add a datapoint to the set
   *
   * This method allows the user to add a point to the set of data
   * in preparation for interpolation over an interval.
   *
   * @b Note: All data sets must have unique @a x values. If the
   * interpolation type is not @a PolynomialNeville,
   * @a x values must be sorted in ascending order.  If
   * @a CubicNatPeriodic interpolation type is used, the first and
   * last points added to the data set
   * must have the same @a y value.
   *
   * @param x     Domain value to add to data set
   * @param y     Range value corresponding to domain value
   *              @a x
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class.
   *   @history 2008-11-05 Jeannie Walldren - Modified to reset
   *            class variables for interpolation types other than
   *            those supported by GSL.
   */
  void NumericalApproximation::AddData(const double x, const double y) {
    p_x.push_back(x);
    p_y.push_back(y);
    p_clampedComputed = false;
    p_clampedEndptsSet = false;
    p_dataValidated = false;
    p_clampedSecondDerivs.clear();
    p_clampedDerivFirstPt = 0;
    p_clampedDerivLastPt = 0;
    p_polyNevError.clear();
    p_interp = 0;
    p_acc = 0;
    return;
  }

  /**
   * @brief Add set of points to the data set using pointer arrays
   *
   * This method allows the user to add a set of points to the set
   * of data in preparation for interpolation over an interval.
   * This method does not overwrite previously added data. Rather,
   * it adds the new data to the end of the existing data set.
   *
   * @b Note: Behavior is undefined if the domain is not
   * sorted in ascending or descending order.
   *
   * @param n     Number of data points to be used.
   * @param x     Array of domain values to add to data set
   * @param y     Array of range values corresponding to each
   *              domain value in @a x
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  void NumericalApproximation::AddData(unsigned int n, double *x, double *y) {
    for(unsigned int i = 0; i < n; i++) {
      p_x.push_back(x[i]);
      p_y.push_back(y[i]);
    }
    p_clampedComputed = false;
    p_clampedEndptsSet = false;
    p_dataValidated = false;
    p_clampedSecondDerivs.clear();
    p_clampedDerivFirstPt = 0;
    p_clampedDerivLastPt = 0;
    p_polyNevError.clear();
    p_interp = 0;
    p_acc = 0;
    return;
  }

  /**
   * @brief Add set of points to the data set using vectors
   *
   * This method allows the user to add a set of points to the set
   * of data in preparation for interpolation over an interval.
   * This method does not overwrite previously added data. Rather,
   * it adds the new data to the end of the existing data set.
   *
   * @b Note: Behavior is undefined if the domain is not
   * sorted in ascending or descending order.
   *
   * @param x     Vector of domain values to add to data set
   * @param y     Vector of range values corresponding to each
   *              domain value in @a x
   * @throws Isis::IException::Programmer "Invalid arguments. The
   *                    number of x-values does not equal the
   *                    number of y-values."
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   *   @history 2008-12-18 Jeannie Walldren - Added address
   *            operator (&) to input variables of type vector.
   */
  void NumericalApproximation::AddData(const vector <double> &x,
                                       const vector <double> &y)
                                        {
    int n = x.size();
    int m = y.size();
    if(n != m) {
      ReportException(IException::Programmer, "AddData()",
                      "Invalid arguments. The sizes of the input vectors do "
                      "not match",
                      _FILEINFO_);
    }

    // Avoid push_back if at all possible. These calls were consuming 10% of
    //   cam2map's run time on a line scan camera.
    if(!p_x.empty() || !p_y.empty()) {
      for(int i = 0; i < n; i++) {
        p_x.push_back(x[i]);
        p_y.push_back(y[i]);
      }
    }
    else {
      p_x = x;
      p_y = y;
    }

    p_clampedComputed = false;
    p_clampedEndptsSet = false;
    p_dataValidated = false;
    p_clampedSecondDerivs.clear();
    p_clampedDerivFirstPt = 0;
    p_clampedDerivLastPt = 0;
    p_polyNevError.clear();
    p_interp = 0;
    p_acc = 0;
    return;
  }

  /**
   * Sets the values for the first derivatives of the endpoints of
   * the data set.  This method can only be called for cubic
   * splines with clamped boundary conditions, i.e. if
   * NumericalApproximation::InterpType is
   * @a CubicClamped.
   *
   * @param yp1 First derivative of the function evaluated at the
   *            domain minimum.
   * @param ypn First derivative of the function evaluated at the
   *            domain maximum.
   * @throws Isis::IException::Programmer "Method only used for
   *             cspline-clamped interpolation type"
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original verison.
   */
  void NumericalApproximation::SetCubicClampedEndptDeriv(double yp1, double ypn) {
    if(p_itype != NumericalApproximation::CubicClamped) {
      ReportException(IException::Programmer, "SetCubicClampedEndptDeriv()",
                      "This method is only valid for cspline-clamped interpolation, may not be used for "
                      + Name() + " interpolation",
                      _FILEINFO_);
    }
    p_clampedDerivFirstPt = yp1;
    p_clampedDerivLastPt = ypn;
    p_clampedEndptsSet = true;
    return;
  }

  /**
   * Adds values for the first derivatives of the data points.
   * This method can only be called for cubic Hermite splines,
   * i.e. if NumericalApproximation::InterpType is
   * @a CubicHermite. These values should be entered in the order
   * of the corresponding data points.
   *
   * @param n     Number of derivative values to be added.
   * @param fprimeOfx Array of derivative values to be added.
   * @throws Isis::IException::Programmer "Method only used for
   *             cspline-Hermite interpolation type"
   * @internal
   *   @history 2009-06-10 Jeannie Walldren - Original version
   */
  void NumericalApproximation::AddCubicHermiteDeriv(unsigned int n, double *fprimeOfx) {
    if(p_itype != NumericalApproximation::CubicHermite) {
      ReportException(IException::Programmer, "SetCubicHermiteDeriv()",
                      "This method is only valid for cspline-Hermite interpolation, may not be used for "
                      + Name() + " interpolation",
                      _FILEINFO_);
    }
    for(unsigned int i = 0; i < n; i++) {
      p_fprimeOfx.push_back(fprimeOfx[i]);
    }
    return;
  }
  /**
   * Adds values for the first derivatives of the data points.
   * This method can only be called for cubic Hermite splines,
   * i.e. if NumericalApproximation::InterpType is
   * @a CubicHermite. These values should be entered in the order
   * of the corresponding data points.
   *
   * @param fprimeOfx Vector of derivative values to be added.
   * @throws Isis::IException::Programmer "Method only used for
   *             cspline-Hermite interpolation type"
   * @internal
   *   @history 2009-06-10 Jeannie Walldren - Original version
   */
  void NumericalApproximation::AddCubicHermiteDeriv(
      const vector <double> &fprimeOfx) {
    if(p_itype != NumericalApproximation::CubicHermite) {
      ReportException(IException::Programmer, "SetCubicHermiteDeriv()",
                      "This method is only valid for cspline-Hermite interpolation, may not be used for "
                      + Name() + " interpolation",
                      _FILEINFO_);
    }

    // Avoid push_back if at all possible.
    if(!p_fprimeOfx.empty()) {
      for(unsigned int i = 0; i < fprimeOfx.size(); i++) {
        p_fprimeOfx.push_back(fprimeOfx[i]);
      }
    }
    else {
      p_fprimeOfx = fprimeOfx;
    }

    return;
  }
  /**
   * Adds value of a first derivative of a data point. This method
   * can only be called for cubic Hermite splines, i.e. if
   * NumericalApproximation::InterpType is
   * @a CubicHermite. Values should be entered in the order
   * of the corresponding data points.
   *
   * @param fprimeOfx Derivative value to be added.
   * @throws Isis::IException::Programmer "Method only used for
   *             cspline-Hermite interpolation type"
   * @internal
   *   @history 2009-06-10 Jeannie Walldren - Original version
   */
  void NumericalApproximation::AddCubicHermiteDeriv(
      double fprimeOfx) {
    if(p_itype != NumericalApproximation::CubicHermite) {
      ReportException(IException::Programmer, "SetCubicHermiteDeriv()",
                      "This method is only valid for cspline-Hermite interpolation, may not be used for "
                      + Name() + " interpolation",
                      _FILEINFO_);
    }
    p_fprimeOfx.push_back(fprimeOfx);
    return;
  }

  /**
   * @brief Retrieves the second derivatives of the data set.
   *
   * This method returns a vector of the same size as the data
   * set.  Each component is the second derivative of the
   * corresponding @a p_x value , as estimated by the
   * ComputeCubicClamped() method.
   *
   * @return @b vector @b \<double\> List of second derivatives
   *        for each @a p_x value in the data set.
   * @throws Isis::IException::Programmer "Method only used for
   *             cspline-clamped interpolation type"
   * @throws Isis::IException::Programmer "Unable to calculate the
   *             second derivatives of the data set for a clamped
   *             cubic spline."
   * @see ComputeCubicClamped()
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version
   *          written to save off the error estimate found by
   *          EvaluatePolynomialNeville()
   *   @history 2009-06-10 Jeannie Walldren - Moved itype
   *            verification to beginning of method.
   *
   */
  vector <double> NumericalApproximation::CubicClampedSecondDerivatives() {
    try {
      if(p_itype != NumericalApproximation::CubicClamped)
        ReportException(IException::Programmer, "CubicClampedSecondDerivatives()",
                        "This method is only valid for cspline-clamped interpolation type may not be used for "
                        + Name() + " interpolation",
                        _FILEINFO_);
      if(!p_clampedComputed) ComputeCubicClamped();
      return p_clampedSecondDerivs;
    }
    catch(IException &e) {  // catch exception from ComputeCubicClamped()
      throw IException(e,
                       e.errorType(),
                       "CubicClampedSecondDerivatives() - Unable to compute clamped cubic spline interpolation",
                       _FILEINFO_);
    }
  }

  /**
   * @brief Input data domain minimum value
   *
   * This method validates the data set for the assigned
   * interpolation type and returns the smallest value in
   * @a p_x of the data set.
   *
   * @return @b double Minimum domain value
   * @throws Isis::IException::Programmer "Unable to calculate the
   *             domain maximum for the data set."
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class.
   *   @history 2008-11-05 Jeannie Walldren - Modified to allow for
   *            interpolation types other than those supported by
   *            GSL.
   *
   */
  double NumericalApproximation::DomainMinimum() {
    try {
      if(GslInterpType(p_itype)) {
        // if GSL interpolation type, we need to compute, if not already done
        if(!GslComputed()) ComputeGsl();
        return (p_interp->interp->xmin);
      }
      else {
        if(!p_dataValidated) ValidateDataSet();
        return *min_element(p_x.begin(), p_x.end());
      }
    }
    catch(IException &e) { // catch exception from ComputeGsl() or ValidateDataSet()
      throw IException(e,
                       e.errorType(),
                       "DomainMinimum() - Unable to calculate the domain minimum for the data set",
                       _FILEINFO_);
    }
  }

  /**
   * @brief Input data domain maximum value
   *
   * This method validates the data set for the assigned
   * interpolation type and returns the largest value in
   * @a p_x of the data set.
   *
   * @return @b double Maximum domain value
   * @throws Isis::IException::Programmer "Unable to calculate the
   *             domain minimum for the data set."
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class.
   *   @history 2008-11-05 Jeannie Walldren - Modified to allow for
   *            interpolation types other than those supported by
   *            GSL.
   */
  double NumericalApproximation::DomainMaximum() {
    try {
      if(GslInterpType(p_itype)) {
        // if GSL interpolation type, we need to compute, if not already done
        if(!GslComputed()) ComputeGsl();
        return (p_interp->interp->xmax);
      }
      else {
        if(!p_dataValidated) ValidateDataSet();
        return *max_element(p_x.begin(), p_x.end());
      }
    }
    catch(IException &e) { // catch exception from ComputeGsl() or ValidateDataSet()
      throw IException(e,
                       e.errorType(),
                       "DomainMaximum() - Unable to calculate the domain maximum for the data set",
                       _FILEINFO_);
    }
  }

  /**
   * Returns whether the passed value is an element of the set of
   * x-values in the data set. This method uses a binary search of
   * the set of x-values to determine whether the input is
   * contained in this set.
   *
   * @param x Value to search for in the data set.
   * @return @b bool Whether the passed value is contained in the
   *         x-values of the data set.
   * @internal
   *   @history 2009-06-19 Jeannie Walldren - Original version.
   *
   */
  bool NumericalApproximation::Contains(double x) {
    return binary_search(p_x.begin(), p_x.end(), x);
  }

  /**
   *@brief Calculates interpolated or extrapolated value of
   *       tabulated data set for given domain value.
   *
   * This method returns the approximate value for @e f(@a a),
   * where @a a is an element near the domain and @e f is the
   * approximated function for the given data set. If the
   * given value, @a a, falls outside of the domain, then the
   * extrapoltion type is examined to determine the result.
   * @a CubicNeighborhood and GSL interpolation types can not
   * extrapolate, so the user must choose to throw an error or
   * return @e f evaluated at the nearest domain boundary.
   * @a CubicClamped, @a CubicHermite, and @a PolynomialNeville
   * interpolation types can extrapolate a value with accuracy
   * only if @a a is near enough to the domain boundary. Default
   * NumericalApproximation::ExtrapType is @a ThrowError.
   *
   * @param a     Domain value from which to interpolate a
   *              corresponding range value
   * @param etype NumericalApproximation::ExtrapType enum
   *              value indicates how to evaluate if @a a
   *              falls outside of the domain.   (Default:
   *              @a ThrowError)
   * @return @b double Range value interpolated or extrapolated
   *         for the given domain value
   * @throws Isis::IException::Programmer "Unable to evaluate the
   *             function at the point a"
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class.
   *   @history 2008-11-05 Jeannie Walldren - Modified to allow for
   *            interpolation types other than those supported by
   *            GSL.
   *   @history 2008-12-18 Jeannie Walldren - Added address
   *            operator (&) to input variables of type
   *            NumericalApproximation::ExtrapType.
   *   @history 2009-06-10 Jeannie Walldren - Added functionality
   *            for CubicHermite evaluation.
   */
  double NumericalApproximation::Evaluate(const double a, const ExtrapType &etype) {
    try {
      // a is const, so we must set up temporary variable in case value needs to be changed
      double a0;
      if(InsideDomain(a))  // this will validate data set, if not already done
        a0 = a;
      else a0 = ValueToExtrapolate(a, etype);
      // perform interpolation/extrapoltion
      if(p_itype == NumericalApproximation::CubicNeighborhood) {
        return EvaluateCubicNeighborhood(a0);
      }
      else if(p_itype == NumericalApproximation::PolynomialNeville) {
        p_polyNevError.clear();
        return EvaluatePolynomialNeville(a0);
      }
      else if(p_itype == NumericalApproximation::CubicClamped) {
        // if cubic clamped, we need to compute, if not already done
        if(!p_clampedComputed) ComputeCubicClamped();
        return EvaluateCubicClamped(a0);
      }
      else if(p_itype == NumericalApproximation::CubicHermite) {
        return EvaluateCubicHermite(a0);
      }
      // if GSL interpolation type we need to compute, if not already done
      if(!GslComputed()) ComputeGsl();
      double result;
      GslIntegrityCheck(gsl_spline_eval_e(p_interp, a0, p_acc, &result), _FILEINFO_);
      return (result);
    }
    catch(IException &e) { // catch exception from EvaluateCubicNeighborhood(), EvaluateCubicClamped(), EvaluatePolynomialNeville(), GslIntegrityCheck()
      throw IException(e,
                       e.errorType(),
                       "Evaluate() - Unable to evaluate the function at the point a = "
                       + IString(a),
                       _FILEINFO_);
    }
  }

  /**
   *@brief Calculates interpolated value for given set of domain
   *       values
   *
   * This method returns the approximate values for
   * @e f(@a a<sub>i</sub>), where @a a<sub>i</sub> is an
   * element in the domain and @e f is the interpolated function
   * for the given data set. If any of the given
   * values in @a a fall outside of the domain, then the
   * extrapoltion type is examined to determine the result.
   * @a CubicNeighborhood and GSL interpolation types can not
   * extrapolate, so the user must choose to throw an error or
   * return f evaluated at the nearest domain boundary.
   * @a CubicClamped and @a PolynomialNeville interpolation types
   * can extrapolate a value with accuracy only if
   * @a a<sub>@a i </sub> is near enough to the domain boundary.
   *
   *
   * @param a     Vector of domain values from which to
   *              interpolate a vector of corresponding
   *              range values
   * @param etype NumericalApproximation::ExtrapType enum value
   *              indicates how to evaluate any
   *              value of @a a that falls outside of the
   *              domain. (Default: @a ThrowError)
   * @return @b vector @b \<double\> Set of interpolated range
   *         values corresponding to the elements of the vector of
   *         domain values given
   * @throws Isis::IException::Programmer "Unable to evaluate the
   *             function at the vector of points, a."
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   *   @history 2008-12-18 Jeannie Walldren - Added address
   *            operator (&) to input variables of type vector and
   *            NumericalApproximation::ExtrapType.
   *   @history 2009-06-10 Jeannie Walldren - Added functionality
   *            for CubicHermite evaluation.
   */
  vector <double> NumericalApproximation::Evaluate(const vector <double> &a, const ExtrapType &etype) {
    try {
      if(p_itype == NumericalApproximation::CubicNeighborhood) {
        // cubic neighborhood has it's own method that will take entire vector
        // this is faster than looping through values calling Evaluate(double)
        // for each component of the passed in vector
        return EvaluateCubicNeighborhood(a, etype);
      }
      vector <double> result(a.size());
      if(p_itype == NumericalApproximation::PolynomialNeville) {
        // cannot loop through values calling Evaluate(double)
        // because it will clear the p_polyNevError vector each time
        p_polyNevError.clear();
        for(unsigned int i = 0; i < result.size(); i++) {
          double a0;
          if(InsideDomain(a[i]))
            a0 = a[i];
          else a0 = ValueToExtrapolate(a[i], etype);
          result[i] = EvaluatePolynomialNeville(a0);
        }
        return result;
      }
      // cubic-clamped, cubic-Hermite and gsl types can be done by calling Evaluate(double)
      // for each value of the passed in vector
      for(unsigned int i = 0; i < result.size(); i++) {
        result[i] = Evaluate(a[i], etype);
      }
      return result;
    }
    catch(IException &e) { // catch exception from EvaluateCubicNeighborhood(), EvaluateCubicClamped(), EvaluatePolynomialNeville(), GslIntegrityCheck()
      throw IException(e,
                       e.errorType(),
                       "Evaluate() - Unable to evaluate the function at the given vector of points",
                       _FILEINFO_);
    }
  }

  /**
   * Retrieves the error estimate for the Neville's polynomial
   * interpolation type.  This method must be called after the
   * Evaluate() method has been invoked.  If the Evaluate() method
   * is passed a vector, this error will contain an error estimate
   * for each of the values of the passed in vector. Otherwise, it
   * will contain a single value.
   *
   * @return @b vector @b \<double\> Estimation of difference
   *         between actual value of
   *        @e f(a) and the polynomial Neville interpolated
   *        return value of Evaluate(), where @e f is the function
   *        that defines the given data set.
   * @throws Isis::IException::Programmer "Method only used for
   *             polynomial-Neville's interpolation type"
   * @throws Isis::IException::Programmer "Error not calculated"
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version
   *          written to save off the error estimate found by
   *          EvaluatePolynomialNeville()
   */
  vector <double> NumericalApproximation::PolynomialNevilleErrorEstimate() {
    if(p_itype != NumericalApproximation::PolynomialNeville) {
      ReportException(IException::Programmer, "PolynomialNevilleErrorEstimate()",
                      "This method is only valid for polynomial-Neville's, may not be used for "
                      + Name() + " interpolation",
                      _FILEINFO_);
    }
    if(p_polyNevError.empty()) {
      ReportException(IException::Programmer, "PolynomialNevilleErrorEstimate()",
                      "Error not calculated. This method only valid after Evaluate() has been called",
                      _FILEINFO_);
    }
    return p_polyNevError;
  }

  /**
   * @brief Approximates the first derivative of the data set
   *       function evaluated at the given domain value for GSL
   *       supported interpolation types.
   *
   * This method returns an approximation of the first derivative
   * evaluated at given a valid domain value, @a a. It
   * is a wrapper for the GSL subroutine
   * gsl_spline_eval_deriv_e().  If the
   * NumericalApproximation::InterpType is not a GSL type, then
   * this method throws an error.  No documentation was found
   * concerning the algorithm used by this method.
   *
   * @param a Domain value at which first deriviative is
   *           evaluated.
   * @return @b double First derivative approximation for the
   *         given domain value, if valid.
   * @throws Isis::IException::Programmer "Invalid argument. Value
   *             entered is outside of domain."
   * @throws Isis::IException::Programmer "Cannot use this method
   *             for interpolation type" (If the interpolation
   *             type is not GSL)
   * @throws Isis::IException::Programmer "Unable to compute the
   *             first derivative at a using the GSL
   *             interpolation"
   * @throws Isis::IException::Programmer "Unable to compute the
   *             first derivative at a. GSL integrity check
   *             failed"
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class. Original name firstDerivative().
   *   @history 2008-11-05 Jeannie Walldren - Modified to throw
   *            errors if passed value is out of domain or if
   *            interpolation type is not supported by GSL.
   */
  double NumericalApproximation::GslFirstDerivative(const double a) {
    try { // we need to compute, if not already done
      if(!GslComputed()) ComputeGsl();
    }
    catch(IException &e) { // catch exception from ComputeGsl()
      throw IException(e,
                       e.errorType(),
                       "GslFirstDerivative() - Unable to compute the first derivative at a = "
                       + IString(a) + " using the GSL interpolation",
                       _FILEINFO_);
    }
    if(!InsideDomain(a)) {
      ReportException(IException::Programmer, "GslFirstDerivative()",
                      "Invalid argument. Value entered, a = "
                      + IString(a) + ", is outside of domain = ["
                      + IString(DomainMinimum()) + ", "
                      + IString(DomainMaximum()) + "]",
                      _FILEINFO_);
    }
    if(!GslInterpType(p_itype)) {
      ReportException(IException::Programmer, "GslFirstDerivative()",
                      "Method only valid for GSL interpolation types, may not be used for "
                      + Name() + " interpolation",
                      _FILEINFO_);
    }
    try {
      double value;
      GslIntegrityCheck(gsl_spline_eval_deriv_e(p_interp, a, p_acc, &value), _FILEINFO_);
      return (value);
    }
    catch(IException &e) { // catch exception from GslIntegrityCheck()
      throw IException(e,
                       e.errorType(),
                       "GslFirstDerivative() - Unable to compute the first derivative at a = "
                       + IString(a) + ". GSL integrity check failed",
                       _FILEINFO_);
    }
  }

  /**
   * @brief Approximates the first derivative of the data set
   *       function evaluated at the given domain value for
   *       @a CubicHermite interpolation type.
   *
   * This method returns an approximation of the first derivative
   * evaluated at given a valid domain value, @a a. It is able to
   * extrapolate for values not far outside of the domain
   *
   * @internal
   *   @history 2009-07-30 Debbie Cook - Original Version
   */
  double NumericalApproximation::EvaluateCubicHermiteFirstDeriv(const double a) {
    if(p_itype != NumericalApproximation::CubicHermite) { //??? is this necessary??? create single derivative method with GSL?
      ReportException(IException::User, "EvaluateCubicHermiteFirstDeriv()",
                      "This method is only valid for cspline-Hermite interpolation, may not be used for "
                      + Name() + " interpolation",
                      _FILEINFO_);
    }
    if(p_fprimeOfx.size() != Size()) {
      ReportException(IException::User, "EvaluateCubicHermiteFirstDeriv()",
                      "Invalid arguments. The size of the first derivative vector does not match the number of (x,y) data points.",
                      _FILEINFO_);
    }
    // find the interval in which "a" exists
    int lowerIndex = FindIntervalLowerIndex(a);

    // we know that "a" is within the domain since this is verified in
    // Evaluate() before this method is called, thus n <= Size()
    if(a == p_x[lowerIndex]) {
      return p_fprimeOfx[lowerIndex];
    }
    if(a == p_x[lowerIndex+1]) {
      return p_fprimeOfx[lowerIndex+1];
    }

    double x0, x1, y0, y1, m0, m1;
    // a is contained within the interval (x0,x1)
    x0 = p_x[lowerIndex];
    x1 = p_x[lowerIndex+1];
    // the corresponding known y-values for x0 and x1
    y0 = p_y[lowerIndex];
    y1 = p_y[lowerIndex+1];
    // the corresponding known tangents (slopes) at (x0,y0) and (x1,y1)
    m0 = p_fprimeOfx[lowerIndex];
    m1 = p_fprimeOfx[lowerIndex+1];

    double h, t;
    h = x1 - x0;
    t = (a - x0) / h;
    if(h != 0.) {
      return ((6 * t * t - 6 * t) * y0 + (3 * t * t - 4 * t + 1) * h * m0 + (-6 * t * t + 6 * t) * y1 + (3 * t * t - 2 * t) * h * m1) / h;
    }
    else {
      return 0;  // Should never happen
    }
  }

  /**
   * @brief Uses an @a n point backward first
   *        difference formula to approximate the first derivative
   *        evaluated at a given domain value.
   *
   * This method uses backward first difference formulas to return
   * an approximation of the first derivative of the interpolated
   * data set function evaluated at given a valid domain value,
   * @a a. Backward difference formulas use
   * @a n points, with the largest @e x value at
   * @a a, for numerical differentiation approximation.
   * This method uses one of the following formulas:
   * <UL>
   *   <LI> 2-point backward difference.
   *     @f[
   *          f\prime(a) \approx \frac{1}{h}[f(a) - f(a - h)]
   *     @f]
   *   <LI> 3-point backward difference.
   *     @f[
   *          f\prime(a) \approx \frac{1}{2h}[3f(a) - 4f(a - h) +
   *          f(a - 2h)]
   *     @f]
   * </UL>
   *
   * @param a Domain value at which first deriviative is
   *          evaluated.
   * @param n The number of points used in the formula
   * @param h Distance between nearest points in the formula
   * @return @b double First derivative approximation for the
   *         given domain value
   * @throws Isis::IException::Programmer "Invalid argument. Value
   *             entered is outside of domain."
   * @throws Isis::IException::Programmer "Formula steps outside
   *             of domain." If a-(n-1)h is less than domain min.
   * @throws Isis::IException::Programmer "Invalid argument. There
   *             is no n-point backward difference formula in
   *             use."
   * @throws Isis::IException::Programmer "Unable to calculate
   *             backward first difference for given (a,n,h)"
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  double NumericalApproximation::BackwardFirstDifference(const double a, const unsigned int n, const double h) {
    if(!InsideDomain(a)) {
      ReportException(IException::Programmer, "BackwardFirstDifference()",
                      "Invalid argument. Value entered, a = "
                      + IString(a) + ", is outside of domain = ["
                      + IString(DomainMinimum()) + ", "
                      + IString(DomainMaximum()) + "]",
                      _FILEINFO_);
    }
    if(!InsideDomain(a - (n - 1)*h)) {
      ReportException(IException::Programmer, "BackwardFirstDifference()",
                      "Formula steps outside of domain. For "
                      + IString((int) n) + "-point backward difference, a-(n-1)h = "
                      + IString(a - (n - 1)*h) + " is smaller than domain min = "
                      + IString(DomainMinimum())
                      + ".  Try forward difference or use smaller value for h or n",
                      _FILEINFO_);
    }
    if(!p_dataValidated) ValidateDataSet();
    vector <double> f;
    double xi;
    try {
      for(double i = 0; i < n; i++) {
        xi = a + h * (i - (n - 1));
        f.push_back(Evaluate(xi)); // allow ExtrapType = ThrowError (default)
      }
    }
    catch(IException &e) { // catch exception from Evaluate()
      throw IException(e,
                       e.errorType(),
                       "BackwardFirstDifference() - Unable to calculate backward first difference for (a, n, h) = ("
                       + IString(a) + ", " + IString((int) n) + ", " + IString(h) + ")",
                       _FILEINFO_);
    }
    switch(n) {
      case 2:
        return (-f[0] + f[1]) / h;              //2pt backward
      case 3:
        return (3 * f[2] - 4 * f[1] + f[0]) / (2 * h); //3pt backward
      default:
        throw IException(IException::Programmer,
                         "BackwardFirstDifference() - Invalid argument. There is no "
                         + IString((int) n) + "-point backward difference formula in use",
                         _FILEINFO_);
    }
  }


  /**
   * @brief Uses an @a n point forward first difference
   *        formula to approximate the first derivative evaluated
   *        at a given domain value.
   * This method uses forward first difference formulas to return
   * an approximation of the first derivative of the interpolated
   * data set function evaluated at given a valid domain value,
   * @a a. Forward difference formulas use
   * @a n points, with the smallest @e x value at
   * @a a, for numerical differentiation approximation.
   * This method uses one of the following formulas:
   * <UL>
   *   <LI> 2-point forward difference.
   *     @f[
   *          f\prime(a) \approx \frac{1}{h}[f(a + h) - f(a)]
   *     @f]
   *   <LI> 3-point forward difference.
   *     @f[
   *          f\prime(a) \approx \frac{1}{2h}[-f(a + 2h) +
   *          4f(a + h) - 3f(a)]
   *     @f]
   * </UL>
   *
   * @param a Domain value at which first deriviative is
   *          evaluated.
   * @param n The number of points used in the formula.
   * @param h Distance between nearest points in the formula.
   * @return @b double First derivative approximation for the
   *         given domain value
   * @throws Isis::IException::Programmer "Invalid argument. Value
   *             entered is outside of domain."
   * @throws Isis::IException::Programmer "Formula steps outside
   *             of domain." If a+(n-1)h is greater than domain
   *             max.
   * @throws Isis::IException::Programmer "Invalid argument. There
   *             is no n-point forward difference formula in use."
   * @throws Isis::IException::Programmer "Unable to calculate
   *             forward first difference for (a,n,h)"
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  double NumericalApproximation::ForwardFirstDifference(const double a, const unsigned int n, const double h) {
    if(!InsideDomain(a)) {
      ReportException(IException::Programmer, "ForwardFirstDifference()",
                      "Invalid argument. Value entered, a = "
                      + IString(a) + ", is outside of domain = ["
                      + IString(DomainMinimum()) + ", "
                      + IString(DomainMaximum()) + "]",
                      _FILEINFO_);
    }
    if(!InsideDomain(a + (n - 1)*h)) {
      ReportException(IException::Programmer, "ForwardFirstDifference()",
                      "Formula steps outside of domain. For "
                      + IString((int) n) + "-point forward difference, a+(n-1)h = "
                      + IString(a + (n - 1)*h) + " is greater than domain max = "
                      + IString(DomainMaximum())
                      + ".  Try backward difference or use smaller value for h or n",
                      _FILEINFO_);
    }
    if(!p_dataValidated) ValidateDataSet();
    vector <double> f;
    double xi;
    try {
      for(double i = 0; i < n; i++) {
        xi = a + h * i;
        f.push_back(Evaluate(xi));// allow ExtrapType = ThrowError (default)
      }
    }
    catch(IException &e) { // catch exception from Evaluate()
      throw IException(e,
                       e.errorType(),
                       "ForwardFirstDifference() - Unable to calculate forward first difference for (a, n, h) = ("
                       + IString(a) + ", " + IString((int) n) + ", " + IString(h) + ")",
                       _FILEINFO_);
    }
    switch(n) {
      case 2:
        return (-f[0] + f[1]) / h;              //2pt forward
      case 3:
        return (-3 * f[0] + 4 * f[1] - f[2]) / (2 * h); //3pt forward
      default:
        throw IException(IException::Programmer,
                         "ForwardFirstDifference() - Invalid argument. There is no "
                         + IString((int) n) + "-point forward difference formula in use",
                         _FILEINFO_);
    }
  }


  /**
   * @brief Uses an @a n point center first difference
   *        formula to approximate the first derivative evaluated
   *        at a given domain value.
   * This method uses center first difference formulas to return
   * an approximation of the first derivative of the interpolated
   * data set function evaluated at given a valid domain value,
   * @a a. Center difference formulas use
   * @a n points, centered at @a a, for
   * numerical differentiation approximation. This method uses one
   * of the following formulas:
   * <UL>
   *   <LI> 3-point center difference.
   *     @f[
   *          f\prime(a) \approx \frac{1}{2h}[f(a + h) -
   *          f(a - h)]
   *     @f]
   *   <LI> 5-point center difference.
   *     @f[
   *          f\prime(a) \approx \frac{1}{12h}[-f(a + 2h) +
   *          8f(a +h) - 8f(a - h) + f(a - 2h)]
   *     @f]
   * </UL>
   *
   * @param a Domain value at which first deriviative is
   *          evaluated.
   * @param n The number of points used in the formula.
   * @param h Distance between nearest points in the formula.
   * @return @b double First derivative approximation for the
   *         given domain value
   * @throws Isis::IException::Programmer "Invalid argument. Value
   *             entered is outside of domain."
   * @throws Isis::IException::Programmer "Formula steps outside
   *             of domain." If a+(n-1)h is greater than domain
   *             max or a-(n-1)h is less than domain min.
   * @throws Isis::IException::Programmer "Invalid argument. There
   *             is no n-point center difference formula in use."
   * @throws Isis::IException::Programmer "Unable to calculate
   *             center first difference for (a,n,h)"
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  double NumericalApproximation::CenterFirstDifference(const double a, const unsigned int n, const double h) {
    if(!InsideDomain(a)) {
      ReportException(IException::Programmer, "CenterFirstDifference()",
                      "Invalid argument. Value entered, a = "
                      + IString(a) + ", is outside of domain = ["
                      + IString(DomainMinimum()) + ", "
                      + IString(DomainMaximum()) + "]",
                      _FILEINFO_);
    }
    if(!InsideDomain(a + (n - 1)*h) || !InsideDomain(a - (n - 1)*h)) {
      ReportException(IException::Programmer, "CenterFirstDifference()",
                      "Formula steps outside of domain. For "
                      + IString((int) n) + "-point center difference, a-(n-1)h = "
                      + IString(a - (n - 1)*h) + " or a+(n-1)h = "
                      + IString(a + (n - 1)*h) + " is out of domain = ["
                      + IString(DomainMinimum()) + ", " + IString(DomainMaximum())
                      + "].  Use smaller value for h or n",
                      _FILEINFO_);
    }
    if(!p_dataValidated) ValidateDataSet();
    vector <double> f;
    double xi;
    try {
      for(double i = 0; i < n; i++) {
        xi = a + h * (i - (n - 1) / 2);
        f.push_back(Evaluate(xi));// allow ExtrapType = ThrowError (default)
      }
    }
    catch(IException &e) { // catch exception from Evaluate()
      throw IException(e,
                       e.errorType(),
                       "CenterFirstDifference() - Unable to calculate center first difference for (a, n, h) = ("
                       + IString(a) + ", " + IString((int) n) + ", " + IString(h) + ")",
                       _FILEINFO_);
    }
    switch(n) {
      case 3:
        return (-f[0] + f[2]) / (2 * h);              //3pt center
      case 5:
        return (f[0] - 8 * f[1] + 8 * f[3] - f[4]) / (12 * h); //5pt center
      default:
        throw IException(IException::Programmer,
                         "CenterFirstDifference() - Invalid argument. There is no "
                         + IString((int) n) + "-point center difference formula in use",
                         _FILEINFO_);
    }
  }

  /**
   * @brief Approximates the second derivative of the
   *       interpolated data set function evaluated at the given domain value
   *       for GSL supported interpolation types.
   *
   * This method returns an approximation of the second derivative
   * evaluated at given a valid domain value, @a a.  It
   * is a wrapper for the GSL subroutine
   * gsl_spline_eval_deriv2_e().  If the
   * NumericalApproximation::InterpType is not a GSL type, then
   * this method throws an error.  No documentation was found
   * concerning the algorithm used by this method.
   *
   *
   * @param a Domain value at which second deriviative is
   *           evaluated.
   * @return @b double Second derivative approximation for the
   *         given domain value, if valid.
   * @throws Isis::IException::Programmer "Invalid argument. Value
   *             entered is outside of domain."
   * @throws Isis::IException::Programmer "Cannot use this method
   *             for interpolation type" (If the interpolation
   *             type is not GSL)
   * @throws Isis::IException::Programmer "Unable to compute the
   *             second derivative at a using GSL interpolation"
   * @throws Isis::IException::Programmer "Unable to compute the
   *             second derivative at a. GSL integrity check
   *             failed"
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created
   *            in DataInterp class. Original name
   *            secondDerivative().
   *   @history 2008-11-05 Jeannie Walldren - Modified to throw
   *            errors if passed value is out of domain or if
   *            interpolation type is not supported by GSL.
   */
  double NumericalApproximation::GslSecondDerivative(const double a) {
    try { // we need to compute, if not already done
      if(!GslComputed()) ComputeGsl();
    }
    catch(IException &e) { // catch exception from ComputeGsl()
      throw IException(e,
                       e.errorType(),
                       "GslSecondDerivative() - Unable to compute the second derivative at a = "
                       + IString(a) + " using the GSL interpolation",
                       _FILEINFO_);
    }
    if(!InsideDomain(a))
      ReportException(IException::Programmer, "GslSecondDerivative()",
                      "Invalid argument. Value entered, a = "
                      + IString(a) + ", is outside of domain = ["
                      + IString(DomainMinimum()) + ", "
                      + IString(DomainMaximum()) + "]",
                      _FILEINFO_);
    if(!GslInterpType(p_itype))
      ReportException(IException::Programmer, "GslSecondDerivative()",
                      "Method only valid for GSL interpolation types, may not be used for "
                      + Name() + " interpolation",
                      _FILEINFO_);
    try {
      // we need to compute, if not already done
      if(!GslComputed()) ComputeGsl();
      double value;
      GslIntegrityCheck(gsl_spline_eval_deriv2_e(p_interp, a, p_acc, &value), _FILEINFO_);
      return (value);
    }
    catch(IException &e) { // catch exception from GslIntegrityCheck()
      throw IException(e,
                       e.errorType(),
                       "GslSecondDerivative() - Unable to compute the second derivative at a = "
                       + IString(a) + ". GSL integrity check failed",
                       _FILEINFO_);
    }
  }


  /**
   * @brief Approximates the second derivative of the data set
   *       function evaluated at the given domain value for
   *       @a CubicHermite interpolation type.
   *
   * This method returns an approximation of the second derivative
   * evaluated at given a valid domain value, @a a. It is able to
   * extrapolate for values not far outside of the domain
   *
   * @internal
   *   @history 2009-07-30 Debbie Cook - Original Version
   */
  double NumericalApproximation::EvaluateCubicHermiteSecDeriv(const double a) {
    if(p_itype != NumericalApproximation::CubicHermite) {
      ReportException(IException::User, "EvaluateCubicHermiteSecDeriv()",
                      "This method is only valid for cspline-Hermite interpolation, may not be used for "
                      + Name() + " interpolation",
                      _FILEINFO_);
    }
    if(p_fprimeOfx.size() != Size()) {
      ReportException(IException::User, "EvaluateCubicHermiteSecDeriv()",
                      "Invalid arguments. The size of the first derivative vector does not match the number of (x,y) data points.",
                      _FILEINFO_);
    }
    // find the interval in which "a" exists
    int lowerIndex = FindIntervalLowerIndex(a);
    double x0, x1, y0, y1, m0, m1;
    // a is contained within the interval (x0,x1)
    x0 = p_x[lowerIndex];
    x1 = p_x[lowerIndex+1];
    // the corresponding known y-values for x0 and x1
    y0 = p_y[lowerIndex];
    y1 = p_y[lowerIndex+1];
    // the corresponding known tangents (slopes) at (x0,y0) and (x1,y1)
    m0 = p_fprimeOfx[lowerIndex];
    m1 = p_fprimeOfx[lowerIndex+1];

    double h, t;
    h = x1 - x0;
    t = (a - x0) / h;
    if(h != 0.) {
      return ((12 * t - 6) * y0 + (6 * t - 4) * h * m0 + (-12 * t + 6) * y1 + (6 * t - 2) * h * m1) / h;
    }
    else {
      return 0; // Should never happen
    }
  }

  /**
   * @brief Uses an @a n point backward second difference
   *        formula to approximate the second derivative evaluated
   *        at a given domain value.
   * This method uses backward second difference formulas to
   * return an approximation of the second derivative of the
   * interpolated data set function evaluated at given a
   * valid domain value, @a a. Backward second
   * difference formulas use @a n points, with the
   * largest @e x value at @a a, for numerical differentiation
   * approximation. This method uses the following formula:
   * <UL>
   *   <LI> 3-point backward second difference.
   *     @f[
   *          f\prime\prime(a) \approx \frac{1}{h^2}[f(a)
   *          - 2f(a - h) + f(a - 2h)]
   *     @f]
   * </UL>
   *
   * @param a Domain value at which second deriviative is
   *          evaluated.
   * @param n The number of points used in the formula.
   * @param h Distance between nearest points in the formula.
   * @return @b double Second derivative approximation for the
   *         given domain value
   * @throws Isis::IException::Programmer "Invalid argument. Value
   *             entered is outside of domain."
   * @throws Isis::IException::Programmer "Formula steps outside
   *             of domain." If a-(n-1)h is less than domain min.
   * @throws Isis::IException::Programmer "Invalid argument. There
   *             is no n-point backward difference formula in
   *             use."
   * @throws Isis::IException::Programmer
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  double NumericalApproximation::BackwardSecondDifference(const double a, const unsigned int n, const double h) {
    if(!InsideDomain(a)) {
      ReportException(IException::Programmer, "BackwardSecondDifference()",
                      "Invalid argument. Value entered, a = "
                      + IString(a) + ", is outside of domain = ["
                      + IString(DomainMinimum()) + ", "
                      + IString(DomainMaximum()) + "]",
                      _FILEINFO_);
    }
    if(!InsideDomain(a - (n - 1)*h)) {
      ReportException(IException::Programmer, "BackwardSecondDifference()",
                      "Formula steps outside of domain. For "
                      + IString((int) n) + "-point backward difference, a-(n-1)h = "
                      + IString(a - (n - 1)*h) + " is smaller than domain min = "
                      + IString(DomainMinimum())
                      + ".  Try forward difference or use smaller value for h or n",
                      _FILEINFO_);
    }
    if(!p_dataValidated) ValidateDataSet();
    vector <double> f;
    double xi;
    try {
      for(double i = 0; i < n; i++) {
        xi = a + h * (i - (n - 1));
        f.push_back(Evaluate(xi));// allow ExtrapType = ThrowError (default)
      }
    }
    catch(IException &e) { // catch exception from Evaluate()
      throw IException(e,
                       e.errorType(),
                       "BackwardSecondDifference() - Unable to calculate backward second difference for (a, n, h) = ("
                       + IString(a) + ", " + IString((int) n) + ", " + IString(h) + ")",
                       _FILEINFO_);
    }
    switch(n) {
      case 3:
        return (f[0] - 2 * f[1] + f[2]) / (h * h);                   //3pt backward
      default:
        throw IException(IException::Programmer,
                         "BackwardSecondDifference() - Invalid argument. There is no "
                         + IString((int) n) + "-point backward second difference formula in use",
                         _FILEINFO_);
    }
  }


  /**
   * @brief Uses an @a n point forward second difference
   *        formula to approximate the second derivative evaluated at a
   *        given domain value.
   * This method uses forward second difference formulas to return
   * an approximation of the second derivative of the interpolated
   * data set function evaluated at given a valid domain value,
   * @a a. Forward second difference formulas use
   * @a n points, with the smallest @e x value at
   * @a a, for numerical differentiation approximation.
   * This method uses the following formula:
   * <UL>
   *   <LI> 3-point forward second difference.
   *     @f[
   *          f\prime\prime(a) \approx \frac{1}{h^2}[f(a +
   *          2h) - 2f(a + h) + f(a)]
   *     @f]
   * </UL>
   *
   * @param a Domain value at which second deriviative is evaluated.
   * @param n The number of points used in the formula.
   * @param h Distance between nearest points in the formula.
   * @return @b double Second derivative approximation for the
   *         given domain value
   * @throws Isis::IException::Programmer "Invalid argument. Value
   *             entered is outside of domain."
   * @throws Isis::IException::Programmer "Formula steps outside
   *             of domain." If a+(n-1)h is greater than domain
   *             max.
   * @throws Isis::IException::Programmer "Invalid argument. There
   *             is no n-point forward difference formula in use."
   * @throws Isis::IException::Programmer "Unable to calculate
   *             forward second difference for (a,n,h)"
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  double NumericalApproximation::ForwardSecondDifference(const double a, const unsigned int n, const double h) {
    if(!InsideDomain(a)) {
      ReportException(IException::Programmer, "ForwardSecondDifference()",
                      "Invalid argument. Value entered, a = "
                      + IString(a) + ", is outside of domain = ["
                      + IString(DomainMinimum()) + ", "
                      + IString(DomainMaximum()) + "]",
                      _FILEINFO_);
    }
    if(!InsideDomain(a + (n - 1)*h)) {
      ReportException(IException::Programmer, "ForwardSecondDifference()",
                      "Formula steps outside of domain. For "
                      + IString((int) n) + "-point forward difference, a+(n-1)h = "
                      + IString(a + (n - 1)*h) + " is greater than domain max = "
                      + IString(DomainMaximum())
                      + ".  Try backward difference or use smaller value for h or n",
                      _FILEINFO_);
    }
    if(!p_dataValidated) ValidateDataSet();
    vector <double> f;
    double xi;
    try {
      for(double i = 0; i < n; i++) {
        xi = a + h * i;
        f.push_back(Evaluate(xi));// allow ExtrapType = ThrowError (default)
      }
    }
    catch(IException &e) { // catch exception from Evaluate()
      throw IException(e,
                       e.errorType(),
                       "ForwardSecondDifference() - Unable to calculate forward second difference for (a, n, h) = ("
                       + IString(a) + ", " + IString((int) n) + ", " + IString(h) + ")",
                       _FILEINFO_);
    }
    switch(n) {
      case 3:
        return (f[0] - 2 * f[1] + f[2]) / (h * h);                   //3pt forward
      default:
        throw IException(IException::Programmer,
                         "ForwardSecondDifference() - Invalid argument. There is no "
                         + IString((int) n) + "-point forward second difference formula in use",
                         _FILEINFO_);
    }
  }


  /**
   * @brief Uses an @a n point center second difference
   *        formula to approximate the second derivative evaluated at a
   *        given domain value.
   * This method uses center second difference formulas to return
   * an approximation of the second derivative of the interpolated
   * data set function evaluated at given a valid domain value,
   * @a a. Center second difference formulas use
   * @a n points, centered at @a a, for
   * numerical differentiation approximation. This method uses one
   * of the following formulas:
   * <UL>
   *   <LI> 3-point center second difference.
   *     @f[
   *          f\prime\prime(a) \approx \frac{1}{h^2}[f(a + h)
   *          - 2f(a) + f(a - h)]
   *     @f]
   *   <LI> 5-point center second difference.
   *     @f[
   *          f\prime\prime(a) \approx \frac{1}{12h^2}[-f(a +
   *          2h) + 16f(a +h) - 30f(a) + 16f(a - h) - f(a
   *          - 2h)]
   *     @f]
   * </UL>
   *
   * @param a Domain value at which second deriviative is evaluated.
   * @param n The number of points used in the formula.
   * @param h Distance between nearest points in the formula.
   * @return @b double Second derivative approximation for the
   *         given domain value
   * @throws Isis::IException::Programmer "Invalid argument. Value
   *             entered is outside of domain."
   * @throws Isis::IException::Programmer "Formula steps outside
   *             of domain." If a+(n-1)h is greater than domain
   *             max or a-(n-1)h is less than domain min.
   * @throws Isis::IException::Programmer "Invalid argument. There
   *             is no n-point center difference formula in use."
   * @throws Isis::IException::Programmer "Unable to calculate
   *             center second difference for (a,n,h)"
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  double NumericalApproximation::CenterSecondDifference(const double a, const unsigned int n, const double h) {
    if(!InsideDomain(a)) {
      ReportException(IException::Programmer, "CenterSecondDifference()",
                      "Invalid argument. Value entered, a = "
                      + IString(a) + ", is outside of domain = ["
                      + IString(DomainMinimum()) + ", "
                      + IString(DomainMaximum()) + "]",
                      _FILEINFO_);
    }
    if(!InsideDomain(a + (n - 1)*h) || !InsideDomain(a - (n - 1)*h)) {
      ReportException(IException::Programmer, "CenterSecondDifference()",
                      "Formula steps outside of domain. For "
                      + IString((int) n) + "-point center difference, a-(n-1)h = "
                      + IString(a - (n - 1)*h) + " or a+(n-1)h = "
                      + IString(a + (n - 1)*h) + " is out of domain = ["
                      + IString(DomainMinimum()) + ", " + IString(DomainMaximum())
                      + "].  Use smaller value for h or n",
                      _FILEINFO_);
    }
    if(!p_dataValidated) ValidateDataSet();
    vector <double> f;
    double xi;
    try {
      for(double i = 0; i < n; i++) {
        xi = a + h * (i - (n - 1) / 2);
        f.push_back(Evaluate(xi));// allow ExtrapType = ThrowError (default)
      }
    }
    catch(IException &e) { // catch exception from Evaluate()
      throw IException(e,
                       e.errorType(),
                       "CenterSecondDifference() - Unable to calculate center second difference for (a, n, h) = ("
                       + IString(a) + ", " + IString((int) n) + ", " + IString(h) + ")",
                       _FILEINFO_);
    }
    switch(n) {
      case 3:
        return (f[0] - 2 * f[1] + f[2]) / (h * h);                   //3pt center
      case 5:
        return (-f[0] + 16 * f[1] - 30 * f[2] + 16 * f[3] - f[4]) / (12 * h * h); //5pt center
      default:
        throw IException(IException::Programmer,
                         "CenterSecondDifference() - Invalid argument. There is no "
                         + IString((int) n) + "-point center second difference formula in use",
                         _FILEINFO_);
    }
  }

  /**
   * @brief Approximates the integral of the data set
   *       function evaluated on the given interval for GSL
   *       supported interpolation types.
   *
   * This method returns an approximation of the integral
   * evaluated on a given valid domain interval,
   * (@a a,@a b).  It is a wrapper for the GSL subroutine
   * gsl_spline_eval_integ_e(). If the
   * NumericalApproximation::InterpType is not a GSL type, then
   * this method throws an error. No documentation was found
   * concerning the algorithm used by this method.
   *
   * @param a Lower endpoint at which integral is evaluated.
   * @param b Upper endpoint at which integral is evaluated.
   * @return @b double Integral approximation for the given
   *         interval, if valid.
   * @throws Isis::IException::Programmer (When a > b) "Invalid
   *        interval entered"
   * @throws Isis::IException::Programmer "Invalid arguments.
   *        Interval entered is not contained within domain"
   * @throws Isis::IException::Programmer "Cannot use this method
   *             for interpolation type" (If the interpolation
   *             type is not GSL)
   * @throws Isis::IException::Programmer "Unable to compute the
   *             integral on the interval (a,b) using GSL
   *             interpolation"
   * @throws Isis::IException::Programmer "Unable to compute the
   *             integral on the interval (a,b). GSL integrity
   *             check failed."
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  double NumericalApproximation::GslIntegral(const double a, const double b) {
    try { // we need to compute, if not already done
      if(!GslComputed()) ComputeGsl();
    }
    catch(IException &e) { // catch exception from ComputeGsl()
      throw IException(e,
                       e.errorType(),
                       "GslIntegral() - Unable to compute the integral on the interval (a,b) = ("
                       + IString(a) + ", " + IString(b)
                       + ") using the GSL interpolation",
                       _FILEINFO_);
    }
    if(a > b) {
      ReportException(IException::Programmer, "GslIntegral()",
                      "Invalid interval entered: [a,b] = ["
                      + IString(a) + ", " + IString(b) + "]",
                      _FILEINFO_);
    }
    if(!InsideDomain(a) || !InsideDomain(b)) {
      ReportException(IException::Programmer, "GslIntegral()",
                      "Invalid arguments. Interval entered ["
                      + IString(a) + ", " + IString(b)
                      + "] is not contained within domain ["
                      + IString(DomainMinimum()) + ", "
                      + IString(DomainMaximum()) + "]",
                      _FILEINFO_);
    }
    if(!GslInterpType(p_itype)) {
      ReportException(IException::Programmer, "GslIntegral()",
                      "Method only valid for GSL interpolation types, may not be used for "
                      + Name() + " interpolation",
                      _FILEINFO_);
    }
    try {
      double value;
      GslIntegrityCheck(gsl_spline_eval_integ_e(p_interp, a, b, p_acc, &value), _FILEINFO_);
      return (value);
    }
    catch(IException &e) { // catch exception from GslIntegrityCheck()
      throw IException(e,
                       e.errorType(),
                       "GslIntegral() - Unable to compute the integral on the interval (a,b) = ("
                       + IString(a) + ", " + IString(b)
                       + "). GSL integrity check failed",
                       _FILEINFO_);
    }
  }

  /**
   * @brief Uses the trapezoidal rule to approximate the
   *        integral of the interpolated data set function on the interval
   *        (@a a, @a b).
   * The trapeziod rule for integration approximation uses a
   * 2-point Newton-Cote formula.  This rule states:
   *         @f[
   *           \int_{a}^b f(x)dx \approx \frac{h}{2}[f(a) + f(b)]
   *         @f]
   *         where @e h = @a b - @a a.
   * This method uses a composite, or extended, trapeziodal rule
   * algorithm to approximate the integral.
   *
   * @param a Lower bound of interval.
   * @param b Upper bound of interval.
   * @return @b double Approximate integral of data set function
   *         from a to b.
   * @throws Isis::IException::Programmer "Unable to calculate the
   *                            integral on the interval (a,b)
   *                            using the trapezoidal rule"
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  double NumericalApproximation::TrapezoidalRule(const double a, const double b) {
    try {
      //n is the number of points used in the formula of the integration type chosen
      unsigned int n = 2;
      double result = 0;
      vector <double> f = EvaluateForIntegration(a, b, n);
      double h = f.back();
      f.pop_back();
      //Compute the integral using composite trapezoid formula
      int ii;
      for(unsigned int i = 0; i < (f.size() - 1) / (n - 1); i++) {
        ii = (i + 1) * (n - 1);
        result += (f[ii-1] + f[ii]) * h / 2;
      }
      return result;
    }
    catch(IException &e) { // catch exception from EvaluateForIntegration()
      throw IException(e,
                       e.errorType(),
                       "TrapezoidalRule() - Unable to calculate the integral on the interval (a,b) = ("
                       + IString(a) + ", " + IString(b)
                       + ") using the trapeziodal rule",
                       _FILEINFO_);
    }
  }

  /**
   * @brief Uses Simpson's 3-point rule to approximate the
   *        integral of the interpolated data set function on the interval
   *        (@a a,@a b).
   *
   * The Simpson's 3-Point Rule for numerical integration uses a
   * 3-point Newton-Cote formula.  This rule states:
   *         @f[
   *           \int_{a}^b f(x)dx \approx \frac{h}{3}[f(a) + f(a+h)
   *               + 4*f(a+2h)]
   *         @f]
   *         where @e h = (@a b - @a a)/2.
   * This method uses a composite, or extended, Simpson's rule
   * algorithm to approximate the integral.
   *
   *
   * @param a Lower bound of interval.
   * @param b Upper bound of interval.
   * @return @b double Approximate integral of data set function
   *         from a to b.
   * @throws Isis::IException::Programmer "Unable to calculate the
   *                            integral on the interval (a,b)
   *                            using Simpson's 3 point rule"
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  double NumericalApproximation::Simpsons3PointRule(const double a, const double b) {
    try {
      //n is the number of points used in the formula of the integration type chosen
      unsigned int n = 3;
      double result = 0;
      vector <double> f = EvaluateForIntegration(a, b, n);
      double h = f.back();
      f.pop_back();
      int ii;
      for(unsigned int i = 0; i < (f.size() - 1) / (n - 1); i++) {
        ii = (i + 1) * (n - 1);
        result += (f[ii-2] + 4 * f[ii-1] + f[ii]) * h / 3;
      }
      return result;
    }
    catch(IException &e) { // catch exception from EvaluateForIntegration()
      throw IException(e,
                       e.errorType(),
                       "Simpsons3PointRule() - Unable to calculate the integral on the interval (a,b) = ("
                       + IString(a) + ", " + IString(b)
                       + ") using Simpson's 3 point rule",
                       _FILEINFO_);
    }
  }


  /**
   * @brief Uses Simpson's 4-point rule to approximate the
   *        integral of the interpolated data set function on the interval
   *        (@a a,@a b).
   *
   * The Simpson's 4-point Rule for numerical integration
   * uses a 4-point Newton-Cote formula and is sometimes
   * called the Simpson's 3/8 Rule. This rule states:
   *         @f[
   *           \int_{a}^b f(x)dx \approx \frac{3h}{8}[f(a) +
   *               3f(a+h) + 3f(a+2h) + f(a+3h)]
   *         @f]
   *         where @e h = (@a b - @a a)/3.
   * This method uses a composite, or extended, Simpson's 3/8 rule
   * algorithm to approximate the integral.
   *
   * @param a Lower bound of interval.
   * @param b Upper bound of interval.
   * @return @b double Approximate integral of data set function
   *         from a to b.
   * @throws Isis::IException::Programmer "Unable to calculate the
   *                            integral on the interval (a,b)
   *                            using Simpson's 4 point rule"
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  double NumericalApproximation::Simpsons4PointRule(const double a, const double b) {
    try {
      //n is the number of points used in the formula of the integration type chosen
      unsigned int n = 4;
      double result = 0;
      vector <double> f = EvaluateForIntegration(a, b, n);
      double h = f.back();
      f.pop_back();
      int ii;
      for(unsigned int i = 0; i < (f.size() - 1) / (n - 1); i++) {
        ii = (i + 1) * (n - 1);
        result += (f[ii-3] + 3 * f[ii-2] + 3 * f[ii-1] + f[ii]) * h * 3 / 8;
      }
      return result;
    }
    catch(IException &e) { // catch exception from EvaluateForIntegration()
      throw IException(e,
                       e.errorType(),
                       "Simpsons4PointRule() - Unable to calculate the integral on the interval (a,b) = ("
                       + IString(a) + ", " + IString(b)
                       + ") using Simpson's 4 point rule",
                       _FILEINFO_);
    }
  }


  /**
   * @brief Uses Boole's Rule to approximate the integral of
   *        the interpolated data set function on the interval (@a
   *        a,@a b).
   *
   * The Boole's Rule for integration approximation uses a 5-point
   * Newton-Cote formula.  This rule states:
   *         @f[
   *           \int_{a}^b f(x)dx \approx \frac{2h}{45}[7f(a) +
   *               32f(a+h) + 12f(a+2h) + 32f(a+3h) + 7f(a+4h)]
   *         @f]
   *         where @e h = (@a b - @a a)/4.
   *
   * This method uses a composite, or extended, Boole's rule
   * formula to approximate the integral.
   *
   * @b Note: The method uses an algorithm that is adapted from
   * the IDL function int_tabulated.pro.
   *
   * @param a Lower bound of interval.
   * @param b Upper bound of interval.
   * @return @b double Approximate integral of data set function
   *         from a to b.
   * @throws Isis::IException::Programmer "Unable to calculate the
   *                            integral on the interval (a,b)
   *                            using Boole's rule"
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  double NumericalApproximation::BoolesRule(const double a, const double b) {
    try {
      //n is the number of points used in the formula of the integration type chosen
      unsigned int n = 5;
      double result = 0;
      vector <double> f = EvaluateForIntegration(a, b, n);
      double h = f.back();
      f.pop_back();
      int ii;
      for(unsigned int i = 0; i < (f.size() - 1) / (n - 1); i++) {
        ii = (i + 1) * (n - 1);
        result += (7 * f[ii-4] + 32 * f[ii-3] + 12 * f[ii-2] + 32 * f[ii-1] + 7 * f[ii]) * h * 2 / 45;
      }
      return result;
    }
    catch(IException &e) { // catch exception from EvaluateForIntegration()
      throw IException(e,
                       e.errorType(),
                       "BoolesRule() - Unable to calculate the integral on the interval (a,b) = ("
                       + IString(a) + ", " + IString(b)
                       + ") using Boole's rule",
                       _FILEINFO_);
    }
  }

  /**
   * @brief Calculates refinements extended trapezoidal rule to
   *        approximate the integral of the interpolated
   *        data set function on the interval (@a a,@a b).
   *
   * This method calculates the @a n<sup>th</sup> stage of
   * refinement of an extended trapezoidal rule. When called with
   * @a n = 1, the method returns the non-composite
   * trapezoidal estimate of the integral.  Subsequent calls with
   * @a n = 2, 3, ... (in sequential order) will improve the
   * accuracy by adding 2<sup>@a n-2 </sup> additional interior
   * points. This method can be used to integrate by the extended
   * trapeziodal rule if you know the number of steps you want to
   * take.  For example, if you want 2<sup>@e M </sup> + 1, use
   * the following code:
   * @code
   * double result;
   * for(int j = 1; j <= M+1; j++){
   *   result = RefineExtendedTrap(a,b,result,j);
   * }
   * @endcode
   * @b Note: Although this method may be used to approximate
   * an integral, as described above, it is most often used by
   * RombergsMethod() to integrate.
   *
   * @param a  Lower limit of integration
   * @param b  Upper limit of integration
   * @param s  Previous value of refinement
   * @param n  Number of partitions to use when integrating
   * @return  @b double Integral (refined) approximation of the
   *          function on the interval (a, b)
   * @throws Isis::IException::Programmer "Unable to calculate the
   *                            integral on the interval (a,b)
   *                            using the extended trapezoidal
   *                            rule"
   * @see RombergsMethod()
   * @internal
   *   @history 1999-08-11 K Teal Thompson - Original version in
   *            Isis2.
   *   @history 2007-02-20 Janet Barrett - Imported to Isis3 in AtmosModel class. Original name r8trapzd().
   *   @history 2008-11-05 Jeannie Walldren - Renamed, modified
   *            input parameters, adapted to be used with data set.
   *
   */
  double NumericalApproximation::RefineExtendedTrap(double a, double b, double s, unsigned int n) {
    // This method was derived from an algorithm in the text
    // Numerical Recipes in C: The Art of Scientific Computing
    // Section 4.2 by Flannery, Press, Teukolsky, and Vetterling
    try {
      if(n == 1) {
        double begin, end;
        if(GslInterpType(p_itype) || p_itype == NumericalApproximation::CubicNeighborhood) {
          // if a or b are outside the domain, return y-value of nearest endpoint
          begin = Evaluate(a, NumericalApproximation::NearestEndpoint);
          end = Evaluate(b, NumericalApproximation::NearestEndpoint);
        }
        else {
          // if a or b are outside the domain, return extrapolated y-value
          begin = Evaluate(a, NumericalApproximation::Extrapolate);
          end = Evaluate(b, NumericalApproximation::Extrapolate);
        }
        return (0.5 * (b - a) * (begin + end));
      }
      else {
        int it;
        double delta, tnm, x, sum;
        it = (int)(pow(2.0, (double)(n - 2)));
        tnm = it;
        delta = (b - a) / tnm; // spacing of the points to be added
        x = a + 0.5 * delta;
        sum = 0.0;
        for(int i = 0; i < it; i++) {
          if(GslInterpType(p_itype) || p_itype == NumericalApproximation::CubicNeighborhood) {
            sum = sum + Evaluate(x, NumericalApproximation::NearestEndpoint);
          }
          else {
            sum = sum + Evaluate(x, NumericalApproximation::Extrapolate);
          }
          x = x + delta;
        }
        return (0.5 * (s + (b - a) * sum / tnm));// return refined value of s
      }
    }
    catch(IException &e) { // catch exception from Evaluate()
      throw IException(e,
                       e.errorType(),
                       "RefineExtendedTrap() - Unable to calculate the integral on the interval (a,b) = ("
                       + IString(a) + ", " + IString(b)
                       + ") using the extended trapeziodal rule",
                       _FILEINFO_);
    }
  }

  /**
   * @brief  Uses Romberg's method to approximate the integral of
   *        the interpolated data set function on the interval (@a
   *        a,@a b).
   *
   * This method returns the integral of the function from a to b
   * using Romberg's method for Numerical Integration of order 2K,
   * where, e.g., K=2 is simpson's rule. This is a generalization
   * of the trapezoidal rule.  Romberg Integration uses a series
   * of refinements on the extended (or composite) trapezoidal
   * rule to reduce error terms. This method makes use of
   * Neville's algorithm for polynomial interpolation to
   * extrapolate successive refinements.
   *
   * @param a  Lower limit of integration
   * @param b  Upper limit of integration
   * @return  @b double Integral approximation of the function on
   *          the interval (a, b)
   * @throws Isis::IException::Programmer "Failed to converge."
   * @throws Isis::IException::Programmer "Unable to calculate the
   *                            integral on the interval (a,b)
   *                            using Romberg's method"
   * @see http://mathworld.wolfram.com/RombergIntegration.html
   * @see RefineExtendedTrap()
   * @internal
   *   @history 1999-08-11 K Teal Thompson - Original version in
   *            Isis2.
   *   @history 2000-12-29 Randy Kirk - Add absolute error
   *            tolerance.
   *   @history 2007-02-20 Janet Barrett - Imported to Isis3 in
   *            AtmosModel class. Original name r8qromb().
   *   @history 2008-11-05 Jeannie Walldren - Renamed, modified
   *            input parameters, adapted to be used with data set.
   */
  double NumericalApproximation::RombergsMethod(double a, double b) {
    // This method was derived from an algorithm in the text
    // Numerical Recipes in C: The Art of Scientific Computing
    // Section 4.3 by Flannery, Press, Teukolsky, and Vetterling
    int maxits = 20;
    double dss = 0; // error estimate for
    double h[maxits+1]; // relative stepsizes for trap
    double trap[maxits+1]; // successive trapeziodal approximations
    double epsilon; // desired fractional accuracy
    double epsilon2;// desired fractional accuracy
    double ss; // result

    epsilon = 1.0e-4;
    epsilon2 = 1.0e-6;

    h[0] = 1.0;
    try {
      NumericalApproximation interp(NumericalApproximation::PolynomialNeville);
      for(int i = 0; i < maxits; i++) {
        // i will determine number of trapezoidal partitions of area
        // under curve for "integration" using refined trapezoidal rule
        trap[i] = RefineExtendedTrap(a, b, trap[i], i + 1); // validates data here
        if(i >= 4) {
          interp.AddData(5, &h[i-4], &trap[i-4]);
          // PolynomialNeville can extrapolate data outside of domain
          ss = interp.Evaluate(0.0, NumericalApproximation::Extrapolate);
          dss = interp.PolynomialNevilleErrorEstimate()[0];
          interp.Reset();
          // we work only until our necessary accuracy is achieved
          if(fabs(dss) <= epsilon * fabs(ss)) return ss;
          if(fabs(dss) <= epsilon2) return ss;
        }
        trap[i+1] = trap[i];
        h[i+1] = 0.25 * h[i];
        // This is a key step:  the factor is 0.25d0 even though
        // the stepsize is decreased by 0.5d0.  This makes the
        // extraplolation a polynomial in h-squared as allowed
        // by the equation from Numerical Recipes 4.2.1 pg.132,
        // not just a polynomial in h.
      }
    }
    catch(IException &e) { // catch error from RefineExtendedTrap, Constructor, Evaluate, PolynomialNevilleErrorEstimate
      throw IException(e,
                       e.errorType(),
                       "RombergsMethod() - Unable to calculate the integral on the interval (a,b) = ("
                       + IString(a) + ", " + IString(b)
                       + ") using Romberg's method",
                       _FILEINFO_);
    }
    throw IException(IException::Programmer,
                     "RombergsMethod() - Unable to calculate the integral using RombergsMethod() - Failed to converge in "
                     + IString(maxits) + " iterations",
                     _FILEINFO_);
  }

  /**
   * @brief Resets the state of the object
   *
   * This method deallocates the internal state of the object and
   * clears the data set and class variables. The object is
   * returned to its original state, so data points must be
   * entered before computing again.  This does not clear or reset
   * the interpolation type.
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class.
   *   @history 2008-11-05 Jeannie Walldren - Modified to reset class
   *            variables related to interpolation types not
   *            supported by GSL.
   *   @history 2009-06-10 Jeannie Walldren - Reset new variable,
   *            p_fprimeOfx.
   */
  void NumericalApproximation::Reset() {
    if(GslInterpType(p_itype)) GslDeallocation();
    p_clampedComputed = false;
    p_clampedEndptsSet = false;
    p_dataValidated = false;
    p_x.clear();
    p_y.clear();
    p_clampedSecondDerivs.clear();
    p_clampedDerivFirstPt = 0;
    p_clampedDerivLastPt = 0;
    p_polyNevError.clear();
    p_fprimeOfx.clear();
    return;
  }

  /**
   * @brief Resets the state of the object and resets
   *        interpolation type.
   *
   * This method will clear the data set, reset the validation
   * status of the data to false, reset the interpolation type,
   * clear class variables and deallocate (inactivate) the
   * internal state of the object. The object is returned to its
   * original state, so data points must be entered before
   * computing again.
   * @param itype NumericalApproximation::InterpType enum value to be assigned to this
   *              object
   * @throws Isis::IException::Programmer "Unable to reset
   *             interpolation type"
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  void NumericalApproximation::Reset(NumericalApproximation::InterpType itype) {
    try {
      Reset();
      SetInterpType(itype);
      return;
    }
    catch(IException &e) { // catch exception from SetInterpType()
      throw IException(e,
                       e.errorType(),
                       "Reset() - Unable to reset interpolation type",
                       _FILEINFO_);
    }
  }

  /**
   * @brief Sets interpolation type.
   * This method will reset the interpolation type to that of the
   * input parameter and deallocate memory.  Unlike the
   * Reset() method, this method does NOT discard the data set. However, since
   * interpolations have differing requirements for valid data
   * sets, the data set stored is not yet validated for the new
   * interpolation type.  Other class variables are cleared if
   * they are interpolation type dependent.
   *
   * @param itype NumericalApproximation::InterpType enum value to be assigned to this
   *              object
   * @throws Isis::IException::Programmer "Invalid argument.
   *             Unknown interpolation type"
   * @throws Isis::IException::Programmer "Unable to set
   *             interpolation type"
   * @see Reset()
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   *   @history 2009-06-10 Jeannie Walldren - Reset new variable,
   *            p_fprimeOfx.
   */
  void NumericalApproximation::SetInterpType(NumericalApproximation::InterpType itype) {
    //  Validates the parameter
    if(GslInterpType(itype)) {
      try {
        GslFunctor(itype);
      }
      catch(IException &e) { // catch exception from GslFunctor()
        throw IException(e,
                         e.errorType(),
                         "SetInterpType() - Unable to set interpolation type",
                         _FILEINFO_);
      }
    }
    else if(itype > 9) {    // there are currently 9 interpolation types
      ReportException(IException::Programmer, "SetInterpType()",
                      "Invalid argument. Unknown interpolation type: "
                      + IString(NumericalApproximation::InterpType(itype)),
                      _FILEINFO_);
    }
    // p_x, p_y are kept and p_itype is replaced
    p_itype = itype;
    // reset state of class variables that are InterpType dependent  //??? should we keep some of this info?
    p_dataValidated = false;
    p_clampedComputed = false;
    p_clampedEndptsSet = false;
    p_clampedSecondDerivs.clear();
    p_clampedDerivFirstPt = 0;
    p_clampedDerivLastPt = 0;
    p_polyNevError.clear();
    p_fprimeOfx.clear();
  }

  /**
   * @brief Initializes the object upon instantiation
   *
   * This method sets up the initial state of the object, typically at
   * instantiation.  It populates the interpolation function
   * table identifying which options are available to the users of
   * this class.
   *
   * GSL error handling is turned off - upon repeated instantiation of this
   * object.  The GSL default error handling, termination of the application via
   * an abort() is unacceptable.  This calls adopts an alternative policy provided
   * by the GSL whereby error checking must be done by the calling environment.
   * This has an unfortunate drawback in that it is not enforceable in an object
   * oriented environment that utilizes the GSL in disjoint classes.
   * @param itype NumericalApproximation::InterpType enum value to
   *              be assigned to this object
   * @throws Isis::IException::Programmer "Unable to initialize
   *             NumericalApproximation object"
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class. Original name initInterp().
   *   @history 2008-11-05 Jeannie Walldren - Modified to allow for
   *            interpolation types not supported by GSL.
   */
  void NumericalApproximation::Init(NumericalApproximation::InterpType itype) {
    if(p_interpFunctors.empty()) {
      p_interpFunctors.insert(make_pair(Linear, gsl_interp_linear));
      p_interpFunctors.insert(make_pair(Polynomial, gsl_interp_polynomial));
      p_interpFunctors.insert(make_pair(CubicNatural, gsl_interp_cspline));
      p_interpFunctors.insert(make_pair(CubicNatPeriodic, gsl_interp_cspline_periodic));
      p_interpFunctors.insert(make_pair(Akima, gsl_interp_akima));
      p_interpFunctors.insert(make_pair(AkimaPeriodic, gsl_interp_akima_periodic));
    }

    p_acc = 0;
    p_interp = 0;
    try {
      SetInterpType(itype);
    }
    catch(IException &e) { // catch exception from SetInterpType()
      throw IException(e,
                       e.errorType(),
                       "Init() - Unable to initialize NumericalApproximation object",
                       _FILEINFO_);
    }
    // Turn all GSL error handling off...repeatedly, every time this routine is
    // called.
    gsl_set_error_handler_off();
  }

  /**
   * Returns whether an interpolation type is adapted from the GSL
   * library.  GSL interpolation types include the following:
   *   <UL>
   *     <LI>@a Linear           = 0
   *     <LI>@a Polynomial       = 1
   *     <LI>@a CubicNatural     = 3
   *     <LI>@a CubicNatPeriodic = 5
   *     <LI>@a Akima            = 7
   *     <LI>@a AkimaPeriodic    = 8
   *    </UL>
   *
   * @param itype Interpolation type to be compared to GSL list
   * @return @b bool  True if interpolation type is GSL
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version
   */
  bool NumericalApproximation::GslInterpType(NumericalApproximation::InterpType itype) const {
    if(itype == NumericalApproximation::Linear)        return true;
    if(itype == NumericalApproximation::Polynomial)    return true;
    if(itype == NumericalApproximation::CubicNatural)  return true;
    if(itype == NumericalApproximation::CubicNatPeriodic) return true;
    if(itype == NumericalApproximation::Akima)         return true;
    if(itype == NumericalApproximation::AkimaPeriodic) return true;
    return false;
  }

  /**
   * @brief Allocates GSL interpolation functions
   *
   * This method is called within Compute to allocate pointers
   * to a GSL interpolation object and to a GSL accelerator object
   * (for interpolation look-ups).  This is only called for the
   * GSL interpolation types. If it is deemed invalid, an
   * exception will be thrown.
   *
   * @param npoints Number of points to allocate for the GSL
   *                interpolator
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class. Original name allocate().
   *   @history 2008-11-05 Jeannie Walldren - Renamed.
   */
  void NumericalApproximation::GslAllocation(unsigned int npoints) {
    GslDeallocation();
    //get pointer to accelerator object (iterator for interpolation lookups)
    p_acc = gsl_interp_accel_alloc();
    //get pointer to interpolation object of interp type given for npoints datapoints
    p_interp = gsl_spline_alloc(GslFunctor(p_itype), npoints);
    return;
  }

  /**
   * @brief Deallocate GSL interpolator resources, if used
   *
   * If a GSL interpolator function has been allocated, this
   * routine will free its resources and reset internal pointers
   * to reflect this state and provide a mechanism to test its
   * state. Otherwise, this method sets the internal GSL pointers
   * to their default, 0.
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class. Original name deallocate().
   *   @history 2008-11-05 Jeannie Walldren - Renamed.
   */
  void NumericalApproximation::GslDeallocation() {
    if(p_interp) gsl_spline_free(p_interp);
    if(p_acc) gsl_interp_accel_free(p_acc);
    p_acc = 0;
    p_interp = 0;
    return;
  }

  /**
   * @brief Search for a GSL interpolation function
   *
   * This method searches the supported GSL options table for a
   * given interpolation function as requested by the caller.  If
   * it is not found, an exception will be thrown indicating the
   * erroneous request.
   *
   * @param itype  Type of GSL interpolator to find
   *
   * @return NumericalApproximation::InterpFunctor Pointer to the
   *         GSL spline interpolator construct.
   * @throws Isis::IException::Programmer "Invalid argument.
   *             Unable to find GSL interpolator"
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created
   *          in DataInterp class.
   */
  NumericalApproximation::InterpFunctor NumericalApproximation::GslFunctor(NumericalApproximation::InterpType itype)
  const  {
    FunctorConstIter fItr = p_interpFunctors.find(itype);
    if(fItr == p_interpFunctors.end()) {
      ReportException(IException::Programmer, "GslFunctor()",
                      "Invalid argument. Unable to find GSL interpolator with id = "
                      + IString(NumericalApproximation::InterpType(itype)),
                      _FILEINFO_);
    }
    return (fItr->second);
  }

  /**
   * @brief Checks the status of the GSL interpolation operations
   *
   * This method takes a return status from a GSL call and determines if it is
   * completed successfully.  This implementation currently allows the GSL_DOMAIN
   * error to propagate through sucessfully as the domain can be checked by the
   * caller if they deem this necessary.
   *
   * @param gsl_status Return status of a GSL function call
   * @param src Name of the calling source invoking the check.  This allows more
   *            precise determination of the error.
   * @param line Line of the calling source that invoked the check
   * @throws Isis::IException::Programmer "GSL error occured"
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class. Original name integrityCheck().
   *   @history 2008-11-05 Jeannie Walldren - Renamed.
   *
   */
  void NumericalApproximation::GslIntegrityCheck(int gsl_status, const char *src, int line)
   {
    if(gsl_status != GSL_SUCCESS) {
      if(gsl_status != GSL_EDOM) {
        ReportException(IException::Programmer, "GslIntegrityCheck(int,char,int)",
                        "GslIntegrityCheck(): GSL error occured: "
                        + string(gsl_strerror(gsl_status)), src, line);
      }
    }
    return;
  }

  /**
   *@brief Validates the data set before computing interpolation.
   *
   * This method is called from the ComputeCubicClamped() and
   * ComputeGsl() methods to verify that the data set contains the
   * minimum number of required points and that the components of
   * the vector of domain values are unique.  For all
   * interpolation types other than polynomial-Neville's, the
   * method verifies that the vector of domain values are also
   * sorted in ascending order. For @a CubicNatPeriodic
   * interpolation type, this method verifies that the first and
   * last @a p_y values are equal, i.e. @f$ f(x_0) = f(x_{n-1})
   * @f$.
   *
   *
   * @throws Isis::IException::Programmer "Interpolation requires
   *             a minimim of data points"
   * @throws Isis::IException::Programmer "Invalid data set,
   *             x-values must be unique"
   * @throws Isis::IException::Programmer "Invalid data set,
   *             x-values must be in ascending order" (if
   *             interpolation type is not polynomial-Neville's)
   * @throws Isis::IException::Programmer "First and last points
   *             of the data set must have the same y-value" (if
   *             interpolation type is cubic-periodic)
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  void NumericalApproximation::ValidateDataSet() {
    if((int) Size() < MinPoints()) {
      ReportException(IException::Programmer, "ValidateDataSet()",
                      Name() + " interpolation requires a minimum of "
                      + IString(MinPoints()) + " data points - currently have "
                      + IString((int) Size()),
                      _FILEINFO_);
    }
    for(unsigned int i = 1; i < Size(); i++) {
      // Check for uniqueness -- this applies to all interpolation types
      if(p_x[i-1] == p_x[i]) {
        ReportException(IException::Programmer, "ValidateDataSet()",
                        "Invalid data set, x-values must be unique: \n\t\tp_x["
                        + IString((int) i - 1) + "] = " + IString(p_x[i-1])
                        + " = p_x[" + IString((int) i) + "]",
                        _FILEINFO_);
      }
      if(p_x[i-1] > p_x[i]) {
        // Verify that data set is in ascending order --
        // this does not apply to PolynomialNeville, which appears to get the same results with unsorted data
        if(p_itype != NumericalApproximation::PolynomialNeville) {
          ReportException(IException::Programmer, "ValidateDataSet()",
                          "Invalid data set, x-values must be in ascending order for "
                          + Name() + " interpolation: \n\t\tx["
                          + IString((int) i - 1) + "] = " + IString(p_x[i-1]) + " > x["
                          + IString((int) i) + "] = " + IString(p_x[i]),
                          _FILEINFO_);
        }
      }
    }
    if(p_itype == NumericalApproximation::CubicNatPeriodic) {
      if(p_y[0] != p_y[Size()-1]) {
        ReportException(IException::Programmer, "ValidateDataSet()",
                        "First and last points of the data set must have the same y-value for "
                        + Name() + "interpolation to prevent discontinuity at the boundary",
                        _FILEINFO_);
      }
    }
    p_dataValidated = true;
    return;
  }


  /**
   * Returns whether the passed value is greater than or equal to
   * the domain minimum and less than or equal to the domain
   * maximum.
   *
   * @param a Value to be verified as valid domain value.
   * @return @b bool  True if passed parameter is within the
   *         domain.
   * @throws Isis::IException::Programmer "Unable to compute
   *             domain boundaries"
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  bool NumericalApproximation::InsideDomain(const double a) {
    try {
      if(a + DBL_EPSILON < DomainMinimum()) {
        return false;
      }
      if(a - DBL_EPSILON > DomainMaximum()) {
        return false;
      }
    }
    catch(IException &e) { // catch exception from DomainMinimum(), DomainMaximum()
      throw IException(e,
                       e.errorType(),
                       "InsideDomain() - Unable to compute domain boundaries",
                       _FILEINFO_);
    }
    return true;
  }

  /**
   * Returns whether a GSL interpolation computation of the data
   * set has been performed. This method is only applicable to GSL
   * interpolation types.
   *
   * @return @b bool  True if GSL interpolation has been
   *         computed.
   * @throws Isis::IException::Programmer "Method only valid for
   *             GSL interpolation types"
   * @see ComputeGsl()
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class.
   *   @history 2008-11-05 Jeannie Walldren - Renamed, added
   *          IException.
   */
  bool NumericalApproximation::GslComputed() const {
    if(GslInterpType(p_itype)) return ((p_interp) && (p_acc));
    else
      throw IException(IException::Programmer,
                       "GslComputed() - Method only valid for GSL interpolation types, may not be used for "
                       + Name() + " interpolation",
                       _FILEINFO_);
  }

  /**
   *@brief Computes the GSL interpolation for a set of
   *       (x,y) data points
   *
   * This protected method is called only if the object is
   * assigned a GSL interpolation type and if it has not already
   * been computed on the given data set. It will compute the
   * interval of interpolated range values over the given domain.
   * A copy of this data is maintained in the object so the data
   * points do not need to be interpolated on each evaluation of a
   * point unless the data set changes.
   *
   * @throws Isis::IException::Programmer "Unable to compute GSL
   *                            interpolation"
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class.
   *   @history 2008-11-05 Jeannie Walldren - Modified to allow
   *            for computation of interpolation types other than
   *            those supported by GSL.
   */
  void NumericalApproximation::ComputeGsl() {
    try {
      if(GslComputed()) return;
      if(!p_dataValidated) ValidateDataSet();
      GslAllocation(Size());
      GslIntegrityCheck(gsl_spline_init(p_interp, &p_x[0], &p_y[0], Size()), _FILEINFO_);
      return;
    }
    catch(IException &e) { // catch exception from ValidateDataSet(), GslIntegrityCheck()
      throw IException(e,
                       e.errorType(),
                       "ComputeGsl() - Unable to compute GSL interpolation",
                       _FILEINFO_);
    }
  }

  /**
   * @brief Computes the cubic clamped interpolation for a
   * set of (x,y) data points, given the first derivatives of the
   * endpoints of the data set.
   *
   * This protected method is called only if the object is
   * assigned a cubic clamped interpolation type and if it has not
   * already been computed for the given data set.  It calculates
   * the second derivatives for each @e p_x value of the known
   * data set and stores these values in
   * @a p_clampedSecondDerivs so that the
   * EvaluateCubicClamped() method may be called to interpolate
   * the set using clamped boundary conditions, if possible. This
   * method must be called when all the data points have been
   * added to the object for the data set and after
   * SetCubicClampedEndptDeriv() has been called. If the endpoint
   * derivatives are greater than or equal to 1 x
   * 10<sup>30</sup>, the routine is signaled to set the
   * corresponding boundary condition for a natural spline, with
   * zero second derivative on that boundary.
   *
   * @throws Issis::IException::Programmer "Must use
   *              SetCubicClampedEndptDeriv() before computing
   *              cubic spline with clamped boundary"
   * @throws Issis::IException::Programmer "Unable to compute
   *              cubic clamped interpolation"
   * @see SetCubicClampedEndptDeriv()
   * @see EvaluateCubicClamped()
   * @internal
   *   @history 1999-08-11 K Teal Thompson - Original version in
   *            Isis2.
   *   @history 2007-02-20 Janet Barrett - Imported to Isis3 in
   *            NumericalMethods class. Original name r8spline().
   *   @history 2008-11-05 Jeannie Walldren - Renamed and modified
   *            input parameters.
   */
  void NumericalApproximation::ComputeCubicClamped() {
    // This method was derived from an algorithm in the text
    // Numerical Recipes in C: The Art of Scientific Computing
    // Section 3.3 by Flannery, Press, Teukolsky, and Vetterling

    if(!p_dataValidated) {
      try {
        ValidateDataSet();
      }
      catch(IException &e) {  // catch exception from ValidateDataSet()
        throw IException(e,
                         e.errorType(),
                         "ComputeCubicClamped() - Unable to compute cubic clamped interpolation",
                         _FILEINFO_);
      }
    }
    if(!p_clampedEndptsSet) {
      ReportException(IException::Programmer, "ComputeCubicClamped()",
                      "Must set endpoint derivative values after adding data in order to compute cubic spline with clamped boundary conditions",
                      _FILEINFO_);
    }
    int n = Size();
    p_clampedSecondDerivs.resize(n);
    double u[n];
    double p, sig, qn, un;

    if(p_clampedDerivFirstPt > 0.99e30) {
      p_clampedSecondDerivs[0] = 0.0;//natural boundary conditions are used if deriv of first value is greater than 10^30
      u[0] = 0.0;
    }
    else {
      p_clampedSecondDerivs[0] = -0.5;// clamped conditions are used
      u[0] = (3.0 / (p_x[1] - p_x[0])) * ((p_y[1] - p_y[0]) / (p_x[1] - p_x[0]) - p_clampedDerivFirstPt);
    }
    for(int i = 1; i < n - 1; i++) { // decomposition loop of the tridiagonal algorithm
      sig = (p_x[i] - p_x[i-1]) / (p_x[i+1] - p_x[i-1]);
      p = sig * p_clampedSecondDerivs[i-1] + 2.0;
      p_clampedSecondDerivs[i] = (sig - 1.0) / p;
      u[i] = (6.0 * ((p_y[i+1] - p_y[i]) / (p_x[i+1] - p_x[i]) - (p_y[i] - p_y[i-1]) /
                     (p_x[i] - p_x[i-1])) / (p_x[i+1] - p_x[i-1]) - sig * u[i-1]) / p;
    }
    if(p_clampedDerivLastPt > 0.99e30) { // upper boundary is natural
      qn = 0.0;
      un = 0.0;
    }
    else {// upper boundary is clamped
      qn = 0.5;
      un = (3.0 / (p_x[n-1] - p_x[n-2])) * (p_clampedDerivLastPt - (p_y[n-1] - p_y[n-2]) /
                                            (p_x[n-1] - p_x[n-2]));
    }
    p_clampedSecondDerivs[n-1] = (un - qn * u[n-2]) / (qn * p_clampedSecondDerivs[n-2] + 1.0);
    for(int i = n - 2; i >= 0; i--) { // backsubstitution loop of the tridiagonal algorithm
      p_clampedSecondDerivs[i] = p_clampedSecondDerivs[i] * p_clampedSecondDerivs[i+1] + u[i];
    }
    p_clampedComputed = true;
    return;
  }

  /**
   * @brief Returns the domain value at which to evaluate.
   *
   * This protected method is called by Evaluate() if @a a
   * falls outside of the domain of @e p_x values in the data set.
   * The return value is determined by the
   * NumericalApproximation::ExtrapType.  If it is @a ThrowError, an
   * error is thrown indicating that @a a is out of the domain. If
   * it is @a NearestEndpoint, then the nearest domain boundary value
   * is returned.  Otherwise, i.e. @a Extrapolate, @a a is returned
   * as long as the NumericalApproximation::InterpType is not GSL
   * or cubic neighborhood.
   *
   * @param a Value passed into Evaluate() that falls outside of
   *          domain.
   * @param etype NumericalApproximation::ExtrapType enum
   *              value indicates how to evaluate if @a a
   *              falls outside of the domain.   (Default:
   *              @a ThrowError)
   * @return @b double Value returned to Evaluate() to be
   *         extrapolated.
   * @throws Isis::IException::Programmer "Invalid argument. Value
   *             entered is outside of domain
   * @throws Isis::IException::Programmer "Invalid argument.
   *             Cannot extrapolate for interpolation type" (GSL
   *             or @a CubicNeighborhood)
   * @see Evaluate()
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class.
   *   @history 2008-11-05 Jeannie Walldren - Modified to allow for
   *            computation of interpolation types other than those
   *            supported by GSL.
   *   @history 2008-12-18 Jeannie Walldren - Added address
   *            operator (&) to input variables of type
   *            NumericalApproximation::ExtrapType.
   */
  double NumericalApproximation::ValueToExtrapolate(const double a, const ExtrapType &etype) {
    if(etype == NumericalApproximation::ThrowError) {
      ReportException(IException::Programmer, "Evaluate()",
                      "Invalid argument. Value entered, a = "
                      + IString(a) + ", is outside of domain = ["
                      + IString(DomainMinimum()) + ", "
                      + IString(DomainMaximum()) + "]",
                      _FILEINFO_);
    }
    if(etype == NumericalApproximation::NearestEndpoint) {
      if(a + DBL_EPSILON < DomainMinimum()) {
        return DomainMinimum();
      }
      else {
        return DomainMaximum(); // (a > DomainMaximum())
      }
    }
    else {  // gsl interpolations and CubicNeighborhood cannot extrapolate
      if(GslInterpType(p_itype)
          || p_itype == NumericalApproximation::CubicNeighborhood) {
        ReportException(IException::Programmer, "Evaluate()",
                        "Invalid argument. Cannot extrapolate for type "
                        + Name() + ", must choose to throw error or return nearest neighbor",
                        _FILEINFO_);
      }
      return a;
    }
  }

  /**
   * @brief Performs cubic spline interpolation for a neighborhood
   *        about @a a.
   *
   * This is a protected method called by Evaluate() if the
   * NumericalApproximation::InterpType is
   * @a CubicNeighborhood. It uses an algorithm that
   * is adapted from the IDL interpol.pro application using the
   * "/spline" keyword on an irregular grid.  This type of cubic
   * spline fits a natural cubic spline to the 4-point
   * neighborhood of known data points surrounding @a a. For
   * example, suppose {@e x<sub>0</sub>, @e x<sub>1</sub>, ...,
   * @e x<sub>n</sub>} is the array of known domain values
   * in the data set and @f$ x_i \leq a < x_{i+1}@f$ for some
   * @e i such that @f$ 0 \leq i \leq n @f$, then
   * @e f(@a a) is evaluated by interpolating the natural
   * cubic spline consisting of the data set
   * {@e x<sub>i-1</sub>, @e x<sub>i</sub>,
   * @e x<sub>i+1</sub>, @e x<sub>i+2</sub>} at
   * @a a.
   *
   * @b Note: If the given value, @a a, falls outside of the
   * domain, then @e f evaluated at the nearest domain boundary
   * is returned.
   *
   * @param a     Domain value from which to interpolate a
   *              corresponding range value.
   * @return @b double Result of interpolated value of f(a).
   * @throws Isis::IException::Programmer "Unable to evaluate
   *             cubic neighborhood interpolation at a"
   * @see Evaluate()
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  double NumericalApproximation::EvaluateCubicNeighborhood(const double a) {
    try {
      int s = 0, s_old, s0;
      vector <double> x0(4), y0(4);
      NumericalApproximation spline(NumericalApproximation::CubicNatural);
      for(unsigned int n = 0; n < Size(); n++) {
        if(p_x[n] < a) s = n;
        else break;
      }
      if(s < 1) s = 1;
      else if(s > (int) Size() - 3) s = Size() - 3;
      s_old = -1;
      s0 = s - 1;
      if(s_old != s0) {
        for(int n = 0; n < 4; n++) {
          x0[n] = p_x[n+s0];
          y0[n] = p_y[n+s0];
          spline.AddData(x0[n], y0[n]);
        }
        s_old = s0;
      }
      // use nearest endpoint extrapolation method for neighborhood spline
      // since CubicNatural can do no other
      return spline.Evaluate(a, NumericalApproximation::NearestEndpoint);
    }
    catch(IException &e) { // catch exception from Constructor, ComputeGsl(), Evaluate()
      throw IException(e,
                       e.errorType(),
                       "EvaluateCubicNeighborhood() - Unable to evaluate cubic neighborhood interpolation at a = "
                       + IString(a),
                       _FILEINFO_);
    }
  }

  /**
   * @brief Performs cubic spline interpolations for neighborhoods
   *        about each value of @a a.
   *
   * This is a protected method called by Evaluate() if the
   * NumericalApproximation::InterpType is
   * @a CubicNeighborhood. It uses an algorithm that
   * is adapted from the IDL interpol.pro application using the
   * "/spline" keyword on an irregular grid.
   * For each component of @a a, this method fits a natural
   * cubic spline using the 4-point neighborhood of known data
   * points surrounding that component. For example, suppose
   * {@e x<sub>0</sub>, @e x<sub>1</sub>, ...,
   * @e x<sub>n</sub>} is the array of known domain values
   * in the data set, then for
   * each component of @a a, @e a<sub>k</sub>, in the domain,
   * there is an @e i such that @f$ 0 \leq i \leq n
   * @f$ and @f$ x_i \leq a_k < x_{i+1}@f$. Then,
   * @e f(@a a<sub>k</sub>) is evaluated by interpolating the
   * natural cubic spline consisting of the data set
   * {@e x<sub>i-1</sub>, @e x<sub>i</sub>,
   * @e x<sub>i+1</sub>, @e x<sub>i+2</sub>} at
   * @e a<sub>k</sub>.
   *
   * @b Note: If any given value, @a a<sub>i</sub>, falls
   * outside of the domain, then @e f is evaluated at the nearest
   * domain boundary.
   *
   * @param a Vector of domain values from which to interpolate a
   *              vector of corresponding range values
   * @param etype NumericalApproximation::ExtrapType enum
   *              value indicates how to evaluate if @a a
   *              falls outside of the domain.   (Default:
   *              @a ThrowError)
   * @return @b vector @b \<double\> Result of interpolated value
   *         of f at each value of a.
   * @throws Isis::IException::Programmer "Unable to evaluate
   *             cubic neighborhood interpolation at the values in
   *             the vector, a"
   * @see Evaluate()
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   *   @history 2008-12-18 Jeannie Walldren - Added address
   *            operator (&) to input variables of type vector and
   *            NumericalApproximation::ExtrapType.
   */
  vector <double> NumericalApproximation::EvaluateCubicNeighborhood(const vector <double> &a,
      const NumericalApproximation::ExtrapType &etype) {
    vector <double> result(a.size());
    int s_old, s0;
    vector <double> x0(4), y0(4);
    vector <int> s;
    s.clear();
    s.resize(a.size());
    for(unsigned int i = 0; i < a.size(); i++) {
      for(unsigned int n = 0; n < Size(); n++) {
        if(p_x[n] < a[i]) {
          s[i] = n;
        }
      }
      if(s[i] < 1) {
        s[i] = 1;
      }
      if(s[i] > ((int) Size()) - 3) {
        s[i] = Size() - 3;
      }
    }
    s_old = -1;
    try {
      NumericalApproximation spline(NumericalApproximation::CubicNatural);
      for(unsigned int i = 0; i < a.size(); i++) {
        s0 = s[i] - 1;
        if(s_old != s0) {
          spline.Reset();
          for(int n = 0; n < 4; n++) {
            x0[n] = p_x[n+s0];
            y0[n] = p_y[n+s0];
            spline.AddData(x0[n], y0[n]);
          }
          s_old = s0;
        }
        double a0;
        // checks whether this value is in domain of the main spline
        if(InsideDomain(a[i]))
          a0 = a[i];
        else a0 = ValueToExtrapolate(a[i], etype);
        // since neighborhood spline is CubicNatural, the only extrapolation possible is NearestEndpoint
        result[i] = spline.Evaluate(a0, NumericalApproximation::NearestEndpoint);
      }
      return result;
    }
    catch(IException &e) { // catch exception from Constructor, ComputeGsl(), Evaluate()
      throw IException(e,
                       e.errorType(),
                       "EvaluateCubicNeighborhood() - Unable to evaluate the function at the given vector of points using cubic neighborhood interpolation",
                       _FILEINFO_);
    }
  }

  /**
   * @brief Performs cubic spline interpolation with
   * clamped boundary conditions, if possible.
   *
   * This is a protected method called by Evaluate() if the
   * NumericalApproximation::InterpType is
   * @a CubicClamped and SetCubicClampedEndptDeriv() has
   * been called. It uses the second derivative vector,
   * @a p_clampedSecondDerivs, to interpolate the value for
   * @e f(@a a) using a clamped cubic spline formula,
   *
   * @b Note: If the given value, @a a, falls outside of the
   * domain, then extrapolation is attempted and the result is
   * accurate only if @a a is near enough to the domain
   * boundary.
   *
   *
   * @param a Domain value from which to interpolate a
   *          corresponding range value
   * @return @b double  Value returned from interpolation or
   *         extrapolation.
   * @see Evaluate()
   * @see ComputeCubicClamped()
   * @see SetCubicClampedEndptDeriv()
   * @internal
   *   @history 1999-08-11 K Teal Thompson - Original version in
   *            Isis2.
   *   @history 2007-02-20 Janet Barrett - Imported to Isis3 in
   *            NumericalMethods class. Original name r8splint().
   *   @history 2008-11-05 Jeannie Walldren - Renamed and modified
   *            input parameters, removed IException and replaced it
   *            with ValidateDataSet() method.
   *
   */
  double NumericalApproximation::EvaluateCubicClamped(double a) {
    // This method was derived from an algorithm in the text
    // Numerical Recipes in C: The Art of Scientific Computing
    // Section 3.3 by Flannery, Press, Teukolsky, and Vetterling
    int n = Size();
    double result = 0;

    int k;
    int k_Lo;
    int k_Hi;
    double h;
    double A;
    double B;

    k_Lo = 0;
    k_Hi = n - 1;
    while(k_Hi - k_Lo > 1) {
      k = (k_Hi + k_Lo) / 2;
      if(p_x[k] > a) {
        k_Hi = k;
      }
      else {
        k_Lo = k;
      }
    }

    h = p_x[k_Hi] - p_x[k_Lo];
    A = (p_x[k_Hi] - a) / h;
    B = (a - p_x[k_Lo]) / h;
    result = A * p_y[k_Lo] + B * p_y[k_Hi] + ((pow(A, 3.0) - A) *
             p_clampedSecondDerivs[k_Lo] + (pow(B, 3.0) - B) * p_clampedSecondDerivs[k_Hi]) * pow(h, 2.0) / 6.0;
    return result;
  }

  /**
   * @brief Performs interpolation using the Hermite cubic
   *        polynomial.
   *
   * This is a protected method called by Evaluate() if the
   * NumericalApproximation::InterpType is
   * @a CubicHermite. It returns an approximate value for
   * @e f(@a a) by using the Hermite cubic polynomial, which uses
   * Lagrange coefficient polynomials. The data points and each
   * corresponding first derivative should have been already
   * added.
   *
   * @param a Domain value from which to interpolate a
   *          corresponding range value
   * @return @b double  Value returned from interpolation or
   *         extrapolation
   * @throw Isis::IException::User "Invalid arguments. The size of
   *        the first derivative vector does not match the number
   *        of (x,y) data points."
   * @see http://mathworld.wolfram.com/HermitesInterpolatingPolynomial.html
   * @see http://en.wikipedia.org/wiki/Lagrange_Polynomial
   * @see Evaluate()
   * @see AddCubicHermiteDeriv()
   * @internal
   *   @history 2009-06-10 Jeannie Walldren - Original version.
   *
   */
  double NumericalApproximation::EvaluateCubicHermite(const double a) {
    //  algorithm was found at en.wikipedia.org/wiki/Cubic_Hermite_spline
    //  it seems to produce same answers, as the NumericalAnalysis book

    if(p_fprimeOfx.size() != Size()) {
      ReportException(IException::User, "EvaluateCubicHermite()",
                      "Invalid arguments. The size of the first derivative vector does not match the number of (x,y) data points.",
                      _FILEINFO_);
    }
    // find the interval in which "a" exists
    int lowerIndex = FindIntervalLowerIndex(a);

    // we know that "a" is within the domain since this is verified in
    // Evaluate() before this method is called, thus n <= Size()
    if(a == p_x[lowerIndex]) {
      return p_y[lowerIndex];
    }
    if(a == p_x[lowerIndex+1]) {
      return p_y[lowerIndex+1];
    }

    double x0, x1, y0, y1, m0, m1;
    // a is contained within the interval (x0,x1)
    x0 = p_x[lowerIndex];
    x1 = p_x[lowerIndex+1];
    // the corresponding known y-values for x0 and x1
    y0 = p_y[lowerIndex];
    y1 = p_y[lowerIndex+1];
    // the corresponding known tangents (slopes) at (x0,y0) and (x1,y1)
    m0 = p_fprimeOfx[lowerIndex];
    m1 = p_fprimeOfx[lowerIndex+1];


    //  following algorithm found at en.wikipedia.org/wiki/Cubic_Hermite_spline
    //  seems to produce same answers, is it faster?

    double h, t;
    h = x1 - x0;
    t = (a - x0) / h;
    return (2 * t * t * t - 3 * t * t + 1) * y0 + (t * t * t - 2 * t * t + t) * h * m0 + (-2 * t * t * t + 3 * t * t) * y1 + (t * t * t - t * t) * h * m1;
  }

  /**
   * Find the index of the x-value in the data set that is just
   * below the input value, a. This method is used by
   * EvaluateCubicHermite(), EvaluateCubFirstDeriv() and
   * EvaluateCubicHermiteSecDeriv() to determine in which interval
   * of x-values a lies. It returns the index of the lower
   * endpoint of the interval. If a is below the domain minimum,
   * the method returns 0 as the lower index.  If a is above the
   * domain maximum, it returns the second to last index of the
   * data set, Size()-2, as the lower index.
   *
   * @param a Domain value around which the interval lies
   * @return @b int Index of the x-value that is the lower
   *         endpoint of the interval of data points that
   *         surrounds a. Returns 0 if a is below domain min and
   *         Size()-2 if a is above domain max.
   * @internal
   *   @history 2009-06-10 Jeannie Walldren - Original Version
   *
   */
  int NumericalApproximation::FindIntervalLowerIndex(const double a) {
    if(InsideDomain(a)) {
      // find the interval in which "a" exists
      std::vector<double>::iterator pos;
      // find position in vector that is greater than or equal to "a"
      pos = upper_bound(p_x.begin(), p_x.end(), a);
      int upperIndex = 0;
      if(pos != p_x.end()) {
        upperIndex = distance(p_x.begin(), pos);
      }
      else {
        upperIndex = Size() - 1;
      }
      return upperIndex - 1;
    }
    else if((a + DBL_EPSILON) < DomainMinimum()) {
      return 0;
    }
    else {
      return Size() - 2;
    }
  }


  /**
   * @brief Performs polynomial interpolation using Neville's
   * algorithm.
   *
   * This is a protected method called by Evaluate() if the
   * NumericalApproximation::InterpType is
   * @a PolynomialNeville. It uses Neville's
   * algorithm for Lagrange polynomials. It returns a value
   * @e f(@a a) and sets the error estimate
   * @a p_polyNevError. After this method is called, the user
   * may access this error estimate by calling
   * PolynomialNevilleErrorEstimate().
   *
   * @b Note: If the given value, @a a, falls outside of the
   * domain, then extrapolation is attempted and the result is
   * accurate only if @a a is near enough to the domain
   * boundary.
   *
   * @param a Domain value from which to interpolate a
   *          corresponding range value
   * @return @b double  Value returned from interpolation or
   *         extrapolation
   * @see http://mathworld.wolfram.com/NevillesAlgorithm.html
   * @see Evaluate()
   * @see PolynomialNevilleErrorEstimate()
   * @internal
   *   @history 1999-08-11 K Teal Thompson - Original version in
   *            Isis2.
   *   @history 2007-02-20 Janet Barrett - Imported to Isis3 in
   *            NumericalMethods class. Original name r8polint()
   *   @history 2008-11-05 Jeannie Walldren - Renamed and modified
   *            input/output parameters, removed IException and
   *            replaced it with ValidateDataSet() method.
   *
   */
  double NumericalApproximation::EvaluatePolynomialNeville(const double a) {
    // This method was derived from an algorithm in the text
    // Numerical Recipes in C: The Art of Scientific Computing
    // Section 3.1 by Flannery, Press, Teukolsky, and Vetterling
    int n = Size();
    double y;

    int ns;
    double den, dif, dift, c[n], d[n], ho, hp, w;
    double *err = 0;
    ns = 1;
    dif = fabs(a - p_x[0]);
    for(int i = 0; i < n; i++) { // Get the index ns of the closest table entry
      dift = fabs(a - p_x[i]);
      if(dift < dif) {
        ns = i + 1;
        dif = dift;
      }
      c[i] = p_y[i];  // initialize c and d
      d[i] = p_y[i];
    }
    ns = ns - 1;
    y = p_y[ns]; // initial approximation for y
    for(int m = 1; m < n; m++) {
      for(int i = 1; i <= n - m; i++) { // loop over c and d and update them
        ho = p_x[i-1] - a;
        hp = p_x[i+m-1] - a;
        w = c[i] - d[i-1];
        den = ho - hp;
        den = w / den;
        d[i-1] = hp * den; // update c and d
        c[i-1] = ho * den;
      }
      if(2 * ns < n - m) { // After each column in the tableau is completed, we decide
        err = &c[ns];   // which correction, c or d, we want to add to our accumulating
      }                 // value of y, i.e., which path to take through the tableau|
      else {            // forking up or down. We do this in such a way as to take the
        ns = ns - 1;    // most "straight line" route through the tableau to its apex,
        err = &d[ns];   // updating ns accordingly to keep track of where we are.  This
      }                 // route keeps the partial approximations centered (insofar as possible)
      y = y + *err;     // on the target x. The last err added is thus the error indication.
    }
    p_polyNevError.push_back(*err);
    return y;
  }

  /**
   * Evaluates data set in order to have enough data points to
   * approximate the function to be integrated.
   *
   * @param a Lower bound of interval.
   * @param b Upper bound of interval.
   * @param n Number of points used in Newton-Cotes formula.
   * @return @b vector @b \<double\> Array of y values to be used
   *         in numerical integration
   * @throws Isis::IException::Programmer (When a > b) "Invalid
   *        interval entered"
   * @throws Isis::IException::Programmer "Invalid arguments.
   *        Interval entered is not contained within domain"
   * @throws Isis::IException::Programmer "Unable to evaluate the
   *             data set for integration"
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original version.
   */
  vector <double> NumericalApproximation::EvaluateForIntegration(const double a, const double b, const unsigned int n) {
    if(a > b) {
      ReportException(IException::Programmer, "EvaluatedForIntegration()",
                      "Invalid interval entered: [a,b] = ["
                      + IString(a) + ", " + IString(b) + "]",
                      _FILEINFO_);
    }
    if(!InsideDomain(a) || !InsideDomain(b)) {
      ReportException(IException::Programmer, "EvaluateForIntegration()",
                      "Invalid arguments. Interval entered ["
                      + IString(a) + ", " + IString(b)
                      + "] is not contained within domain ["
                      + IString(DomainMinimum()) + ", "
                      + IString(DomainMaximum()) + "]",
                      _FILEINFO_);
    }
    vector <double> f;
    //need total number of segments to be divisible by n-1
    // This way we can use the formula for each interval: 0 to n, n to 2n, 2n to 3n, etc.
    // Notice interval endpoints will overlap for these composite formulas
    int xSegments = Size() - 1;
    while(xSegments % (n - 1) != 0) {
      xSegments++;
    }
    // x must be sorted and unique
    double xMin = a;
    double xMax = b;
    //Uniform step size.
    double h = (xMax - xMin) / xSegments;
    //Compute the interpolates using spline.
    try {
      for(double i = 0; i < (xSegments + 1); i++) {
        double xi = h * i + xMin;
        f.push_back(Evaluate(xi));  // validate data set here, allow default ThrowError extrap type
      }
      f.push_back(h);
      return f;
    }
    catch(IException &e) { // catch exception from Evaluate()
      throw IException(e,
                       e.errorType(),
                       "EvaluateForIntegration() - Unable to evaluate the data set for integration",
                       _FILEINFO_);
    }
  }// for integration using composite of spline (creates evenly spaced points)

  /**
   * @brief Generalized error report generator
   *
   * This method is used throughout this class to standardize error reporting as a
   * convenience to its implementor.
   *
   * @param type  Type of @b Isis::IException
   * @param methodName Name of method where exception originated
   * @param message Error context string provided by the caller
   * @param filesrc Name of the file the error occured in.
   * @param lineno Line number of the calling source where the error occured.
   * @throws Isis::IException::errType equal to
   *             @a type and error message equal to
   *             (@a methodName + " - " + @a message)
   * @internal
   *   @history 2006-06-14 Kris Becker - Original version created in
   *            DataInterp class.
   *   @history 2008-11-05 Jeannie Walldren - Modified to take
   *            IException::errType as input parameter.
   */
  void NumericalApproximation::ReportException(IException::ErrorType type, const string &methodName, const string &message,
      const char *filesrc, int lineno)
  const  {
    string msg = methodName + " - " + message;
    throw IException(type, msg.c_str(), filesrc, lineno);
    return;
  }

}
