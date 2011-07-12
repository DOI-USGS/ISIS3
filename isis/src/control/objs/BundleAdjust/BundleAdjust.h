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
 *   @history 2011-02-01 Debbie A. Cook Moved code to create point index map into its own method to be called after the solution
 *                          method has been set.
 *   @history 2011-02-17 Debbie A. Cook Updated to use new parameter added to SpicePosition, p_timeScale
 *   @history 2011-03-05 Debbie A. Cook Put point index creation back in init.  This will prevent QRD and SVD from working if ground
 *                          points are in the control net.
 *   @history 2011-03-29 Ken Edmundson Fixed bug in observation mode when solving for spacecraft position and improved output
 *   @history 2011-04-02 Debbie A. Cook Updated to ControlPoint class changes regarding target radii.  Also separated out 2 sets of
 *                          calculations to test later for efficiency
 *   @history 2011-06-05 Debbie A. Cook Changed checks for solution type to match change from SPARSE to SPARSE-LU
 *   @history 2011-06-07 Debbie A. Cook and Tracie Sucharski - Modified point types
 *                          Ground ------> Fixed
 *                          Tie----------> Free
 *   @history 2011-06-14 Debbie A. Cook added method IsHeld(int index) for preventing any updates to held images
 *   @history 2011-06-27 Debbie A. Cook and Ken Edmundson Added names to top
 *                          header fields of .csv output and fixed bugs in
 *                          sparse output.
 *   @history 2011-07-12 Ken Edmundson Segmentation fault bugfix in OutputHeader
 *                          method. Previously was attempting to output camera
 *                          angle sigmas when none had been allocated.
 */

#include <vector>
#include <fstream>

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

//  class Latitude;
//  class Longitude;
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

      bool ReadSCSigmas(const std::string &scsigmasList);

      double Solve();
      bool SolveSpecialK();

      Isis::ControlNet *ControlNet() { return m_pCnet; }

      Isis::SerialNumberList *SerialNumberList() { return m_pSnList; }
      int Images() const { return m_pSnList->Size(); }
      int Observations() const;
      std::string Filename(int index);
      bool IsHeld(int index);
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

      void SetGlobalCameraAnglesAprioriSigma(double d) {  if( m_nNumberCameraCoefSolved < 1 ) return; m_dGlobalCameraAnglesAprioriSigma[0] = d; }
      void SetGlobalCameraAngularVelocityAprioriSigma(double d) { if( m_nNumberCameraCoefSolved < 2 ) return; m_dGlobalCameraAnglesAprioriSigma[1] = d; }
      void SetGlobalCameraAngularAccelerationAprioriSigma(double d) { if( m_nNumberCameraCoefSolved < 3 ) return; m_dGlobalCameraAnglesAprioriSigma[2] = d; }

//      void SetGlobalCameraAngularVelocityAprioriSigma(double d) { m_dGlobalCameraAngularVelocityAprioriSigma = d; }
//      void SetGlobalCameraAngularAccelerationAprioriSigma(double d) { m_dGlobalCameraAngularAccelerationAprioriSigma = d; }

      void SetStandardOutput(bool b) { m_bOutputStandard = b; }
      void SetCSVOutput(bool b) { m_bOutputCSV = b; }
      void SetResidualOutput(bool b) { m_bOutputResiduals = b; }
      void SetOutputFilePrefix(const std::string &str) { m_strOutputFilePrefix = str; }

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

      struct SpacecraftWeights {
        std::string SpacecraftName;
        std::string InstrumentId;
        std::vector<double> weights;
      };

      void SetSolveCmatrix(CmatrixSolveType type);
      void SetSolveSpacecraftPosition(SpacecraftPositionSolveType type) { m_spacecraftPositionSolveType = type; }

      //! Set the degree of the polynomial to fit to the camera angles
      void SetCkDegree(int degree) { m_nckDegree = degree; }

      //! Set the degree of the polynomial to adjust in the solution
      void SetSolveCamDegree(int degree) { m_nsolveCamDegree = degree; }

      int BasisColumns();
      int ComputeConstrainedParameters();

      double Error() const { return m_dError; }
      double Iteration() const { return m_nIteration; }

