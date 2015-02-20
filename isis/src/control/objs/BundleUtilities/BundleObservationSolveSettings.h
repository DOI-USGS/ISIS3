#ifndef BundleObservationSolveSettings_h
#define BundleObservationSolveSettings_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2014/5/28 09:31:00 $
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
#include <QObject>
#include <QString>

#include "BundleImage.h"
#include "SpicePosition.h"
#include "SpiceRotation.h"
#include "XmlStackedHandler.h"

class QDataStream;
class QUuid;
class QXmlStreamWriter;

namespace Isis {
  class FileName;
  class Project;  // TODO: does xml stuff need project???
  class PvlObject;
  class XmlStackedHandlerReader;
  /**
   * @brief 
   *  
   * @ingroup ControlNetworks
   *
   * @author 2014-07-09 Ken Edmundson
   *
   * @internal
   *   @history 2014-07-09 Ken Edmundson - Original version.
   *   @history 2014-07-23 Jeannie Backer - Replace QVectors with QLists. Moved copy
   *                           constructor above destructor. Moved operator= implementation
   *                           to cpp file. Added methods to set/get instrument Id. Created
   *                           setInstrumentPointingSettings() and
   *                           setInstrumentPositionSettings() methods to check for variable
   *                           dependencies before setting. Reordered so that all position
   *                           methods were together and all pointing methods were together.
   *                           Updated documentation. Added QDataStream >> and << operators
   *                           and read/write methods. Added unitTest for BundleUtilities
   *                           BundleObservationSolveSettings class.
   *   @history 2014-07-25 Jeannie Backer - For enums < 4, set solve degrees one less than
   *                           enum value.
   *   @history 2014-12-02 Jeannie Backer - Undo last modification. While it makes sense, it
   *                           was causing jigsaw to fail. Brought test coverage of this
   *                           class to 99.403% scope.
   *  
   *  
   *   @todo Figure out why solve degree and num coefficients does not match solve option.
   *   @todo Determine whether xml stuff needs a Project pointer
   *   @todo Determine which XmlStackedHandlerReader constructor is preferred
   */

  class BundleObservationSolveSettings : public QObject {
    Q_OBJECT
    public:
      BundleObservationSolveSettings(QObject *parent = 0);
      BundleObservationSolveSettings(Project *project, 
                                     XmlStackedHandlerReader *xmlReader, 
                                     QObject *parent = 0);  // TODO: does xml stuff need project???
      BundleObservationSolveSettings(FileName xmlFile, 
                                     Project *project, 
                                     XmlStackedHandlerReader *xmlReader, 
                                     QObject *parent = 0);  // TODO: does xml stuff need project???
      BundleObservationSolveSettings(const BundleObservationSolveSettings &src);
      ~BundleObservationSolveSettings();
      BundleObservationSolveSettings &operator=(const BundleObservationSolveSettings &src);
      void initialize();

      bool setFromPvl(PvlGroup &scParameterGroup);
      PvlObject pvlObject(QString name = "") const; // default name is instrument ID

      void setInstrumentId(QString instrumentId);
      QString instrumentId() const;



      // Instrument Pointing stuff
      enum InstrumentPointingSolveOption {
        NoPointingFactors          = 0, /**< Solve for none of the pointing factors.*/
        AnglesOnly                 = 1, /**< Solve for pointing angles: right ascension, declination
                                             and, optionally, twist.*/
        AnglesVelocity             = 2, //!< Solve for pointing angles and their angular velocities.
        AnglesVelocityAcceleration = 3, /**< Solve for pointing angles, their velocities and their
                                             accelerations.*/
        AllPointingCoefficients    = 4 /**< Solve for all coefficients in the polynomials fit to
                                             the pointing angles.*/
      };
      static InstrumentPointingSolveOption stringToInstrumentPointingSolveOption(QString option);
      static QString instrumentPointingSolveOptionToString(InstrumentPointingSolveOption option);
      void setInstrumentPointingSettings(InstrumentPointingSolveOption option, 
                                         bool solveTwist, 
                                         int ckDegree = 2, 
                                         int ckSolveDegree = 2,
                                         bool solvePolynomialOverExisting = false, 
                                         double anglesAprioriSigma = -1.0,
                                         double angularVelocityAprioriSigma = -1.0, 
                                         double angularAccelerationAprioriSigma = -1.0);
      InstrumentPointingSolveOption instrumentPointingSolveOption() const;
      bool solveTwist() const;
      int ckDegree() const;
      int ckSolveDegree() const;
      int numberCameraAngleCoefficientsSolved() const;
      bool solvePolyOverPointing() const;
      QList<double> aprioriPointingSigmas() const;
      SpiceRotation::Source pointingInterpolationType() const;



