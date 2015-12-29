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
 */

#include <QObject> // parent class

#include <vector>
#include <fstream>

#include <CHOLMOD/cholmod.h>
#include <CHOLMOD/UFconfig.h>

#ifndef __sun__
#include <gmm/gmm.h>
#endif

#include "BundleObservationSolveSettings.h"
#include "BundleControlPointVector.h"
#include "BundleObservationVector.h"
#include "BundleResults.h"
#include "BundleSettings.h"
#include "BundleSolutionInfo.h"
#include "BundleTargetBody.h"
#include "Camera.h"
#include "CameraGroundMap.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "MaximumLikelihoodWFunctions.h" // why not just forward declare???
#include "ObservationNumberList.h"
#include "Progress.h"
#include "SerialNumberList.h"
#include "SparseBlockMatrix.h"
#include "Statistics.h"

template< typename T > class QList;
template< typename A, typename B > class QMap;

namespace Isis {
  class Control;
  class ImageList;

  /**
   * @author 2006-05-30 Jeff Anderson, Debbie A. Cook, and Tracie Sucharski
   *
   * @internal
   *   @history 2005-05-30 Jeff Anderson, Debbie A. Cook & Tracie Sucharski - Original version
   *   @history 2007-05-29 Debbie A. Cook - Added new method iterationSummary and
   *                           changed points on held images to held instead of ground.
   *   @history 2007-07-12 Debbie A. Cook - Fixed bug in iteration statistics calculations in the
   *                           case of a single control point that was causing a divide by zero
   *                           error.
   *   @history 2007-08-25 Debbie A. Cook - Added methods and members to support instrument position
   *                           solution.
   *   @history 2007-09-17 Debbie A. Cook - Added ability to process in observation mode for Lunar
   *                           Orbiter.
   *   @history 2007-11-17 Debbie A. Cook - Added method SetSolution Method.
   *   @history 2007-12-21 Debbie A. Cook - Added member p_Degree and methods m_nsolveCamDegree and
   *                           ckDegree.
   *   @history 2008-01-11 Debbie A. Cook - Added observation mode functionality for spacecraft 
   *                           position and upgraded ObservationNumber methods for compatability.
   *   @history 2008-01-14 Debbie A. Cook - Added code to solve for local radii.
   *   @history 2008-04-18 Debbie A. Cook - Added progress for ControlNet.
   *   @history 2008-06-18 Christopher Austin - Fixed ifndef.
   *   @history 2008-11-07 Tracie Sucharski - Added bool to constructors to indicate whether to
   *                           print iteration summary info to the session log. This was needed for
   *                           qtie which has no session log.
   *   @history 2008-11-22 Debbie A. Cook - Added code to wrap longitude to keep it in [0.0, 360.0].
   *   @history 2008-11-22 Debbie A. Cook - Added new call to get timeScale and set for the
   *                           observation along with basetime.
   *   @history 2008-11-26 Debbie A. Cook - Added check to applyHeldList for Ignored points and
   *                           measures.
   *   @history 2009-01-08 Debbie A. Cook - Revised AddPartials and PointPartial to avoid using the
   *                           camera methods to map a body-fixed vector to the camera because they
   *                           compute a new time for line scan cameras based on the lat/lon/radius
   *                           and the new time is used to retrieve Spice. The updated software uses
   *                           the Spice at the time of the measurement.
   *   @history 2009-02-15 Debbie A. Cook - Corrected focal length to include its sign and removed
   *                           obsolete calls to X/Y direction methods.  Also modified PointPartial
   *                           to use lat/lon/radius from the point instead of the camera.
   *   @history 2009-08-13 Debbie A. Cook - Corrected calculations of cudx and cudy so that they use
   *                           the signed focal length also.
   *   @history 2009-10-14 Debbie A. Cook - Modified AddPartials method to use new CameraGroundMap
   *                           method, GetXY.
   *   @history 2009-10-30 Debbie A. Cook - Improved error message in AddPartials.
   *   @history 2009-12-14 Debbie A. Cook - Updated SpicePosition enumerated partial type constants.
   *   @history 2010-03-19 Debbie A. Cook - Moved partials to GroundMap classes to support Radar
   *                           sensors and modified argument list for GroundMap method ComputeXY
   *                           since it now returns cudx and cudy.
   *   @history 2010-06-18 Debbie A. Cook - Added p_cnetFile as member since it was taken out of
   *                           ControlNet.
   *   @history 2010-07-09 Ken Edmundson - Added Folding in solution method (SPECIALK), error
   *                           propogation, statistical report, etc.
   *   @history 2010-08-13 Debbie A. Cook - Changed surface point from lat/lon/radius to body-fixed
   *                           XYZ.
   *   @history 2010-12-17 Debbie A. Cook - Merged Ken Edmundson version with system and updated to
   *                           new binary control net.
   *   @history 2011-02-01 Debbie A. Cook - Moved code to create point index map into its own method
   *                           to be called after the solution method has been set.
   *   @history 2011-02-17 Debbie A. Cook - Updated to use new parameter added to SpicePosition,
   *                           p_timeScale.
   *   @history 2011-03-05 Debbie A. Cook - Put point index creation back in init.  This will
   *                           prevent QRD and SVD from working if ground points are in the control
   *                           net.
   *   @history 2011-03-29 Ken Edmundson - Fixed bug in observation mode when solving for spacecraft
   *                           position and improved output.
   *   @history 2011-04-02 Debbie A. Cook - Updated to ControlPoint class changes regarding target
   *                           radii.  Also separated out 2 sets of calculations to test later for
   *                           efficiency.
   *   @history 2011-06-05 Debbie A. Cook - Changed checks for solution type to match change from
   *                           SPARSE to SPARSE-LU.
   *   @history 2011-06-07 Debbie A. Cook - and Tracie Sucharski - Modified point types from
   *                           Ground to Fixed and from Tie to Free.
   *   @history 2011-06-14 Debbie A. Cook - added method isHeld(int index) for preventing any
   *                           updates to held images.
   *   @history 2011-06-27 Debbie A. Cook - and Ken Edmundson Added names to top header fields of
   *                           .csv output and fixed bugs in sparse output.
   *   @history 2011-07-12 Ken Edmundson - Segmentation fault bugfix in OutputHeader method.
   *                           Previously was attempting to output camera angle sigmas when none had
   *                           been allocated.
   *   @history 2011-07-14 Ken Edmundson and Debbie Cook - Added new member, m_bDeltack to indicate
   *                           calling application was deltack (or qtie) and has potential to have a
   *                           single ControlPoint and ControlMeasure.
   *   @history 2011-08-08 Tracie Sucharski - Added method to return the iteration summary to be
   *                           used in qtie which does not have a log file. In SetImages, clear the
   *                           cameraMavtpv_targetBodyp and cameraList.  Added this back in (was originally added
   *                           on 2011-01-19), was deleted somewhere along the line.
   *   @history 2011-09-28 Debbie A. Cook - Renamed SPARSE solve method to OLDSPARSE and CHOLMOD to
   *                           SPARSE. 
   *   @history 2011-10-14 Ken Edmundson - Added call to m_pCnet->ClearJigsawRejected() to the
   *                           init() method to set all measure/point JigsawRejected flags to false
   *                           prior to bundle.
   *   @history 2011-12-09 Ken Edmundson - Memory leak fix in method cholmod_Inverse. Need call to
   *                           "cholmod_free_dense(&x,&m_cm)" inside loop.
   *   @history 2011-12-20 Ken Edmundson - Fixes to outlier rejection. Added rejection multiplier
   *                           member variable, can be set in jigsaw interface.
   *   @history 2012-02-02 Debbie A. Cook - Added SetSolvePolyOverHermite method and members
   *                           m_bSolvePolyOverHermite and m_nPositionType.
   *   @history 2012-03-26 Orrin Thomas - Added maximum likelihood capabilities.
   *   @history 2012-05-21 Debbie A. Cook - Added initialization of m_dRejectionMultiplier.
   *   @history 2012-07-06 Debbie A. Cook - Updated Spice members to be more compliant with Isis 
   *                           coding standards. References #972.
   *   @history 2012-09-28 Ken Edmundson - Initialized variables for bundle statistic computations.
   *                           The bundleout.txt file was modifed to show N/A for RMS, Min, Max of
   *                           Radius Sigmas when not solving for radius. References #783.
   *   @history 2013-11-12 Ken Edmundson - Programmers Note. References #813, #1521, #1653
   *                           #813 - Info echoed to screen when using Maximum Likelihood
   *                                  methods are now printed to print.prt file.
   *                           #1521 - The cout debug statements that appear on screen when updating
   *                                   images were removed from SpiceRotation.cpp
   *                           #1653 - Constraints were being applied for "Free" points that have
   *                                   constrained coordinates. Also found that a priori coordinates
   *                                   for these points were not being computed in
   *                                   ControlPoint::ComputeApriori, this has also been fixed.
   *   @history 2013-12-18 Tracie Sucharski - The ControlNet::GetNumberOfMeasuresInImage was
   *                           renamed to ControlNet::GetNumberOfValidMeasuresInImage and only
   *                           returns the number of valid (Ignore= False) measures.
   *   @history 2014-02-25 Ken Edmundson - Speed up and memory improvements to error propagation.
   *                           References #2031.
   *   @history 2014-05-16 Jeannie Backer - Added BundleSettings object to constructor inputs.
   *                           Cleaned and organized code. Updated to be more compliant with ISIS
   *                           coding standards.
   *   @history 2014-07-14 Kimberly Oyama - Added support for correlation matrix. Covariance matrix
   *                           is now written to a file and the location is saved as part of the
   *                           CorrelationMatrix object.
   *   @history 2014-07-23 Jeannie Backer - Modified to print "N/A" for rejection multiplier if 
   *                           outlier rejection is turned off.
   *   @history 2014-09-18 Kimberly Oyama - Added a constructor for running the bunlde in a
   *                           separate thread.
   *   @history 2014-11-05 Ken Edmundson - Fixed memory bug. Wasn't releasing cholmod_factor m_L
   *                           every iteration. Now release every iteration but the last since we
   *   @history 2015-02-20 Jeannie Backer - Updated to be more compliant with ISIS coding standards.
   *   @history 2015-08-17 Jeannie Backer - Updated to be more compliant with ISIS coding standards.
   *   @history 2015-09-03 Jeannie Backer - Changed the name of the output correlation matrix file
   *                           from ["inverseMatrix" + random unique code + ".dat"] to
   *                           [BundleSettings::outputFilePrefix() + "inverseMatrix.dat"]. So that
   *                           the prefix can be used to specify the path of the location where the
   *                           matrix file should be written. Some improvements made to comply with
   *                           coding standards.
   */
  class BundleAdjust : public QObject {
      Q_OBJECT
    public:
      BundleAdjust(BundleSettingsQsp bundleSettings,
                   const QString &cnetFile, 
                   const QString &cubeList,
                   bool printSummary = true);
      BundleAdjust(BundleSettingsQsp bundleSettings,
                   const QString &cnetFile, 
                   const QString &cubeList,
                   const QString &heldList, 
                   bool printSummary = true);
      BundleAdjust(BundleSettingsQsp bundleSettings,
                   QString &cnet, 
                   SerialNumberList &snlist, 
                   bool printSummary = true);
      BundleAdjust(BundleSettingsQsp bundleSettings,
                   Control &cnet,
                   SerialNumberList &snlist,
                   bool bPrintSummary);
      BundleAdjust(BundleSettingsQsp bundleSettings,
                   ControlNet &cnet, 
                   SerialNumberList &snlist, 
                   bool printSummary = true);
      BundleAdjust(BundleSettings bundleSettings, 
                   ControlNet &cnet, 
                   SerialNumberList &snlist, 
                   SerialNumberList &heldsnlist, 
                   bool printSummary = true);
      BundleAdjust(BundleSettingsQsp bundleSettings,
                   Control &control,
                   QList<ImageList *> imgList,
                   bool bPrintSummary);
      ~BundleAdjust();
    
