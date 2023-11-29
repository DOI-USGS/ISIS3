#include "TargetBody.h"

#include <QDataStream>
#include <QDebug>
#include <QList>
#include <QUuid>
#include <QXmlStreamWriter>

#include "Distance.h"
#include "IString.h"
#include "Project.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "TargetBodyDisplayProperties.h"

namespace Isis {
/**
   * @brief The first constructor for this class.
   * @param target A traditional: Isis::Target object.
   * @param parent A pointer to the object instantiating this object.
   */
TargetBody::TargetBody(Target *target, QObject *parent) : QObject(parent) {
    m_id = NULL;

    m_radii.resize(3, Distance());
    m_sigmaRadii.resize(3, Distance(0.0, Distance::Kilometers));

    m_bodyCode = new SpiceInt;
    m_systemCode = new SpiceInt;
    
    m_targetName = target->name();

    m_systemName = target->systemName();

    m_frametype = target->frameType();

    // TODO - initialize TargetBody members from target
    *m_bodyCode = target->naifBodyCode();
    *m_systemCode = target->naifPlanetSystemCode();
    m_radii[0] = target->radii().at(0);
    m_radii[1] = target->radii().at(1);
    m_radii[2] = target->radii().at(2);

    m_raPole = target->poleRaCoefs();
    m_decPole = target->poleDecCoefs();
    m_pm = target->pmCoefs();

    m_raNutPrec = target->poleRaNutPrecCoefs();
    m_decNutPrec = target->poleDecNutPrecCoefs();
    m_pmNutPrec = target->pmNutPrecCoefs();

    m_sysNutPrec0 = target->sysNutPrecConstants();
    m_sysNutPrec1 = target->sysNutPrecCoefs();

    m_displayProperties
        = new TargetBodyDisplayProperties(target->name(), this);

    m_id = new QUuid(QUuid::createUuid());
  }


/**
   * @brief The second constructor for this class.
   * @param bundleTargetBody A QSharedPointer to a BundleTarget object.
   * @param parent A pointer to the object instantiating this object.
   */
/*
  TargetBody::TargetBody(BundleTargetBodyQsp bundleTargetBody, QObject *parent) : QObject(parent)
  {
    m_id = NULL;

    m_radii.resize(3, Distance());
    m_sigmaRadii.resize(3, Distance(0.0, Distance::Kilometers));

    m_bodyCode = new SpiceInt;
    m_systemCode = new SpiceInt;

//    m_systemName = bundleTargetBody->systemName();

    // TODO - initialize TargetBody members from bundleTargetBody
//    *m_bodyCode = target->naifBodyCode();
//    *m_systemCode = target->naifPlanetSystemCode();
    m_radii = bundleTargetBody->radii();
//    m_sigmaRadii = bundleTargetBody->

    m_raPole = bundleTargetBody->poleRaCoefs();
    m_decPole = bundleTargetBody->poleDecCoefs();
    m_pm = bundleTargetBody->pmCoefs();

//    m_raNutPrec = bundleTargetBody->poleRaNutPrecCoefs();
//    m_decNutPrec = bundleTargetBody->poleDecNutPrecCoefs();
//    m_pmNutPrec = bundleTargetBody->pmNutPrecCoefs();

//    m_sysNutPrec0 = bundleTargetBody->sysNutPrecConstants();
//    m_sysNutPrec1 = bundleTargetBody->sysNutPrecCoefs();

//    m_displayProperties
//        = new TargetBodyDisplayProperties(target->name(), this);

    m_id = new QUuid(QUuid::createUuid());
  }
*/



//  TargetBody::TargetBody(const TargetBody &src)
//      : m_id(new QUuid(src.m_id->toString())) {
//
//    m_bodyCode = new SpiceInt(*src.m_bodyCode);
//
//    m_radii.resize(3, Distance());
//    m_sigmaRadii.resize(3, Distance(3.0, Distance::Kilometers));
// TODO - radii sigma fudged for now
//
//    for (int i = 0; i < 3; i++) {
//      m_radii[i] = src.m_radii[i];
//      m_sigmaRadii[i] = src.m_sigmaRadii[i];
//    }
//
//    m_displayProperties
//        = new TargetBodyDisplayProperties(*src.m_displayProperties);
//
//    int fred=1;
//    m_bodyCode = src.m_bodyCode;
//  }


