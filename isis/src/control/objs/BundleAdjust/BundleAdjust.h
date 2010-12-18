#ifndef BundleAdjust_h
#define BundleAdjust_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2009/10/15 01:35:17 $
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
 *   @history 2007-12-21 Debbie A. Cook Added member p_Degree and methods m_nsolveCamDegree and ckDegree
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
 *   @history 2010-06-18 Debbie A. Cook Added p_cnetFile as member since it was taken out of ControlNet
 *   @history 2010-07-09 Ken Edmundson Added Folding in solution method (SPECIALK), error propogation, statistical report, etc.
 *   @history 2010-08-13 Debbie A. Cook Changed surface point from lat/lon/radius to body-fixed XYZ.
 *   @history 2010-12-17 Debbie A. Cook Merged Ken Edmundson version with system and updated to new binary control net
*/

#include "ControlNet.h"
#include "SerialNumberList.h"
#include "ObservationNumberList.h"
#include "Camera.h"
#include "Statistics.h"
#include "SpicePosition.h"
#include "Progress.h"
#include "CameraGroundMap.h"
#include "ControlMeasure.h"

#include "boost/numeric/ublas/symmetric.hpp"

#if !defined(__sun__)
#include "gmm/gmm.h"
#endif

namespace Isis {

  using namespace boost::numeric::ublas;

//  class Latitude;
//  class Longitude;
  class LeastSquares;
  class BasisFunction;

  class BundleAdjust {
    public:
      BundleAdjust(const std::string &cnetFile, const std::string &cubeList,
                   bool printSummary=true);
      BundleAdjust(const std::string &cnetFile, const std::string &cubeList,
                   const std::string &heldList,bool printSummary=true);
      BundleAdjust(Isis::ControlNet &cnet, Isis::SerialNumberList &snlist,
                   bool printSummary=true);
      BundleAdjust(Isis::ControlNet &cnet, Isis::SerialNumberList &snlist,
                   Isis::SerialNumberList &heldsnlist,bool printSummary=true);
      ~BundleAdjust();

      bool ReadSCSigmas(const std::string& scsigmasList);

      double Solve(double tol);
      bool SolveSpecialK();

      Isis::ControlNet *ControlNet() { return m_pCnet; }

      Isis::SerialNumberList *SerialNumberList() { return m_pSnList; }
      int Images() const { return m_pSnList->Size(); }
      int Observations() const;
      std::string Filename(int index);
      Table Cmatrix(int index);
      Table SpVector(int index);

      void SetSolveTwist(bool solve) { m_bSolveTwist = solve; ComputeNumberPartials(); }
      void SetSolveRadii(bool solve) { m_bSolveRadii = solve; }
      void SetErrorPropagation(bool b) { m_bErrorPropagation = b; }
      void SetOutlierRejection(bool b) { m_bOutlierRejection = b; }

      void SetGlobalLatitudeAprioriSigma(double d) { m_dGlobalLatitudeAprioriSigma = d; }
      void SetGlobalLongitudeAprioriSigma(double d) { m_dGlobalLongitudeAprioriSigma = d; }
      void SetGlobalRadiiAprioriSigma(double d) { m_dGlobalRadiusAprioriSigma = d; }

//    void SetGlobalSurfaceXAprioriSigma(double d) { m_dGlobalSurfaceXAprioriSigma = d; }
//    void SetGlobalSurfaceYAprioriSigma(double d) { m_dGlobalSurfaceYAprioriSigma = d; }
//    void SetGlobalSurfaceZAprioriSigma(double d) { m_dGlobalSurfaceZAprioriSigma = d; }
//
      void SetGlobalSpacecraftPositionAprioriSigma(double d) { m_dGlobalSpacecraftPositionAprioriSigma = d; }
      void SetGlobalSpacecraftVelocityAprioriSigma(double d) { m_dGlobalSpacecraftVelocityAprioriSigma = d; }
      void SetGlobalSpacecraftAccelerationAprioriSigma(double d) { m_dGlobalSpacecraftAccelerationAprioriSigma = d; }

      void SetGlobalCameraAnglesAprioriSigma(double d) { m_dGlobalCameraAnglesAprioriSigma = d; }
      void SetGlobalCameraAngularVelocityAprioriSigma(double d) { m_dGlobalCameraAngularVelocityAprioriSigma = d; }
      void SetGlobalCameraAngularAccelerationAprioriSigma(double d) { m_dGlobalCameraAngularAccelerationAprioriSigma = d; }

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