      double           solve();
      BundleSolutionInfo    solveCholeskyBR();
///////////////////////////////////////////////////////////////////////////////////////////////////
    public slots:
      bool             solveCholesky();
      void             abortBundle();
///////////////////////////////////////////////////////////////////////////////////////////////////
      // accessors
      ControlNet       *controlNet() { return m_pCnet; } // TODO: change from pointer to const ref???
      SerialNumberList *serialNumberList() { return m_pSnList; } // TODO: move implementation to cpp per ISIS standards
      int              images() const { return m_pSnList->size(); }// TODO: move implementation to cpp per ISIS standards
//      int              observations() const;
      QString          fileName(int index);
      bool             isHeld(int index);
      Table            cMatrix(int index);
      Table            spVector(int index);
      double error() const { // TODO: move implementation to cpp per ISIS standards
        return m_dError;
      }
      double iteration() const { // TODO: move implementation to cpp per ISIS standards
        return m_nIteration;
      }

      bool isConverged();
      QString iterationSummaryGroup() const {
        return m_iterationSummary;
      } // TODO: move implementation to cpp per ISIS standards

    signals:
      void statusUpdate(QString);
      void error(QString);
      void iterationUpdate(int, double);
      void bundleException(QString);
      void resultsReady(BundleSolutionInfo *bundleSolveInformation);
      void finished();