  /**
   * @brief The destructor
   */
  TargetBody::~TargetBody() {
    delete m_id;
    m_id = NULL;

    delete m_bodyCode;
    m_bodyCode = NULL;

    delete m_systemCode;
    m_systemCode = NULL;
  }


//  TargetBody &TargetBody::operator=(const TargetBody &src) {

//    if (&src != this) {

//      delete m_id;
//      m_id = NULL;
//      m_id = new QUuid(src.m_id->toString());
//    }

//    return *this;
//  }


  /**
   * @brief Compares two Target Body objects to see if they are equal
   *
   * @param src TargetBody object to compare against
   *
   * @return @b bool Returns true if the objects are equal, false if not.
   */
  bool TargetBody::operator==(const TargetBody &src) const {

    TargetBody *rtargetBody = (TargetBody *) &src;

    if (*m_bodyCode != rtargetBody->naifBodyCode())
      return false;
    if (m_radii[0] != rtargetBody->radiusA())
      return false;
    if (m_radii[1] != rtargetBody->radiusB())
      return false;
    if (m_radii[2] != rtargetBody->radiusC())
      return false;

    return true;
  }


  /**
   * @brief Gets TargetBodyDisplayProperties.
   * @return @b TargetBodyDisplayProperties * A pointer to the display properties for the
   * TargetBody.
   */
  TargetBodyDisplayProperties *TargetBody::displayProperties() {
    return m_displayProperties;
  }


  /**
   * @brief TargetBody::displayProperties
   * @return @b TargetBodyDisplayProperties * A pointer to the display properties for the
   * TargetBody.
   */
  const TargetBodyDisplayProperties *TargetBody::displayProperties() const {
    return m_displayProperties;
  }
  
  
  /**
   * @brief Returns the value stored at TargetName in the original pvl label.
   * @return QString Returns m_targetName
   */
  QString TargetBody::targetName() {
    return m_targetName;
  }


  /**
   * @brief Returns the frame type.
   * @return @b
   */
  int TargetBody::frameType() {
    return m_frametype;
  }


  /**
   * @brief TargetBody::poleRaCoefs
   * @return std::vector<Angle>
   */
  std::vector<Angle> TargetBody::poleRaCoefs() {
    return m_raPole;
  }


  /**
   * @brief Returns coefficients of a quadratic polynomial fitting pole dec
   * @return @b std::vector<Angle>
   */
  std::vector<Angle> TargetBody::poleDecCoefs() {
    return m_decPole;
  }


  /**
   * @brief Returns coefficients of a quadratic polynomial fitting pole pm.
   * @return @b std::vector<Angle>
   */
  std::vector<Angle> TargetBody::pmCoefs() {
    return m_pm;
  }


  /**
   * @brief Returns coefficients of pole right ascension nut/prec terms.
   * @return @b std::vector<double>
   */
  std::vector<double> TargetBody::poleRaNutPrecCoefs() {
    return m_raNutPrec;
  }


  /**
   * @brief TargetBody::poleDecNutPrecCoefs
   * @return @b std::vector<double>
   */
  std::vector<double> TargetBody::poleDecNutPrecCoefs() {
    return m_decNutPrec;
  }


  /**
   * @brief Returns coefficients of the prime meridian nut/prec terms.
   * @return @b std::vector<double>
   */
  std::vector<double> TargetBody::pmNutPrecCoefs() {
    return m_pmNutPrec;
  }


  /**
   * @brief Returns constants of planetary system nut/prec periods.
   * @return @b std::vector<Angle>
   */
  std::vector<Angle> TargetBody::sysNutPrecConstants() {
    return m_sysNutPrec0;
  }

  /**
   * @brief Returns Linear terms of planetary system nut/prec periods.
   * @return @b std::vector<Angle>
   */
  std::vector<Angle> TargetBody::sysNutPrecCoefs() {
    return m_sysNutPrec1;
  }


  /**
   * @brief This returns the NAIF body code of the target.
   *
   * @return @b SpiceInt NAIF body code
   *
   */
  SpiceInt TargetBody::naifBodyCode() const {
    return *m_bodyCode;
  }


