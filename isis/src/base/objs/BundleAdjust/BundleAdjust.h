#ifndef BundleAdjust_h
#define BundleAdjust_h
/**
 * @file
 * $Revision: 1.24 $
 * $Date: 2010/03/27 06:23:42 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 *
 * @author 2006-05-30 Jeff Anderson, Debbie A. Cook, and Tracie Sucharski
 *
 * @internal
 *   @history 2005-05-30 Jeff Anderson, Debbie A. Cook & Tracie Sucharski Original version
 *   @history 2007-05-29 Debbie A. Cook  Added new method IterationSummary and
 *                       changed points on held images to held instead of ground
 *   @history 2007-07-12 Debbie A. Cook  Fixed bug in iteration statistics calculations in the
 *                       case of a single control point that was causing a divide by zero error
 *   @history 2007-08-25 Debbie A. Cook Added methods and members to support instrument position solution
 *   @history 2007-09-17 Debbie A. Cook Added ability to process in observation mode for Lunar Orbiter
 *   @history 2007-11-17 Debbie A. Cook Added method SetSolution Method.
 *   @history 2007-12-21 Debbie A. Cook Added member p_Degree and methods p_solveCamDegree and ckDegree
 *   @history 2008-01-11 Debbie A. Cook Added observation mode functionality for spacecraft position
 *                       and upgraded ObservationNumber methods for compatability
 *   @history 2008-01-14 Debbie A. Cook Added code to solve for local radii
 *   @history 2008-04-18 Debbie A. Cook Added progress for ControlNet
 *   @history 2008-06-18 Christopher Austin Fixed ifndef
 *   @history 2008-11-07 Tracie Sucharski, Added bool to constructors to
 *                          indicate whether to print iteration summary info
 *                          to the session log. This was needed for qtie which
 *                          has no session log.
 *   @history 2008-11-22 Debbie A. Cook Added code to wrap longitude to keep it in [0.,360.]
 *   @history 2008-11-22 Debbie A. Cook Added new call to get timeScale and set for the observation along with basetime
 *   @history 2008-11-26 Debbie A. Cook Added check to ApplyHeldList for Ignored points and measures
 *   @history 2009-01-08 Debbie A. Cook Revised AddPartials and PointPartial to avoid using the camera methods
 *                          to map a body-fixed vector to the camera because they compute a new time for line
 *                          scan cameras based on the lat/lon/radius and the new time is used to retrieve Spice.
 *                          The updated software uses the Spice at the time of the measurement.
 *   @history 2009-02-15 Debbie A. Cook Corrected focal length to include its sign and removed obsolete calls to X/Y
 *                          direction methods.  Also modified PointPartial to use lat/lon/radius from the point
 *                          instead of the camera.
 *   @history 2009-08-13 Debbie A. Cook Corrected calculations of cudx and cudy so that they use the signed focal length
 *                          also
 *   @history 2009-10-14 Debbie A. Cook Modified AddPartials method to use new CameraGroundMap method, GetXY
 *   @history 2009-10-30 Debbie A. Cook Improved error message in AddPartials
 *   @history 2009-12-14 Debbie A. Cook Updated SpicePosition enumerated partial type constants
 *   @history 2010-03-19 Debbie A. Cook Moved partials to GroundMap classes to support Radar sensors and modified
 *                          argument list for GroundMap method ComputeXY since it now returns cudx and cudy
 */

#include "ControlNet.h"
#include "SerialNumberList.h"
#include "ObservationNumberList.h"
#include "Camera.h"
#include "Statistics.h"
#include "SpicePosition.h"
#include "Progress.h"
#include "CameraGroundMap.h"

namespace Isis {
  class LeastSquares;
  class BasisFunction;

  class BundleAdjust {
    public:
      BundleAdjust(const std::string &cnetFile, const std::string &cubeList,
                   bool printSummary = true);
      BundleAdjust(const std::string &cnetFile, const std::string &cubeList,
                   const std::string &heldList, bool printSummary = true);
      BundleAdjust(Isis::ControlNet &cnet, Isis::SerialNumberList &snlist,
                   bool printSummary = true);
      BundleAdjust(Isis::ControlNet &cnet, Isis::SerialNumberList &snlist,
                   Isis::SerialNumberList &heldsnlist, bool printSummary = true);
      ~BundleAdjust();

      double Solve(double tol, int maxIterations);

      Isis::ControlNet *ControlNet() {
        return p_cnet;
      };

      Isis::SerialNumberList *SerialNumberList() {
        return p_snlist;
      };
      int Images() const;
      int Observations() const;
      std::string Filename(int index);
      Table Cmatrix(int index);
      Table SpVector(int index);

      void SetSolveTwist(bool solve);
      void SetSolveRadii(bool solve);

      enum CmatrixSolveType {
        None,
        AnglesOnly,
        AnglesVelocity,
        AnglesVelocityAcceleration,
        All
      };

      enum SpacecraftPositionSolveType {
        Nothing,
        PositionOnly,
        PositionVelocity,
        PositionVelocityAcceleration
      };

      void SetSolveCmatrix(CmatrixSolveType type);
      void SetSolveSpacecraftPosition(SpacecraftPositionSolveType type);

      //! Set the degree of the polynomial to fit to the camera angles
      void SetCkDegree(int degree) {
        p_ckDegree = degree;
      };

      //! Set the degree of the polynomial to adjust in the solution
      void SetSolveCamDegree(int degree) {
        p_solveCamDegree = degree;
      };

      int BasisColumns() const;

      double Error() const {
        return p_error;
      };
      double Iteration() const {
        return p_iteration;
      };

      int HeldPoints() const {
        return p_heldPoints;
      };
      int IgnoredPoints() const {
        return p_ignoredPoints;
      };
      int GroundPoints() const {
        return p_groundPoints;
      };
      void SetObservationMode(bool observationMode);

      //! Set the solution method to use for solving the matrix
      void SetSolutionMethod(std::string solutionMethod) {
        p_solutionMethod = solutionMethod;
      };

    private:
      void Init(Progress *progress = 0);

      void ComputeNumberPartials();

      void AddPartials(LeastSquares &lsq,
                       int point);
      void Update(BasisFunction &basis);

      int PointIndex(int i) const;

      int ImageIndex(int i) const;

      void CheckHeldList();
      void ApplyHeldList();

      Isis::ControlNet *p_cnet;
      Isis::SerialNumberList *p_snlist;
      Isis::SerialNumberList *p_heldsnlist;
      Isis::ObservationNumberList *p_onlist;

      double p_error;
      int p_iteration;
      bool p_printSummary;

      int p_numImagePartials;
      int p_numPointPartials;

      bool p_solveTwist;
      bool p_solveRadii;
      bool p_observationMode;
      CmatrixSolveType p_cmatrixSolveType;
      SpacecraftPositionSolveType p_spacecraftPositionSolveType;

      int p_heldPoints;
      int p_groundPoints;
      int p_ignoredPoints;
      int p_heldImages;
      int p_heldObservations;
      std::vector<int> p_pointIndexMap;
      std::vector<int> p_imageIndexMap;
      bool p_cleanUp;

      void IterationSummary(double avErr, double sigmaXY,
                            double sigmaHat, double sigmaX, double sigmaY);
      Statistics p_statx;
      Statistics p_staty;
      std::string p_solutionMethod;
      int p_ckDegree;
      int p_solveCamDegree;
      int p_numberCameraCoefSolved;  //!< The number of camera angle coefficients in the solution
  };
};

#endif