    public slots:
      void outputBundleStatus(QString status);

    private:

      void init(Progress *progress = 0); // combine all initializations?
      void initialize();

      void checkHeldList(); // unnecessary?

      bool validateNetwork();
      // use bundle settings to initialize more variables
      // JWB - should I make a public method resetBundle(BundleSettings bundleSettings);
      // that calls these methods to allow rerun with new params???
      // max likelihood says it may be called multiple times...
      // where does this happen ???

      void applyHeldList();

      // output methods
      void iterationSummary();
      bool output();
      bool outputHeader(std::ofstream  &fp_out);
      bool outputText();
      bool outputPointsCSV();
      bool outputImagesCSV();
      bool outputResiduals();
      bool wrapUp();
      BundleSolutionInfo bundleSolveInformation();
      bool computeBundleStatistics();

      bool formNormalEquations();
      void applyParameterCorrections();
      bool errorPropagation();
      bool solveSystem();

      // solution, error propagation, and matrix methods for cholmod approach
      bool formNormalEquations_CHOLMOD();

      bool formNormals1_CHOLMOD(boost::numeric::ublas::symmetric_matrix< double,
                                boost::numeric::ublas::upper >  &N22,
                                SparseBlockColumnMatrix  &N12,
                                boost::numeric::ublas::compressed_vector< double >  &n1,
                                boost::numeric::ublas::vector< double >  &n2,
                                boost::numeric::ublas::matrix< double >  &coeff_target,
                                boost::numeric::ublas::matrix< double >  &coeff_image,
                                boost::numeric::ublas::matrix< double >  &coeff_point3D,
                                boost::numeric::ublas::vector< double >  &coeff_RHS,
                                int observationIndex);

