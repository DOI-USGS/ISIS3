#include "SurfaceModel.h"

namespace Isis {
  //! Constructor
  SurfaceModel::SurfaceModel () {
    p_poly2d = new PolynomialBivariate(2);
    p_lsq = new LeastSquares(*p_poly2d);
  }

  //! Destructor
  SurfaceModel::~SurfaceModel() {
    delete p_lsq;
    delete p_poly2d;
  }

  //! Add a single (x,y,z) triplet to the list of knowns.
  //! After all knowns are added invoke the Solve method
  void SurfaceModel::AddTriplet (const double x, const double y, const double z) {
    std::vector<double> vec;
    vec.push_back(x);
    vec.push_back(y);
    p_lsq->AddKnown(vec,z);
  }

  //! Add an array of (x,y,z) triplet to the list of knowns 
  //! After all knowns are added invoke the Solve method
  void SurfaceModel::AddTriplets (const double *x, const double *y, 
                                  const double *z, const int n) {
    for (int i=0; i<n; i++) {
      AddTriplet(x[i],y[i],z[i]);
    }
  }

  //! Add an array of (x,y,z) triplet to the list of knowns 
  //! After all knowns are added invoke the Solve method
  void SurfaceModel::AddTriplets (const std::vector<double> &x, 
                    const std::vector<double> &y, 
                    const std::vector<double> &z) {
    for (int i=0; i<(int)x.size(); i++) {
      AddTriplet(x[i],y[i],z[i]);
    }
  }

  //! Fit a surface to the input triplets
  void SurfaceModel::Solve() {
    p_lsq->Solve();
  }

  //! Evaluate at x,y to compute z. This is available after the
  //! Solve method is invoked
  double SurfaceModel::Evaluate (const double x, const double y) {
    std::vector<double> vec;
    vec.push_back(x);
    vec.push_back(y);
    return p_lsq->Evaluate(vec);
  }

  /** After invoking Solve, a coordinate (x,y) at a local minimum (or
   * maximum) of the surface model can be computed using this method.
   * 
   * @return A zero if successful, otherwise, the surface is a plane
   * and has no min/max
   */
  int SurfaceModel::MinMax(double &x, double &y) {
    /* For a PolynomialBivariate of 2nd degree, the partial derivatives are two lines:
    * 
    * dz/dx = b + 2dx + ey
    * dz/dy = c + ex + 2fy
    * 
    * We will have a local min/max where dz/dx and dz/dy = 0.
    * Solve using that condition using linear algebra yields:
    * 
    * xlocal = (ce - 2bf) / (4df - ee)
    * ylocal = (be - 2cd) / (4df - ee)
    */

     // Get coefficients
     double b = p_poly2d->Coefficient(1);
     double c = p_poly2d->Coefficient(2);
     double d = p_poly2d->Coefficient(3);
     double e = p_poly2d->Coefficient(4);
     double f = p_poly2d->Coefficient(5);

     // Compute the determinant
     double det = 4.0*d*f - e*e;
     if (det == 0.0) return 1;

     // Compute local min/max
     x = (c*e - 2.0*b*f) / det;
     y = (b*e - 2.0*c*d) / det;
     return 0;
  }
}