      // Instrument Position stuff
      enum InstrumentPositionSolveOption {
        NoPositionFactors            = 0, /**< Solve for none of the position factors.*/
        PositionOnly                 = 1, /**< Solve for instrument positions only.*/
        PositionVelocity             = 2, /**< Solve for instrument positions and velocities.*/
        PositionVelocityAcceleration = 3, /**< Solve for instrument positions, velocities, and
                                               accelerations.*/
        AllPositionCoefficients      = 4  /**< Solve for all coefficients in the polynomials fit to
                                               the instrument positions.*/
      };
      static InstrumentPositionSolveOption stringToInstrumentPositionSolveOption(QString option);
      static QString instrumentPositionSolveOptionToString(InstrumentPositionSolveOption option);
      void setInstrumentPositionSettings(InstrumentPositionSolveOption option,
                                         int spkDegree = 2,
                                         int spkSolveDegree = 2,
                                         bool positionOverHermite = false,
                                         double positionAprioriSigma = -1.0,
                                         double velocityAprioriSigma = -1.0,
                                         double accelerationAprioriSigma = -1.0);
      InstrumentPositionSolveOption instrumentPositionSolveOption() const;
      int spkDegree() const;
      int spkSolveDegree() const;
      int numberCameraPositionCoefficientsSolved() const;
      bool solvePositionOverHermite() const;
      QList<double> aprioriPositionSigmas() const;
      SpicePosition::Source positionInterpolationType() const;

      void save(QXmlStreamWriter &stream, const Project *project) const;  // TODO: does xml stuff need project???

      QDataStream &write(QDataStream &stream) const;
      QDataStream &read(QDataStream &stream);

    private:
      /**
       *
       * @author 2014-07-28 Jeannie Backer
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(BundleObservationSolveSettings *settings, Project *project);  // TODO: does xml stuff need project???
          ~XmlHandler();
   
          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool characters(const QString &ch);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName);
   
        private:
          Q_DISABLE_COPY(XmlHandler);
   
          BundleObservationSolveSettings *m_xmlHandlerObservationSettings;
          Project *m_xmlHandlerProject;  // TODO: does xml stuff need project???
          QString m_xmlHandlerCharacters;
          QStringList m_xmlHandlerAprioriSigmas;
      };

      /**
       * A unique ID for this object (useful for others to reference this object
       *   when saving to disk).
       */
      QUuid *m_id;
      QString m_instrumentId;               //!< The spacecraft instrument id for this observation.

      // pointing related parameters
      InstrumentPointingSolveOption m_instrumentPointingSolveOption;
      int m_numberCamAngleCoefSolved;             /**< The number of camera angle coefficients in solution.*/
      int m_ckDegree;                             //!< ck degree (define)
      int m_ckSolveDegree;                        //!< solve ck degree (define)
      bool m_solveTwist;                          //!< Solve for "twist" angle.
      bool m_solvePointingPolynomialOverExisting; /**< The polynomial will be fit over the existing 
                                                       pointing polynomial.*/
      QList<double> m_anglesAprioriSigma; /**< The image position a priori sigmas.The size of the
                                               list is equal to the number of coefficients in the
                                               solution. A negative value implies no weighting will
                                               be applied.*/ // ??? no negatives are filled in when size = 0
      SpiceRotation::Source
          m_pointingInterpolationType;    /**< SpiceRotation interpolation type.
                                                 Defined in SpiceRotation.cpp, these types are:
                                                 1) Spice: directly from kernels,
                                                 2) Nadir: Nadir pointing,
                                                 3) Memcache: from cached table,
                                                 4) PolyFunction: from nth degree polynomial,
                                                 5) PolyFunctionOverSpice: kernels plus nth degree
                                                    polynomial.*/

      // position related parameters
      InstrumentPositionSolveOption m_instrumentPositionSolveOption;
      int m_numberCamPosCoefSolved;          /**< The number of camera position coefficients in the
                                                  solution.*/
      int m_spkDegree;                       //!< spk degree (define)
      int m_spkSolveDegree;                  //!< solve spk degree (define)
      bool m_solvePositionOverHermiteSpline; /**< The polynomial will be fit over an existing
                                                  Hermite spline.*/
      QList<double> m_positionAprioriSigma; /**< The instrument pointing a priori sigmas. The
                                                   size of the list is equal to the number of
                                                   coefficients in the solution. A negative value
                                                   implies no weighting will be applied.*/ // ??? no negatives are filled in when size = 0
      SpicePosition::Source
          m_positionInterpolationType;      /**< SpicePosition interpolation types.
                                                 Defined in SpicePosition.cpp, these types are:
                                                 1) Spice: read directly from kernels,
                                                 2) Memcache: read from cached table,
                                                 3) HermiteCache: read from splined table,
                                                 4) PolyFunction: calced from nth degree polynomial,
                                                 5) PolyFunctionOverHermiteConstant: read from
                                                    splined table and adding nth degree 
                                                    polynomial.*/ 

  };
  // operators to read/write BundleResults to/from binary data
  QDataStream &operator<<(QDataStream &stream, const BundleObservationSolveSettings &settings);
  QDataStream &operator>>(QDataStream &stream, BundleObservationSolveSettings &settings);
};

#endif // BundleObservationSolveSettings_h