      bool formNormals2_CHOLMOD(boost::numeric::ublas::symmetric_matrix< double,
                                boost::numeric::ublas::upper >  &N22,
                                SparseBlockColumnMatrix  &N12,
                                boost::numeric::ublas::vector< double >  &n2,
                                boost::numeric::ublas::vector< double >  &nj,
                                BundleControlPoint *point);

      bool formNormals3_CHOLMOD(boost::numeric::ublas::compressed_vector< double >  &n1,
                                boost::numeric::ublas::vector< double >  &nj);

      bool solveSystem_CHOLMOD();

      void AmultAdd_CNZRows_CHOLMOD(double alpha, SparseBlockColumnMatrix &A,
                                    SparseBlockRowMatrix &B);

      void transA_NZ_multAdd_CHOLMOD(double alpha, SparseBlockRowMatrix &A,
                                     boost::numeric::ublas::vector< double > &B,
                                     boost::numeric::ublas::vector< double > &C);

      void applyParameterCorrections_CHOLMOD();

      bool errorPropagation_CHOLMOD();

      // solution, error propagation, and matrix methods for specialk approach
      // TODO: this may be able to go away if I can verify cholmod behavior for a truly dense matrix
      bool formNormalEquations_SPECIALK();

      bool formNormals1_SPECIALK(boost::numeric::ublas::symmetric_matrix< double,
                                 boost::numeric::ublas::upper >  &N22,
                                 boost::numeric::ublas::matrix< double >  &N12,
                                 boost::numeric::ublas::compressed_vector< double >  &n1,
                                 boost::numeric::ublas::vector< double >  &n2,
                                 boost::numeric::ublas::matrix< double >  &coeff_image,
                                 boost::numeric::ublas::matrix< double >  &coeff_point3D,
                                 boost::numeric::ublas::vector< double >  &coeff_RHS,
                                 int nImageIndex);

