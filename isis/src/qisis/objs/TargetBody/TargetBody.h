#ifndef TargetBody_h
#define TargetBody_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>

#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QString>

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

#include "Angle.h"
//#include "BundleTargetBody.h"
#include "Distance.h"
#include "Target.h"
#include "XmlStackedHandler.h"


class QDataStream;
class QUuid;
class QXmlStreamWriter;

namespace Isis {
  //class Distance;
  class FileName;
  class Project;  // TODO: does xml stuff need project???
  class PvlObject;
  class TargetBodyDisplayProperties;
  class XmlStackedHandlerReader;

  /**
   * @brief Container class for TargetBody.
   *
   * This class represents a target body in a project-based GUI interface. It encapsulates ideas
   *   about a target such as it's display name, how it should be viewed, where it is on disk, etc.
   *
   *
   * @ingroup qisis
   *
   * @author 2015-06-08 Ken Edmundson
   *
   * @internal
   *   @history 2015-06-08 Ken Edmundson - Original version.
   *   @history 2015-10-14 Jeffrey Covington - Declared TargetBodyQsp as a Qt
   *                           metatype for use with QVariant.
   *   @history 2016-06-13 Tyler Wilson - Added new documentation and corrected
   *                          formatting to be consisten with ISIS3 coding standards.
   *                          Fixes #3997 and #4018.
   *   @hitsory 2018-07-12 Summer Stapleton - Added m_targetName and targetName() in order to
   *                           collect the TargetName from the original cube label for
   *                           comparisons related to image imports in ipce. References #5460.
   *
   */
  class TargetBody : public QObject {

    Q_OBJECT

    public:
      TargetBody(Target *target, QObject *parent = 0);
      //TargetBody(BundleTargetBodyQsp bundleTargetBody, QObject *parent = 0);
//      TargetBody(Project *project, XmlStackedHandlerReader *xmlReader,
//                 QObject *parent = 0);  // TODO: does xml stuff need project???
      ~TargetBody();

      bool operator==(const TargetBody &src) const;

      TargetBodyDisplayProperties *displayProperties();
      const TargetBodyDisplayProperties *displayProperties() const;

      QString id() const;
      QString targetName();
//    void deleteFromDisk();

      int frameType();

      std::vector<Angle> poleRaCoefs();
      std::vector<Angle> poleDecCoefs();
      std::vector<Angle> pmCoefs();

      std::vector<double> poleRaNutPrecCoefs();
      std::vector<double> poleDecNutPrecCoefs();

      std::vector<double> pmNutPrecCoefs();

      std::vector<Angle> sysNutPrecConstants();
      std::vector<Angle> sysNutPrecCoefs();

      SpiceInt naifBodyCode() const;
      SpiceInt naifPlanetSystemCode() const;
      QString naifPlanetSystemName() const;
      Distance radiusA() const;
      Distance radiusB() const;
      Distance radiusC() const;
      Distance meanRadius() const;
      Distance sigmaRadiusA() const;
      Distance sigmaRadiusB() const;
      Distance sigmaRadiusC() const;
      Distance sigmaMeanRadius() const;

//      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;
//      TODO: does xml stuff need project and newRoot???
//      void save(QXmlStreamWriter &stream, const Project *project) const;
//      TODO: does xml stuff need project???
//
//      QDataStream &write(QDataStream &stream) const;
//      QDataStream &read(QDataStream &stream);

    private:
      /**
       *
       * @author 2015-06-08 Ken Edmundson
       *
       * @internal
       */
//      class XmlHandler : public XmlStackedHandler {
//        public:
//          XmlHandler(TargetBody *TargetBody, Project *project);
//      TODO: does xml stuff need project???
//          ~XmlHandler();
//
//          virtual bool startElement(const QString &namespaceURI, const QString &localName,
//                                    const QString &qName, const QXmlAttributes &atts);
//          virtual bool characters(const QString &ch);
//          virtual bool endElement(const QString &namespaceURI, const QString &localName,
//                                    const QString &qName);
//
//        private:
//          Q_DISABLE_COPY(XmlHandler);
//
//          TargetBody *m_xmlHandlerTargetBody;
//          Project *m_xmlHandlerProject;  // TODO: does xml stuff need project???
//          QString m_xmlHandlerCharacters;
//      };

    private:
      TargetBody(const TargetBody &other);          // NOTE: copy constructor & assignment operators
      TargetBody &operator=(const TargetBody &rhs); // are private so compiler will generate error
                                                    // if trying to use them (because parent is
                                                    // QObject which uses Q_DISABLE_COPY macro
      //testing
      TargetQsp m_isisTarget;

      /**
       * A unique ID for this TargetBody object (useful for others to reference this object
       *   when saving to disk).
       */
      QUuid *m_id;

      /**
       * The TargetName as it appears in the original cube.
       */
      QString m_targetName;

      /**
       * TODO -   RETHINK MEMBER VARIABLES AND METHODS
       * The NaifBodyCode value, if it exists in the cube labels. Otherwise, if the target is sky,
       *   it's the SPK code and if not sky then it's calculated by the NaifBodyCode() method.
       */
      SpiceInt *m_bodyCode;

      /**
       * The NaifBodyCode system code. If the target is sky, this is -1.
       */
      SpiceInt *m_systemCode;

      /**
       * The NaifBodyCode system name.
       */
      QString m_systemName;

      /**
       * target radii
       */
      std::vector<Distance> m_radii;

      /**
       * target radii sigmas
       */
      std::vector<Distance> m_sigmaRadii;

      /**
       * The GUI information for how this Target will be displayed.
       */
      TargetBodyDisplayProperties *m_displayProperties;

      int m_frametype;  //!< Fill this in when Debbie or Ken tell me what it is returning.

      // The next three vectors will have length 3 (for a quadratic polynomial) if used.
      std::vector<Angle> m_raPole;      //!< Coefficients of a quadratic polynomial fitting pole ra
      std::vector<Angle> m_decPole;     //!< Coefficients of a quadratic polynomial fitting pole dec
      std::vector<Angle> m_pm ;         //!< Coefficients of a quadratic polynomial fitting pole pm
      //
      // Currently multiples (terms with periods matching other terms but varying amplitudes)
      // are handled as additional terms added to the end of the vector as Naif does (see
      // comments in any of the standard Naif PCK.
      std::vector<double> m_raNutPrec;   //!< Coefficients of pole right ascension nut/prec terms.
      std::vector<double> m_decNutPrec;  //!< Coefficients of pole decliniation nut/prec terms.
      std::vector<double> m_pmNutPrec;   //!< Coefficients of prime meridian nut/prec terms.

      // The periods of bodies in the same system are modeled with a linear equation
      std::vector<Angle> m_sysNutPrec0; //!< Constants of planetary system nut/prec periods
      std::vector<Angle> m_sysNutPrec1; //!< Linear terms of planetary system nut/prec periods
  };

  typedef QSharedPointer<TargetBody> TargetBodyQsp;//!< Defines A smart pointer to a TargetBody obj

  //

  // operators to read/write TargetBody to/from binary data
  QDataStream &operator<<(QDataStream &stream, const TargetBody &TargetBody);
  QDataStream &operator>>(QDataStream &stream, TargetBody &TargetBody);
};

Q_DECLARE_METATYPE(Isis::TargetBodyQsp);

#endif // TargetBody_h