//      int HeldPoints() const { return m_nHeldPoints; }
      int IgnoredPoints() const { return m_nIgnoredPoints; }
      int FixedPoints() const { return m_nFixedPoints; }
      void SetObservationMode(bool bObservationMode);

      void SetConvergenceThreshold(double d) { m_dConvergenceThreshold = d; }
      void SetMaxIterations(int n) { m_nMaxIterations = n; }
      void SetSolutionMethod(std::string str);

      void GetSparseParameterCorrections();

    private:

      void Init(Progress *progress = 0);

      void ComputeNumberPartials();
      void ComputeImageParameterWeights();
      void SetSpaceCraftWeights();

      void AddPartials(int nPointIndex);
      void FillPointIndexMap();
      void Update(BasisFunction &basis);

      int PointIndex(int i) const;
      int ImageIndex(int i) const;

      void CheckHeldList();
      void ApplyHeldList();

      // triangulation functions
      int Triangulation(bool bDoApproximation = false);
      bool ApproximatePoint_ClosestApproach(const ControlPoint &rpoint, int nIndex);
      bool TriangulatePoint(const ControlPoint &rpoint);
      bool TriangulationPartials();

      bool SetParameterWeights();
      void SetPostBundleSigmas();

      // output functions
      void IterationSummary(double avErr, double sigmaXY, double sigmaHat, 
                            double sigmaX, double sigmaY);
      void SpecialKIterationSummary();
      bool Output();
      bool OutputHeader(std::ofstream& fp_out);
      bool OutputWithErrorPropagation();
      bool OutputNoErrorPropagation();
      bool OutputPointsCSV();
      bool OutputImagesCSV();
      bool OutputResiduals();
      bool WrapUp();
      bool ComputeBundleStatistics();

                                                             //!< flags...
      bool m_bSolveTwist;                                    //!< to solve for "twist" angle
      bool m_bSolveRadii;                                    //!< to solve for point radii
      bool m_bObservationMode;                               //!< for observation mode (explain this somewhere)
      bool m_bErrorPropagation;                              //!< to perform error propagation
      bool m_bOutlierRejection;                              //!< to perform automatic outlier detection/rejection
      bool m_bPrintSummary;                                  //!< to print summary
      bool m_bOutputStandard;                                //!< to print standard bundle output file (bundleout.txt)
      bool m_bOutputCSV;                                     //!< to output points and image station data in csv format
      bool m_bOutputResiduals;                               //!< to output residuals in csv format
      bool m_bCleanUp;                                       //!< for cleanup (i.e. in destructor)
      bool m_bSimulatedData;                                 //!< indicating simulated (i.e. 'perfect' data)
      bool m_bLastIteration;
      bool m_bMaxIterationsReached;

      int m_nIteration;                                      //!< current iteration
      int m_nMaxIterations;                                  //!< maximum iterations
      int m_nNumImagePartials;                               //!< number of image-related partials
      int m_nNumPointPartials;                               //!< number of point-related partials
      int m_nObservations;                                   //!< number of image coordinate observations
      int m_nRejectedObservations;                         //!< number of rejected image coordinate observations                                             //!
      int m_nImageParameters;                                //!< number of image parameters
      int m_nPointParameters;                                //!< total number of point parameters (including constrained)
      int m_nConstrainedPointParameters;                     //!< number of constrained point parameters
      int m_nConstrainedImageParameters;                     //!< number of constrained image parameters
      int m_nDegreesOfFreedom;                               //!< degrees of freedom                                            //!
      int m_nHeldPoints;                                     //!< number of 'held' points (define)
      int m_nFixedPoints;                                    //!< number of 'fixed' (ground) points (define)
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
      double m_drms_sigmaLat;                           //!< rms of adjusted Latitude sigmas
      double m_drms_sigmaLon;                          //!< rms of adjusted Longitude sigmas
      double m_drms_sigmaRad;                          //!< rms of adjusted Radius sigmas
      double m_dminSigmaLatitude;
      std::string m_idMinSigmaLatitude;
      double m_dmaxSigmaLatitude;
      std::string m_idMaxSigmaLatitude;
      double m_dminSigmaLongitude;
      std::string m_idMinSigmaLongitude;
      double m_dmaxSigmaLongitude;
      std::string m_idMaxSigmaLongitude;
      double m_dminSigmaRadius;
      std::string m_idMinSigmaRadius;
      double m_dmaxSigmaRadius;
      std::string m_idMaxSigmaRadius;

      double m_dRejectionLimit;                              //!< current rejection limit

      //!< apriori sigmas from user interface
      //!< for points, these override values control-net except

      //!< for "held" & "fixed" points
      double m_dGlobalLatitudeAprioriSigma;                  //!< latitude apriori sigma
      double m_dGlobalLongitudeAprioriSigma;                 //!< longitude apriori sigma
      double m_dGlobalRadiusAprioriSigma;                    //!< radius apriori sigma
      double m_dGlobalSurfaceXAprioriSigma;                  //!< surface point x apriori sigma
      double m_dGlobalSurfaceYAprioriSigma;                  //!< surface point y apriori sigma
      double m_dGlobalSurfaceZAprioriSigma;                  //!< surface point z apriori sigma

      double m_dGlobalSpacecraftPositionAprioriSigma;        //!< spacecraft coordinates apriori sigmas
      double m_dGlobalSpacecraftVelocityAprioriSigma;        //!< spacecraft coordinate velocities apriori sigmas
      double m_dGlobalSpacecraftAccelerationAprioriSigma;    //!< spacecraft coordinate accelerations apriori sigmas

      std::vector<double> m_dGlobalCameraAnglesAprioriSigma;              //!< camera angles apriori sigmas: size is # camera coefficients solved