      bool formNormals2_SPECIALK(boost::numeric::ublas::symmetric_matrix< double,
                                 boost::numeric::ublas::upper >  &N22,
                                 boost::numeric::ublas::matrix< double >  &N12,
                                 boost::numeric::ublas::vector< double >  &n2,
                                 boost::numeric::ublas::vector< double >  &nj,
                                 int nPointIndex, int i);

      bool formNormals3_SPECIALK(boost::numeric::ublas::compressed_vector< double >  &n1,
                                 boost::numeric::ublas::vector< double >  &nj);

      bool solveSystem_SPECIALK();

      void AmultAdd_CNZRows_SPECIALK(double alpha,
                                     boost::numeric::ublas::matrix< double > &A,
                                     boost::numeric::ublas::compressed_matrix< double > &B,
                                     boost::numeric::ublas::symmetric_matrix< double,
                                     boost::numeric::ublas::upper,
                                     boost::numeric::ublas::column_major > &C);

      void transA_NZ_multAdd_SPECIALK(double alpha,
                                      boost::numeric::ublas::compressed_matrix< double > &A,
                                      boost::numeric::ublas::vector< double > &B,
                                      boost::numeric::ublas::vector< double > &C);

      void applyParameterCorrections_SPECIALK();

      bool errorPropagation_SPECIALK();

      bool CholeskyUT_NOSQR();
      bool CholeskyUT_NOSQR_Inverse();
      bool CholeskyUT_NOSQR_BackSub(
                                    boost::numeric::ublas::symmetric_matrix< double,
                                    boost::numeric::ublas::upper,
                                    boost::numeric::ublas::column_major >  &m,
                                    boost::numeric::ublas::vector< double >  &s,
                                    boost::numeric::ublas::vector< double >  &rhs);

//      bool computePartials_DC(boost::numeric::ublas::matrix< double >  &coeff_image,
//                              boost::numeric::ublas::matrix< double >  &coeff_point3D,
//                              boost::numeric::ublas::vector< double >  &coeff_RHS,
//                              const ControlMeasure &measure, const ControlPoint &point);
      bool computePartials_DC(boost::numeric::ublas::matrix< double >  &coeff_target,
                              boost::numeric::ublas::matrix< double >  &coeff_image,
                              boost::numeric::ublas::matrix< double >  &coeff_point3D,
                              boost::numeric::ublas::vector< double >  &coeff_RHS,
                              BundleMeasure &measure, BundleControlPoint &point);

//      bool CholeskyUT_NOSQR_BackSub(symmetric_matrix<double,lower> &m, vector<double> &s, vector<double> &rhs);
      double computeResiduals();

      // dedicated matrix functions
      void AmulttransBNZ(boost::numeric::ublas::matrix< double >  &A,
                         boost::numeric::ublas::compressed_matrix< double >  &B,
                         boost::numeric::ublas::matrix< double >  &C, double alpha = 1.0);
      void ANZmultAdd(boost::numeric::ublas::compressed_matrix< double >  &A,
                      boost::numeric::ublas::symmetric_matrix< double,
                      boost::numeric::ublas::upper, boost::numeric::ublas::column_major >  &B,
                      boost::numeric::ublas::matrix< double >  &C, double alpha = 1.0);