      struct SpacecraftWeights
      {
          std::string SpacecraftName;
          std::string InstrumentId;
          std::vector<double> weights;
      };

      void SetSolveCmatrix(CmatrixSolveType type);
      void SetSolveSpacecraftPosition(SpacecraftPositionSolveType type) { m_spacecraftPositionSolveType = type; }

      //! Set the degree of the polynomial to fit to the camera angles
      void SetCkDegree( int degree ) { m_nckDegree = degree; }

      //! Set the degree of the polynomial to adjust in the solution
      void SetSolveCamDegree( int degree ) { m_nsolveCamDegree = degree; }

      int BasisColumns();
      int ComputeConstrainedParameters();

      double Error() const { return m_dError; }
      double Iteration() const { return m_nIteration; }

//      int HeldPoints() const { return m_nHeldPoints; }
      int IgnoredPoints() const { return m_nIgnoredPoints; }
      int GroundPoints() const { return m_nGroundPoints; }
      void SetObservationMode ( bool bObservationMode );

      void SetConvergenceThreshold(double d) { m_dConvergenceThreshold = d; }
      void SetMaxIterations(int n) { m_nMaxIterations = n; }

      //! Set the solution method to use for solving the matrix
      void SetSolutionMethod ( std::string str ) { m_strSolutionMethod = str; }

    private:

      void Init(Progress *progress=0);

      void ComputeNumberPartials();
      void ComputeImageParameterWeights();

      void AddPartials (int nPointIndex);
      void Update (BasisFunction &basis);

      int PointIndex (int i) const;
      int ImageIndex (int i) const;

      void CheckHeldList();
      void ApplyHeldList();

      // triangulation functions
      int Triangulation(bool bDoApproximation = false);
      bool ApproximatePoint_ClosestApproach(ControlPoint& rpoint, int nIndex);
      bool TriangulatePoint(ControlPoint& rpoint);
      bool TriangulationPartials();

      bool SetParameterWeights();
      void SetPostBundleSigmas();

      // output functions
      void IterationSummary(double avErr, double sigmaXY, double sigmaHat, double sigmaX,
                            double sigmaY);
      bool Output();
      bool OutputWithErrorPropagation();
      bool OutputNoErrorPropagation();
      bool OutputPointsCSV();
      bool OutputImagesCSV();
      bool OutputResiduals();
      bool WrapUp();

                                                             //!< flags...
      bool m_bSolveTwist;                                    //!< to solve for "twist" angle
      bool m_bSolveRadii;                                    //!< to solve for point radii
      bool m_bObservationMode;                               //!< for observation mode (explain this somewhere)
      bool m_bErrorPropagation;                              //!< to perform error propagation
      bool m_bOutlierRejection;                              //!< to perform automatic outlier detection/rejection
      bool m_bPrintSummary;                                  //!< to print summary
      bool m_bOutputCSV;                                     //!< to output points and image station data in csv format                                            //!
      bool m_bCleanUp;                                       //!< for cleanup (i.e. in destructor)
      bool m_bSimulatedData;                                 //!< indicating simulated (i.e. 'perfect' data)
      bool m_bLastIteration;
      bool m_bMaxIterationsReached;

      int m_nIteration;                                      //!< current iteration
      int m_nMaxIterations;                                  //!< maximum iterations
      int m_nNumImagePartials;                               //!< number of image-related partials
      int m_nNumPointPartials;                               //!< number of point-related partials
      int m_nObservations;                                   //!< number of image coordinate observations
  //      int m_nRejectedObservations;                         //!< number of rejected image coordinate observations                                             //! 
      int m_nImageParameters;                                //!< number of image parameters
      int m_nPointParameters;                                //!< total number of point parameters (including constrained)
      int m_nConstrainedPointParameters;                     //!< number of constrained point parameters
      int m_nConstrainedImageParameters;                     //!< number of constrained image parameters
      int m_nDegreesOfFreedom;                               //!< degrees of freedom                                            //! 
      int m_nHeldPoints;                                     //!< number of 'held' points (define)
      int m_nGroundPoints;                                   //!< number of 'ground' points (define)
      int m_nIgnoredPoints;                                  //!< number of ignored points
      int m_nHeldImages;                                     //!< number of 'held' images (define)
      int m_nHeldObservations;                               //!< number of 'held' observations (define)
      int m_nckDegree;                                       //!< ck degree (define)
      int m_nsolveCamDegree;                                 //!< solve cad degree (define)
      int m_nNumberCameraCoefSolved;                         //!< number of camera angle coefficients in solution
      int m_nUnknownParameters;                              //!< total number of parameters to solve for
      int m_nBasisColumns;                                   //!< number of columns (parameters) in normal equations