//      double m_dGlobalCameraAnglesAprioriSigma;              //!< camera angles apriori sigmas
//      double m_dGlobalCameraAngularVelocityAprioriSigma;     //!< camera angular velocities apriori sigmas
//      double m_dGlobalCameraAngularAccelerationAprioriSigma; //!< camera angular accelerations apriori sigmas

      double m_dGlobalSpacecraftPositionWeight;
      double m_dGlobalSpacecraftVelocityWeight;
      double m_dGlobalSpacecraftAccelerationWeight;
      double m_dGlobalCameraAnglesWeight;
      double m_dGlobalCameraAngularVelocityWeight;
      double m_dGlobalCameraAngularAccelerationWeight;

      std::vector<double> m_dImageParameterWeights;


      double m_dRTM;                                         //!< radians to meters conversion factor (body specific)
      double m_dMTR;                                         //!< meters to radians conversion factor (body specific)
      Distance m_BodyRadii[3];                               //!< body radii i meters

      std::vector<double> m_dEpsilons;                       //!< vector maintaining total corrections to parameters
      std::vector<double> m_dParameterWeights;               //!< vector of parameter weights

      std::vector<double> m_dxKnowns;
      std::vector<double> m_dyKnowns;

      std::string m_strCnetFilename;                         //!< Control Net file specification
      std::string m_strSolutionMethod;                       //!< solution method string (QR,SVD,SPARSE-LU,SPECIALK)
      std::string m_strOutputFilePrefix;                     //!< output file prefix

      //!< pointers to...
      Isis::LeastSquares *m_pLsq;                            //!< 'LeastSquares' object
      Isis::ControlNet *m_pCnet;                             //!< 'ControlNet' object
      Isis::SerialNumberList *m_pSnList;                     //!< list of image serial numbers
      Isis::SerialNumberList *m_pHeldSnList;                 //!< list of held image serial numbers
      Isis::ObservationNumberList *m_pObsNumList;            //!< list of observation numbers

      //!< vectors for statistical computations...
      Statistics m_Statsx;                                   //!<  x errors
      Statistics m_Statsy;                                   //!<  y errors
      Statistics m_Statsrx;                                  //!<  x residuals
      Statistics m_Statsry;                                  //!<  y residuals
      Statistics m_Statsrxy;                                 //!< xy residuals

      std::vector<Statistics> m_rmsImageSampleResiduals;
      std::vector<Statistics> m_rmsImageLineResiduals;
      std::vector<Statistics> m_rmsImageResiduals;
      std::vector<Statistics> m_rmsImageXSigmas;
      std::vector<Statistics> m_rmsImageYSigmas;
      std::vector<Statistics> m_rmsImageZSigmas;
      std::vector<Statistics> m_rmsImageRASigmas;
      std::vector<Statistics> m_rmsImageDECSigmas;
      std::vector<Statistics> m_rmsImageTWISTSigmas;

      CmatrixSolveType m_cmatrixSolveType;                          //!< cmatrix solve type (define)
      SpacecraftPositionSolveType m_spacecraftPositionSolveType;    //!< spacecraft position solve type (define)

      std::vector<SpacecraftWeights>  m_SCWeights;

      // beyond this place (there be dragons) all refers to the folded bundle solution (referred to as 'SpecialK'
      // in the interim; there is no dependence on the least-squares class

    private:
      int m_nRank;

      bool m_bConverged;
      bool m_bError;

      boost::numeric::ublas::symmetric_matrix < double,
            boost::numeric::ublas::upper, boost::numeric::ublas::column_major >
            m_Normals; //!< reduced normal equations matrix