      bool Invert_3x3(boost::numeric::ublas::symmetric_matrix< double,
                      boost::numeric::ublas::upper >  &m);

      bool product_ATransB(boost::numeric::ublas::symmetric_matrix< double,
                           boost::numeric::ublas::upper >  &N22, SparseBlockColumnMatrix  &N12,
                           SparseBlockRowMatrix  &Q);
      void product_AV(double alpha, boost::numeric::ublas::bounded_vector< double, 3 > &v2,
                      SparseBlockRowMatrix &Q, boost::numeric::ublas::vector< double > &v1);

      bool computeRejectionLimit();
      bool flagOutliers();

      bool initializeCHOLMODLibraryVariables();
      bool freeCHOLMODLibraryVariables();
      bool cholmod_Inverse();
      bool loadCholmodTriplet();

      // JWB - member data should be reorganized... by type possibly? by use
      //       probably better??? !< flags...
      //       we can probably remove several that are unused ???
      bool m_abort;                                     //!< to abort threaded bundle
      bool m_bPrintSummary;                             //!< to print summary
      bool m_bCleanUp;                                  //!< for cleanup (i.e. in destructor)

      int m_nIteration;                                 //!< current iteration
      int m_nNumImagePartials;                          //!< number of image-related partials
      int m_nNumPointPartials;                          //!< number of point-related partials
      int m_nNumberCamAngleCoefSolved;                  //!< number of camera angle coefficients in solution
      int m_nNumberCamPosCoefSolved;                    //!< number of camera position coefficients in solution
      SpicePosition::Source m_nPositionType;            //!< type of SpicePosition interpolation
      SpiceRotation::Source m_nPointingType;            //!< type of SpiceRotation interpolation

      double m_dError;                                  //!< error

      BundleObservationVector m_bundleObservations;
      BundleControlPointVector m_bundleControlPoints;
      BundleTargetBodyQsp m_bundleTargetBody;

      double m_dRTM;                                    //!< radians to meters conversion factor (body specific)
      double m_dMTR;                                    //!< meters to radians conversion factor (body specific)
      Distance m_bodyRadii[3];                          //!< body radii i meters

      QString m_strCnetFileName;                        //!< Control Net file specification

      //!< pointers to...
      ControlNet *m_pCnet;                              //!< 'ControlNet' object
      SerialNumberList *m_pSnList;                      //!< list of image serial numbers
      SerialNumberList *m_pHeldSnList;                  //!< list of held image serial numbers

      // BEYOND THIS PLACE (THERE BE DRAGONS) all refers to the folded bundle solution (referred to
      // as either 'CHOLMOD' (sparse solution) or 'SpecialK' (dense solution - less desirable) in
      // the interim; there is no dependence on the least-squares class.

      int m_nRank;

      boost::numeric::ublas::symmetric_matrix< double, 
                                               boost::numeric::ublas::upper, 
                                               boost::numeric::ublas::column_major > m_Normals; //!< reduced normal equations matrix
      boost::numeric::ublas::vector< double > m_nj;

      //!< array of Qs   (see Brown, 1976)
      std::vector< boost::numeric::ublas::compressed_matrix< double > > m_Qs_SPECIALK;

      boost::numeric::ublas::vector< double > m_imageSolution;                                     //!< image parameter solution vector


      QString m_iterationSummary;

      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      // variables for cholmod
      cholmod_common  m_cm;
      cholmod_factor *m_L;
      cholmod_sparse *m_N;

      SparseBlockMatrix m_SparseNormals;
      cholmod_triplet  *m_pTriplet;

      BundleSettingsQsp m_bundleSettings;

      BundleResults  m_bundleResults;
//    BundleSolutionInfo m_bundleSolveInformation;


// ??? moved to bundle stats class    //!< vectors for statistical computations...
Statistics m_Statsx;                       //!<  x errors
Statistics m_Statsy;                       //!<  y errors
Statistics m_Statsrx;                      //!<  x residuals
Statistics m_Statsry;                      //!<  y residuals
Statistics m_Statsrxy;                     //!< xy residuals
  };
}

#endif