  /**
   * @brief This returns the NAIF body code of the target's planet system
   *
   * @return @b SpiceInt NAIF body code of target's planet system.
   *
   */
  SpiceInt TargetBody::naifPlanetSystemCode() const {
    return *m_systemCode;
  }


  /**
   * @brief This returns the body name of the target's planet system
   *
   * @return @b QString The body name of target's planet system.
   *
   */
  QString TargetBody::naifPlanetSystemName() const {
    return m_systemName;
  }


  /**
   * @brief Returns "a" radius
   *
   * @return @b Distance The value of body radius "a"
   */
  Distance TargetBody::radiusA() const {
    return m_radii[0];
  }


  /**
   * @brief Returns "a" radius sigma.
   *
   * @return @b Distance value of body radius "a" sigma
   */
  Distance TargetBody::sigmaRadiusA() const {
    return m_sigmaRadii[0];
  }


  /**
   * @brief Returns "b" radius.
   *
   * @return @b Distance value of body radius "b".
   */
  Distance TargetBody::radiusB() const {
    return m_radii[1];
  }


  /**
   * @brief Returns "b" radius sigma.
   *
   * @return @b Distance The value of body radius "b" sigma.
   */
  Distance TargetBody::sigmaRadiusB() const {
    return m_sigmaRadii[1];
  }


  /**
   * @brief Returns the "c" radius
   *
   * @return @b Distance value of body radius "c".
   */
  Distance TargetBody::radiusC() const {
    return m_radii[2];
  }


  /**
   * @brief Returns the "c" radius sigma
   *
   * @return @b Distance value of body radius "c" sigma
   */
  Distance TargetBody::sigmaRadiusC() const {
    return m_sigmaRadii[2];
  }


  /**
   * @brief Returns the mean radius.
   *
   * @return @b Distance value of body mean radius.
   */
  Distance TargetBody::meanRadius() const {
    Distance meanRadius = m_radii[0] + m_radii[1] + m_radii[2];

    meanRadius = meanRadius/3.0;

    return meanRadius;
  }


  /**
   * @brief Returns the mean radius sigma.
   *
   * @return @b Distance value of body mean radius
   */
  Distance TargetBody::sigmaMeanRadius() const {
    Distance sigmaMeanRadius = m_sigmaRadii[0] + m_sigmaRadii[1] + m_sigmaRadii[2];

    sigmaMeanRadius = sigmaMeanRadius/3.0;

    return sigmaMeanRadius;
  }


  /**
   * Output format:
   *
   *
   * <image id="..." fileName="...">
   *   ...
   * </image>
   *
   * (fileName attribute is just the base name)
   */
//  void TargetBody::save(QXmlStreamWriter &stream, const Project *project,
//                            FileName newProjectRoot) const {

//    stream.writeStartElement("TargetBody");
//    // save ID, cnet file name, and run time to stream
//    stream.writeStartElement("generalAttributes");
//    stream.writeTextElement("id", m_id->toString());
//    stream.writeTextElement("runTime", runTime());
//    stream.writeTextElement("fileName", m_controlNetworkFileName->expanded());
//    stream.writeEndElement(); // end general attributes

//    // save settings to stream
//    m_settings->save(stream, project);

//    // save statistics to stream
//    m_statisticsResults->save(stream, project);

//    // save image lists to stream
//    if ( !m_images->isEmpty() ) {
//      stream.writeStartElement("imageLists");

//      for (int i = 0; i < m_images->count(); i++) {
//        m_images->at(i)->save(stream, project, "");
//      }

//      stream.writeEndElement();
//    }
//    stream.writeEndElement(); //end TargetBody
//  }



//  void TargetBody::save(QXmlStreamWriter &stream, const Project *project) const {

//    stream.writeStartElement("TargetBody");

//    // save ID, attributes, and run time to stream
//    stream.writeStartElement("generalAttributes");
//    stream.writeTextElement("id", m_id->toString());
//    stream.writeTextElement("runTime", runTime());
//    stream.writeEndElement(); // end general attributes

//    stream.writeEndElement(); //end TargetBody
//  }



  /**
   * @brief Get a unique, identifying string associated with this TargetBody object.
   *
   * @return @b A unique ID for this TargetBody object.
   */
  QString TargetBody::id() const {
    return m_id->toString().remove(QRegExp("[{}]"));
  }


}