//      symmetric_matrix<double,lower>     m_Normals;                      //!< reduced normal equations matrix
      boost::numeric::ublas::vector< double > m_nj;

      //!< array of Qs   (see Brown, 1976)
      std::vector< boost::numeric::ublas::compressed_matrix< double> > m_Qs;

//      vector<bounded_vector<double,3> >  m_NICs;                         //!< array of NICs (see Brown, 1976)
      //!< array of NICs (see Brown, 1976)
      std::vector< boost::numeric::ublas::bounded_vector< double, 3 > >  m_NICs;

      boost::numeric::ublas::vector<double> m_Image_Corrections;                                  //!< image parameter cumulative correction vector
      boost::numeric::ublas::vector<double> m_Image_Solution;                                     //!< image parameter solution vector

//      vector<bounded_vector<double,3> >  m_Point_Corrections;            //!< vector of corrections to 3D point parameter
      std::vector< boost::numeric::ublas::bounded_vector< double, 3 > > m_Point_Corrections;         //!< vector of corrections to 3D point parameter
      std::vector< boost::numeric::ublas::bounded_vector< double, 3 > > m_Point_AprioriSigmas;       //!< vector of apriori sigmas for 3D point parameters
      std::vector< boost::numeric::ublas::bounded_vector< double, 3 > > m_Point_Weights;             //!< vector of weights for 3D point parameters

      void Initialize();
      bool InitializePointWeights();
      void InitializePoints();

      bool FormNormalEquations();

      bool ComputePartials_DC(boost::numeric::ublas::matrix<double>& coeff_image, boost::numeric::ublas::matrix<double>& coeff_point3D,
                           boost::numeric::ublas::vector<double>& coeff_RHS, const ControlMeasure &measure,
                           const ControlPoint &point);

      bool FormNormalEquations1(boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::upper>&N22, boost::numeric::ublas::matrix<double>& N12,
                                boost::numeric::ublas::compressed_vector<double>& n1, boost::numeric::ublas::vector<double>& n2,
                                boost::numeric::ublas::matrix<double>& coeff_image, boost::numeric::ublas::matrix<double>& coeff_point3D,
                                boost::numeric::ublas::vector<double>& coeff_RHS, int nImageIndex);

      bool FormNormalEquations2(boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::upper>&N22, boost::numeric::ublas::matrix<double>& N12,
                                boost::numeric::ublas::vector<double>& n2, boost::numeric::ublas::vector<double>& nj, int nPointIndex, int i);

      bool FormNormalEquations3(boost::numeric::ublas::compressed_vector<double>& n1, boost::numeric::ublas::vector<double>& nj);

      bool SolveSystem();
      bool CholeskyUT_NOSQR();
      bool CholeskyUT_NOSQR_Inverse();
      bool CholeskyUT_NOSQR_BackSub(
        boost::numeric::ublas::symmetric_matrix < double,
        boost::numeric::ublas::upper,
        boost::numeric::ublas::column_major > &m,
        boost::numeric::ublas::vector< double > &s,
        boost::numeric::ublas::vector< double > &rhs);

//      bool CholeskyUT_NOSQR_BackSub(symmetric_matrix<double,lower>& m, vector<double>& s, vector<double>& rhs);
      void ApplyParameterCorrections();
      double ComputeResiduals();
      bool ErrorPropagation();

      // dedicated matrix functions
      void transA_NZ_multAdd(double alpha,
                             boost::numeric::ublas::compressed_matrix< double > &A,
                             boost::numeric::ublas::vector< double > &B,
                             boost::numeric::ublas::vector< double > &C);
      void AmultAdd_CNZRows(double alpha,
                            boost::numeric::ublas::matrix< double > &A,
                            boost::numeric::ublas::compressed_matrix< double > &B,
                            boost::numeric::ublas::symmetric_matrix < double,
                            boost::numeric::ublas::upper,
                            boost::numeric::ublas::column_major > &C);
//      void AmultAdd_CNZRows(double alpha, matrix<double>& A, compressed_matrix<double>& B,
//                            symmetric_matrix<double, lower>& C);
      bool Invert_3x3(boost::numeric::ublas::symmetric_matrix < double,
                      boost::numeric::ublas::upper > &m);
      bool ComputeRejectionLimit();
      bool FlagOutliers();
  };
}

#endif