      std::vector<int> m_nPointIndexMap;                     //!< index into normal equations of point parameter positions
      std::vector<int> m_nImageIndexMap;                     //!< index into normal equations of image parameter positions

      double m_dError;                                       //!< error
      double m_dConvergenceThreshold;                        //!< bundle convergence threshold
      double m_dElapsedTime;                                 //!< elapsed time for bundle
      double m_dElapsedTimeErrorProp;                        //!< elapsed time for error propagation                                            //!
      double m_dSigma0;                                      //!< std deviation of unit weight
      double m_drms_rx;                                      //!< rms of x residuals  
      double m_drms_ry;                                      //!< rms of y residuals
      double m_drms_rxy;                                     //!< rms of all x and y residuals
      double m_dRejectionLimit;                              //!< current rejection limit

                                                             //!< apriori sigmas from user interface
                                                             //!< for points, these override values control-net except

                                                             //!< for "held" & "ground" points
      double m_dGlobalLatitudeAprioriSigma;                  //!< latitude apriori sigma 
      double m_dGlobalLongitudeAprioriSigma;                 //!< longitude apriori sigma
      double m_dGlobalRadiusAprioriSigma;                    //!< radius apriori sigma
      double m_dGlobalSurfaceXAprioriSigma;                  //!< surface point x apriori sigma 
      double m_dGlobalSurfaceYAprioriSigma;                  //!< surface point y apriori sigma
      double m_dGlobalSurfaceZAprioriSigma;                  //!< surface point z apriori sigma

      double m_dGlobalSpacecraftPositionAprioriSigma;        //!< spacecraft coordinates apriori sigmas
      double m_dGlobalSpacecraftVelocityAprioriSigma;        //!< spacecraft coordinate velocities apriori sigmas
      double m_dGlobalSpacecraftAccelerationAprioriSigma;    //!< spacecraft coordinate accelerations apriori sigmas

      double m_dGlobalCameraAnglesAprioriSigma;              //!< camera angles apriori sigmas
      double m_dGlobalCameraAngularVelocityAprioriSigma;     //!< camera angular velocities apriori sigmas
      double m_dGlobalCameraAngularAccelerationAprioriSigma; //!< camera angular accelerations apriori sigmas

      double m_dGlobalSpacecraftPositionWeight;
      double m_dGlobalSpacecraftVelocityWeight;
      double m_dGlobalSpacecraftAccelerationWeight;
      double m_dGlobalCameraAnglesWeight;
      double m_dGlobalCameraAngularVelocityWeight;
      double m_dGlobalCameraAngularAccelerationWeight;

      std::vector<double> m_dImageParameterWeights;


      double m_dRTM;                                         //!< radians to meters conversion factor (body specific)
      double m_dMTR;                                         //!< meters to radians conversion factor (body specific)
      double m_BodyRadii[3];                                 //!< body radii

      std::vector<double> m_dEpsilons;                       //!< vector maintaining total corrections to parameters
      std::vector<double> m_dParameterWeights;               //!< vector of parameter weights

      std::vector<double> m_dxKnowns;
      std::vector<double> m_dyKnowns;

      std::string p_cnetFile;                                //!< Control Net file specification
      std::string m_strSolutionMethod;                       //!< solution method string (QR,SVD,SPARSE-LU,SPECIALK)

                                                             //!< pointers to...
      Isis::LeastSquares* m_pLsq;                            //!< 'LeastSquares' object
      Isis::ControlNet* m_pCnet;                             //!< 'ControlNet' object
      Isis::SerialNumberList* m_pSnList;                     //!< list of image serial numbers
      Isis::SerialNumberList* m_pHeldSnList;                 //!< list of held image serial numbers
      Isis::ObservationNumberList* m_pObsNumList;            //!< list of observation numbers

