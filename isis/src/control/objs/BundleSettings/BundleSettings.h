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

#include <QList>
#include <QPair>
#include <QObject>
#include <QString>

#include <H5Cpp.h>
#include <hdf5_hl.h>
#include <hdf5.h>

#include "BundleObservationSolveSettings.h"
#include "MaximumLikelihoodWFunctions.h" // why not forward declare???
#include "PvlObject.h"
#include "SpecialPixel.h"
#include "XmlStackedHandler.h"

class QDataStream;
class QUuid;
class QXmlStreamWriter;

using namespace H5;
using namespace Isis;

namespace Isis {
  class FileName;
  class MaximumLikelihoodWFunctions;
  class Project;  // TODO: does xml stuff need project???
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
   *  
   *   @todo Determine whether xml stuff needs a Project pointer
   *   @todo Determine which XmlStackedHandlerReader constructor is preferred
   *  
   */
  class BundleSettings : public QObject {
    Q_OBJECT
    public:
      BundleSettings(QObject *parent = 0);
      BundleSettings(const BundleSettings &src);
      BundleSettings(Project *project, 
                     XmlStackedHandlerReader *xmlReader, 
                     QObject *parent = 0);  // TODO: does xml stuff need project???
      BundleSettings(FileName xmlFile,
                     Project *project, 
                     XmlStackedHandlerReader *xmlReader, 
                     QObject *parent = 0);  // TODO: does xml stuff need project???
      BundleSettings(XmlStackedHandlerReader *xmlReader, QObject *parent = NULL); // parent = 0 or NULL ???
      ~BundleSettings();

      // copy constructor
      BundleSettings &operator=(const BundleSettings &src);

      void setValidateNetwork(bool validate);
      bool validateNetwork() const;

      // Solve Options
      /**
       * This enum defines the types of matrix decomposition methods for solving
       * the bundle. 
       */
      enum SolveMethod {
        Sparse,   //!< Cholesky model sparse normal equations matrix. (Uses the cholmod library).
        SpecialK, //!< Dense normal equations matrix.
      };
      // converter
      static SolveMethod stringToSolveMethod(QString solveMethod);
      static QString solveMethodToString(SolveMethod solveMethod);

      // mutators
      void setSolveOptions(SolveMethod method = Sparse, 
                           bool solveObservationMode = false,
                           bool updateCubeLabel = false, 
                           bool errorPropagation = false,
                           bool solveRadius = false, 
                           double globalLatitudeAprioriSigma = Isis::Null, 
                           double globalLongitudeAprioriSigma = Isis::Null, 
                           double globalRadiusAprioriSigma = Isis::Null);
      void setOutlierRejection(bool outlierRejection, double multiplier = 1.0);
      void setObservationSolveOptions(QList<BundleObservationSolveSettings> observationSolveSettings);

      // accessors
      SolveMethod solveMethod() const;
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

      // Convergence Criteria
      /**
       * This enum defines the options for convergence. 
       */
      enum ConvergenceCriteria {
        Sigma0,              /**< Sigma0 will be used to determine that the bundle adjustment has 
                                  converged.*/
        ParameterCorrections /**< All parameter corrections will be used to determine that the 
                                  bundle adjustment has converged.*/
      };
      static ConvergenceCriteria stringToConvergenceCriteria(QString criteria);
      static QString convergenceCriteriaToString(ConvergenceCriteria criteria);
      void setConvergenceCriteria(ConvergenceCriteria criteria, double threshold, 
                                  int maximumIterations);
      ConvergenceCriteria convergenceCriteria() const;
      double convergenceCriteriaThreshold() const;
      int convergenceCriteriaMaximumIterations() const;

      // Parameter Uncertainties (Weighting)
      // mutators
//      void setGlobalLatitudeAprioriSigma(double sigma);
//      void setGlobalLongitudeAprioriSigma(double sigma);
//      void setGlobalRadiiAprioriSigma(double sigma);

      // Maximum Likelihood Estimation Options
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

      // Self Calibration ??? (from cnetsuite only)

      // Target Body ??? (from cnetsuite only)

      // Output Options ??? (from Jigsaw only)
      void setOutputFiles(QString outputFilePrefix, bool createBundleOutputFile, 
                          bool createCSVPointsFile, bool createResidualsFile);
      QString outputFilePrefix() const;
      bool createBundleOutputFile() const;
      bool createCSVFiles() const;
      bool createResidualsFile() const;

      PvlObject pvlObject(QString name = "BundleSettings") const;

      void save(QXmlStreamWriter &stream, const Project *project) const;

      QDataStream &write(QDataStream &stream) const;
      QDataStream &read(QDataStream &stream);

      void savehdf5(hid_t fileId, H5::Group settingsGroup) const;
      void savehdf5(hid_t settingsGroupId, QString objectName) const;

    private:
      /**
       *
       * @author 2014-07-21 Ken Edmundson
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(BundleSettings *bundleSettings, Project *project);
          XmlHandler(BundleSettings *bundleSettings);
          ~XmlHandler();
   
          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool characters(const QString &ch);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
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
       * A unique ID for this BundleSettings object (useful for others to reference this object
       *   when saving to disk).
       */
      QUuid *m_id;

      bool m_validateNetwork;
      SolveMethod m_solveMethod; //!< Solution method for matrix decomposition.
      bool m_solveObservationMode; //!< for observation mode (explain this somewhere)
      bool m_solveRadius; //!< to solve for point radii
      bool m_updateCubeLabel; //!< update cubes (only here for output into bundleout.txt)
      bool m_errorPropagation; //!< to perform error propagation
      bool m_outlierRejection; //!< to perform automatic outlier detection/rejection
      double m_outlierRejectionMultiplier; // multiplier = 1 if rejection = false

      // Parameter Uncertainties (Weighting)
      double m_globalLatitudeAprioriSigma;
      double m_globalLongitudeAprioriSigma;
      double m_globalRadiusAprioriSigma;

      // QList of observation solve settings
      QList<BundleObservationSolveSettings> m_observationSolveSettings; // TODO: pointer???

      // Convergence Criteria
      ConvergenceCriteria m_convergenceCriteria;
      double m_convergenceCriteriaThreshold;
      int m_convergenceCriteriaMaximumIterations;

      // Maximum Likelihood Estimation Options
      /**
       * Model and C-Quantile for each of the three maximum likelihood 
       * estimations. The C-Quantile is the quantile of the residual used 
       * to compute the tweaking constant. Note that this is an ordered 
       * list and that the Welsch and Chen models can not be used for the 
       * first model.
       */
      QList< QPair< MaximumLikelihoodWFunctions::Model, double > > m_maximumLikelihood; // TODO: pointer???

      // Self Calibration ??? (from cnetsuite only)

      // Target Body ??? (from cnetsuite only)

      // Output Options ??? (from Jigsaw only)
      QString m_outputFilePrefix; //!< output file prefix
      bool m_createBundleOutputFile; //!< to print standard bundle output file (bundleout.txt)
      bool m_createCSVFiles; //!< to output points and image station data in csv format
      bool m_createResidualsFile; //!< to output residuals in csv format

 };
  // operators to read/write BundleResults to/from binary data
  QDataStream &operator<<(QDataStream &stream, const BundleSettings &settings);
  QDataStream &operator>>(QDataStream &stream, BundleSettings &settings);
};
#endif

