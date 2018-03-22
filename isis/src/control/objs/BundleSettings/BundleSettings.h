#ifndef BundleSettings_h
#define BundleSettings_h

/**
 * @file
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

// Qt library
#include <QList>
#include <QPair>
#include <QSharedPointer>
#include <QString>



// ISIS library
#include "BundleTargetBody.h"
#include "BundleObservationSolveSettings.h"
#include "MaximumLikelihoodWFunctions.h"
#include "SpecialPixel.h"
#include "XmlStackedHandler.h"

class QDataStream;
class QUuid;
class QXmlStreamWriter;


using namespace Isis;

namespace Isis {
  class FileName;
  class Project;
  class PvlObject;
  class XmlStackedHandlerReader;

  /**
   * @brief Container class for BundleAdjustment settings.
   * This class contains all of the settings needed to run a bundle adjustment.
   * A BundleSettings object is passed into the BundleAdjustment constructor.
   *
   * @ingroup ControlNetworks
   *
   * @author 2014-05-14 Jeannie Backer
   *
   * @internal
   *   @history 2014-05-14 Jeannie Backer - Original version.
   *   @history 2014-07-16 Jeannie Backer - Removed redundant mutators.  Moved implementation for
   *                           numberSolveSettings() and observationSolveSettings() to cpp file.
   *                           Removed static methods to convert MaximumLikelihoodWFunctions::Model
   *                           enum to QString since these methods now exist in
   *                           MaximumLikelihoodWFunctions class. Changed pvlGroup() method to
   *                           pvlObject().  Added unitTest.
   *   @history 2014-07-23 Jeannie Backer - Added QDataStream >> and << operators and read/write
   *                           methods. Created unitTest.
   *   @history 2014-07-25 Jeannie Backer - Improved unitTest coverage to 100% scope/line/function.
   *   @history 2014-11-17 Jeannie Backer - Added xml read/write capabilities. XmlHandler
   *                           constructor/destructor is not shown as covered by unitTest. Current
   *                           test code coverage is (scope 98.79%, line 98.698%, function 96.0%).
   *   @history 2015-09-03 Jeannie Backer - Changed a priori sigma defaults from -1.0 to Isis::Null.
   *   @history 2015-09-03 Jeannie Backer - Added preliminary hdf5 read/write capabilities.
   *   @history 2015-10-14 Jeffrey Covington - Declared BundleSettingsQsp as a
   *                           Qt metatype for use with QVariant.
   *   @history 2016-06-30 Jeannie Backer - Changed method name from "getBundleTargetBody" to
   *                           "bundleTargetBody" to comply with ISIS coding standards.
   *                           Added documentation. Updated test. References #3976. Fixes #.
   *   @history 2016-08-15 Jesse Mapel - Added methods to check if solving for triaxial radii,
   *                           mean radius, or Prime Meridian Acceleration. Fixes #4159.
   *   @history 2016-08-18 Jesse Mapel - Changed to no longer inherit from QObject.  Fixes #4192.
   *   @history 2016-08-18 Jeannie Backer - Removed the SolveMethod enum and all references to it.
   *                           This option was deprecated because the Sparse option is faster than
   *                           the other options (OldSparse and SpecialK) and gets identical
   *                           results. Fixes #4162.
   *   @history 2016-08-23 Jesse Mapel - Modified to no longer determine which output files
   *                           BundleAdjust and BundleSolutionInfo create.  Fixes #4279.
   *   @history 2016-09-02 Jesse Mapel - Added m_SCPVLFilename member for storing multi-sensor
   *                           settings from jigsaw.  Fixes #4316.
   *   @history 2016-10-05 Ian Humphrey - Added m_createInverseMatrix, createInverseMatrix(), and
   *                           setCreateInverseMatrix() so that bundle settings stores whether or
   *                           not the inverse correlation matrix file (inverseMatrix.dat) will be
   *                           generated during error propagation in the adjustment. Fixes #4315.
   *   @history 2016-10-13 Ian Humphrey - Updated documentation and variable names for
   *                           observationSolveSettings(), as BundleObservationSolveSettings
   *                           are acquired by an associated observation number. References #4293.
   *   @history 2016-10-17 Jesse Mapel - Removed m_SCPVLFilename parameter in accordance with
   *                           USEPVL being removed from jigsaw.  References #4316.
   *   @history 2017-04-24 Ian Humphrey - Removed pvlObject(). Fixes #4797.
   *   @history 2018-03-20 Ken Edmundson
   *                           1) Temporarily set default for m_createInverseMatrix to false. This
   *                              is for creating and displaying the correlation matrix, which is
   *                              currently not working.
   *                           2) commented out hdf5 header includes in cpp
   *
   *   @todo Determine which XmlStackedHandlerReader constructor is preferred
   *   @todo Determine which XmlStackedHandler needs a Project pointer (see constructors)
   *   @todo Determine whether QList<BundleObservationSolveSettings> m_observationSolveSettings
   *         should be a list of pointers, or a pointer to a list, or a pointer to a list of
   *         pointers, etc...
   *   @todo Determine whether QList< QPair< MaximumLikelihoodWFunctions::Model, double > >
   *         m_maximumLikelihood should be a list of pointers, or a pointer to a list, or a pointer
   *         to a list of pointers, etc...
   *   @todo TargetBody information is not being serialized. A determination needs to
   *         be made as to where it will be stored.
   */
  class BundleSettings {
    public:

      //=====================================================================//
      //================ constructors, destructor, operators ================//
      //=====================================================================//
      BundleSettings();
      BundleSettings(const BundleSettings &other);
      BundleSettings(Project *project,
                     XmlStackedHandlerReader *xmlReader);
#if 0
      BundleSettings(FileName xmlFile,
                     Project *project,
                     XmlStackedHandlerReader *xmlReader,
                     QObject *parent = NULL);
      BundleSettings(XmlStackedHandlerReader *xmlReader,
                     QObject *parent = NULL);
#endif
      ~BundleSettings();
      BundleSettings &operator=(const BundleSettings &other);

      void setValidateNetwork(bool validate);
      bool validateNetwork() const;


      //=====================================================================//
      //============================ Solve options ==========================//
      //=====================================================================//

      // mutators
      void setSolveOptions(bool solveObservationMode = false,
                           bool updateCubeLabel = false,
                           bool errorPropagation = false,
                           bool solveRadius = false,
                           double globalLatitudeAprioriSigma = Isis::Null,
                           double globalLongitudeAprioriSigma = Isis::Null,
                           double globalRadiusAprioriSigma = Isis::Null);
      void setOutlierRejection(bool outlierRejection,
                               double multiplier = 1.0);
      void setObservationSolveOptions(QList<BundleObservationSolveSettings> obsSolveSettingsList);
      void setCreateInverseMatrix(bool createMatrix);

      // accessors
      bool createInverseMatrix() const;
      bool solveObservationMode() const;
      bool solveRadius() const;
      bool updateCubeLabel() const;
      bool errorPropagation() const;
      bool outlierRejection() const;
      double outlierRejectionMultiplier() const;
      double globalLatitudeAprioriSigma() const;
      double globalLongitudeAprioriSigma() const;
      double globalRadiusAprioriSigma() const;

      int numberSolveSettings() const;
      BundleObservationSolveSettings observationSolveSettings(QString instrumentId) const;
      BundleObservationSolveSettings observationSolveSettings(int n) const;


      //=====================================================================//
      //======================- Convergence Criteria ========================//
      //=====================================================================//

      /**
       * This enum defines the options for the bundle adjustment's convergence.
       */
      enum ConvergenceCriteria {
        Sigma0,              /**< The value of sigma0 will be used to determine that the bundle
                                  adjustment has converged.*/
        ParameterCorrections /**< All parameter corrections will be used to determine that the
                                  bundle adjustment has converged.*/
      };


      static ConvergenceCriteria stringToConvergenceCriteria(QString criteria);
      static QString convergenceCriteriaToString(ConvergenceCriteria criteria);
      void setConvergenceCriteria(ConvergenceCriteria criteria,
                                  double threshold,
                                  int maximumIterations);
      ConvergenceCriteria convergenceCriteria() const;
      double convergenceCriteriaThreshold() const;
      int convergenceCriteriaMaximumIterations() const;

      //=====================================================================//
      //================ Parameter Uncertainties (Weighting) ================//
      //=====================================================================//
      // mutators
//      void setGlobalLatitudeAprioriSigma(double sigma);
//      void setGlobalLongitudeAprioriSigma(double sigma);
//      void setGlobalRadiiAprioriSigma(double sigma);


      //=====================================================================//
      //=============== Maximum Likelihood Estimation Options ===============//
      //=====================================================================//

      /**
       * This enum defines the options for maximum likelihood estimation.
       */
      enum MaximumLikelihoodModel {
        NoMaximumLikelihoodEstimator, /**< Do not use a maximum likelihood model.*/
        Huber,                        /**< Use a Huber maximum likelihood model. This model
                                           approximates the L2 norm near zero and the L1 norm
                                           thereafter. This model has one continuous derivative.*/
        ModifiedHuber,                /**< Use a modified Huber maximum likelihood model. This model
                                           approximates the L2 norm near zero and the L1 norm
                                           thereafter. This model has two continuous derivative.*/
        Welsch,                       /**< Use a Welsch maximum likelihood model. This model
                                           approximates the L2 norm near zero, but then decays
                                           exponentially to zero.*/
        Chen                          /**< Use a Chen maximum likelihood model. This is a highly
                                           aggressive model that intentionally removes the largest
                                           few percent of residuals.???? */
      };


      void addMaximumLikelihoodEstimatorModel(MaximumLikelihoodWFunctions::Model model,
                                              double cQuantile);
      QList< QPair< MaximumLikelihoodWFunctions::Model, double > >
          maximumLikelihoodEstimatorModels() const;

      //=====================================================================//
      //========================= Self Calibration ==========================//
      //=====================================================================//


      //=====================================================================//
      //============================== Target Body ==========================//
      //=====================================================================//
      // Note targeBody information is not currently serialized.
      void setBundleTargetBody(BundleTargetBodyQsp bundleTargetBody);
      BundleTargetBodyQsp bundleTargetBody() const;
//      bool solveTargetBodyPolePosition() const;
//      static TargetRadiiSolveMethod stringToTargetRadiiOption(QString option);
//      static QString targetRadiiOptionToString(TargetRadiiSolveMethod targetRadiiSolveMethod);
      int numberTargetBodyParameters() const;
      bool solveTargetBody() const;
      bool solvePoleRA() const;
      bool solvePoleRAVelocity() const;
      bool solvePoleDec() const;
      bool solvePoleDecVelocity() const;
      bool solvePM() const;
      bool solvePMVelocity() const;
      bool solvePMAcceleration() const;
      bool solveTriaxialRadii() const;
      bool solveMeanRadius() const;
//  void BundleSettings::setTargetBodySolveOptions(bool solveTargetBodyPolePosition,


      //=====================================================================//
      //================== Output Options ??? (from Jigsaw only)=============//
      //=====================================================================//
      void setOutputFilePrefix(QString outputFilePrefix);
      void setSCPVLFilename(QString SCParamFilename);
      QString outputFilePrefix() const;
      QString SCPVLFilename() const;

      void save(QXmlStreamWriter &stream, const Project *project) const;

    private:
      void init();

      //=====================================================================//
      //============= Saving/Restoring a BundleSettings object ==============//
      //=====================================================================//

      /**
       * This class is needed to read/write BundleSettings from/to an XML
       * formateed file.
       *
       * @author 2014-07-21 Ken Edmundson
       *
       * @internal
       *   @history 2014-07-21 Ken Edmundson - Original version.
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(BundleSettings *bundleSettings,
                     Project *project);
          XmlHandler(BundleSettings *bundleSettings);
          ~XmlHandler();

          virtual bool startElement(const QString &namespaceURI,
                                    const QString &localName,
                                    const QString &qName,
                                    const QXmlAttributes &atts);
          virtual bool characters(const QString &ch);
          virtual bool endElement(const QString &namespaceURI,
                                  const QString &localName,
                                  const QString &qName);
          bool fatalError(const QXmlParseException &exception);

        private:
          Q_DISABLE_COPY(XmlHandler);

          BundleSettings *m_xmlHandlerBundleSettings ;
          Project *m_xmlHandlerProject; // TODO: does xml stuff need project???
          QString m_xmlHandlerCharacters;
          QList<BundleObservationSolveSettings *> m_xmlHandlerObservationSettings;
      };


      /**
       * This struct is needed to write the m_maximumLikelihood variable as an
       * HDF5 table. Each table record has 3 field values: index, name, and
       * quantile.
       */
      struct MaximumLikelihoodModelTableRecord {
          unsigned int indexFieldValue; //!< The index of the TableRecord.???
          QString nameFieldValue; //!< The model name of the TableRecord.???
          double quantileFieldValue; //!< The quantile of the TableRecord.???
      };

      bool m_validateNetwork; //!< Indicates whether the network should be validated.
      bool m_solveObservationMode; //!< Indicates whether to solve for observation mode.
      bool m_solveRadius; //!< Indicates whether to solve for point radii.
      bool m_updateCubeLabel; //!< Indicates whether to update cubes.
      bool m_errorPropagation; //!< Indicates whether to perform error propagation.
      bool m_createInverseMatrix; //!< Indicates whether to create the inverse matrix file.
      bool m_outlierRejection; /**< Indicates whether to perform automatic
                                    outlier detection/rejection.*/
      double m_outlierRejectionMultiplier; /**< The multiplier value for outlier rejection.
                                                Defaults to 1, so no change if rejection = false.*/

      // Parameter Uncertainties (Weighting)
      double m_globalLatitudeAprioriSigma;  //!< The global a priori sigma for latitude.
      double m_globalLongitudeAprioriSigma; //!< The global a priori sigma for longitude.
      double m_globalRadiusAprioriSigma;    //!< The global a priori sigma for radius.

      // QList of observation solve settings
      QList<BundleObservationSolveSettings> m_observationSolveSettings; //!<

      // Convergence Criteria
      ConvergenceCriteria m_convergenceCriteria;  /**< Enumeration used to indicate what criteria
                                                       to use to determine bundle
                                                       adjustment convergence.*/
      double m_convergenceCriteriaThreshold;      /**< Tolerance value corresponding to the selected
                                                       convergence criteria.*/
      int m_convergenceCriteriaMaximumIterations; /**< Maximum number of iterations before
                                                       quitting the bundle adjustment if it has
                                                       not yet converged to the given threshold.*/

      // Maximum Likelihood Estimation Options
      /**
       * Model and C-Quantile for each of the three maximum likelihood
       * estimations. The C-Quantile is the quantile of the residual used
       * to compute the tweaking constant. Note that this is an ordered
       * list and that the Welsch and Chen models can not be used for the
       * first model.
       */
      QList< QPair< MaximumLikelihoodWFunctions::Model, double > > m_maximumLikelihood;

      // Self Calibration

      // Target Body
      bool m_solveTargetBody; //!< Indicates whether to solve for target body.
      BundleTargetBodyQsp m_bundleTargetBody; /**< A pointer to the target body
                                                   settings and information.*/

      // Output Options
      QString m_outputFilePrefix;    /**< The prefix for all output files. If the user does not want
                                          output files to be written to the current directory, the
                                          output directory path should be included in this prefix.*/
 };
  // typedefs
  //! Definition for a BundleSettingsQsp, a shared pointer to a BundleSettings object.
  typedef QSharedPointer<BundleSettings> BundleSettingsQsp;


};

Q_DECLARE_METATYPE(Isis::BundleSettingsQsp);

#endif