                                                             //!< vectors for statistical computations...
      Statistics m_Statsx;                                   //!<  x errors
      Statistics m_Statsy;                                   //!<  y errors
      Statistics m_Statsrx;                                  //!<  x residuals
      Statistics m_Statsry;                                  //!<  y residuals
      Statistics m_Statsrxy;                                 //!< xy residuals

      CmatrixSolveType m_cmatrixSolveType;                          //!< cmatrix solve type (define)
      SpacecraftPositionSolveType m_spacecraftPositionSolveType;    //!< spacecraft position solve type (define)

      std::vector<SpacecraftWeights>  m_SCWeights;

      // beyond this place (there be dragons) all refers to the folded bundle solution (referred to as 'SpecialK'
      // in the interim; there is no dependence on the least-squares class

  private:
      int m_nRank;

      bool m_bConverged;
      bool m_bError;

      symmetric_matrix<double,upper,column_major>     m_Normals;           //!< reduced normal equations matrix
//      symmetric_matrix<double,lower>     m_Normals;                      //!< reduced normal equations matrix
      vector<double> m_nj;
      std::vector<compressed_matrix<double> > m_Qs;                        //!< array of Qs   (see Brown, 1976)

//      vector<bounded_vector<double,3> >  m_NICs;                         //!< array of NICs (see Brown, 1976)
      std::vector<bounded_vector<double,3> >  m_NICs;                      //!< array of NICs (see Brown, 1976)

      vector<double> m_Image_Corrections;                                  //!< image parameter cumulative correction vector
      vector<double> m_Image_Solution;                                     //!< image parameter solution vector

//      vector<bounded_vector<double,3> >  m_Point_Corrections;            //!< vector of corrections to 3D point parameter
      std::vector<bounded_vector<double,3> > m_Point_Corrections;          //!< vector of corrections to 3D point parameter
      std::vector<bounded_vector<double,3> > m_Point_AprioriSigmas;        //!< vector of apriori sigmas for 3D point parameters
      std::vector<bounded_vector<double,3> > m_Point_Weights;              //!< vector of weights for 3D point parameters

      void Initialize();
      bool InitializePointWeights();
      void InitializePoints();

      bool FormNormalEquations();

      bool ComputePartials (matrix<double>& coeff_image, matrix<double>& coeff_point3D,
                            vector<double>& coeff_RHS, ControlMeasure& measure,
                            ControlPoint& point);

      bool FormNormalEquations1(symmetric_matrix<double,upper>&N22, matrix<double>& N12,
                                compressed_vector<double>& n1, vector<double>& n2,
                                matrix<double>& coeff_image, matrix<double>& coeff_point3D,
                                vector<double>& coeff_RHS, int nImageIndex);

      bool FormNormalEquations2(symmetric_matrix<double,upper>&N22, matrix<double>& N12,
                                vector<double>& n2, vector<double>& nj, int nPointIndex, int i);

      bool FormNormalEquations3(compressed_vector<double>& n1,vector<double>& nj);

      bool SolveSystem();
      bool CholeskyUT_NOSQR();
      bool CholeskyUT_NOSQR_Inverse();
      bool CholeskyUT_NOSQR_BackSub(symmetric_matrix<double,upper,column_major>& m, vector<double>& s, vector<double>& rhs);
//      bool CholeskyUT_NOSQR_BackSub(symmetric_matrix<double,lower>& m, vector<double>& s, vector<double>& rhs);
      void ApplyParameterCorrections();
      double ComputeResiduals();
      bool ErrorPropagation();

      // dedicated matrix functions
      void transA_NZ_multAdd(double alpha, compressed_matrix<double>& A, vector<double>& B,
                             vector<double>& C);
      void AmultAdd_CNZRows(double alpha, matrix<double>& A, compressed_matrix<double>& B,
                            symmetric_matrix<double, upper,column_major>& C);
//      void AmultAdd_CNZRows(double alpha, matrix<double>& A, compressed_matrix<double>& B,
//                            symmetric_matrix<double, lower>& C);
      bool Invert_3x3(symmetric_matrix<double,upper>& m);
      bool ComputeRejectionLimit();
      bool FlagOutliers();
  };
};

#endif

