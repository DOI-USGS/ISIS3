/**
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

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

#include "Camera.h"
#include "DbProfile.h"
#include "Lommelseeliger.h"
#include "PhotometricFunction.h"
#include "PvlObject.h"

using namespace std;

namespace Isis {
  /**
   * Create Lommel Seeliger photometric object.
   *
   * @param pvl Photometric parameter files
   * @param cube Input cube file
   * @param useCamera Indicates whether to use the camera model for the given 
   *                  cube.
   *
   * @author 2016-10-07 Driss Takir
   */
  Lommelseeliger::Lommelseeliger(PvlObject &pvl, 
                                 Cube &cube, 
                                 bool useCamera) 
                                 : PhotometricFunction(pvl, cube, useCamera) {
    init(pvl, cube);
  }


  //! Destructor
  Lommelseeliger::~Lommelseeliger() {
  }


  /**
   * Initialize class from input PVL and Cube files.
   *
   * This method is typically called at class instantiation time,
   * but is reentrant.  It reads the parameter PVL file and
   * extracts Photometric model and Normalization models from it.
   * The cube is needed to match all potential profiles for each
   * band.
   *
   * @param pvl  Input PVL parameter files
   * @param cube Input cube file to correct
   *
   * @author 2016-10-07 Driss Takir
   */
  void Lommelseeliger::init(PvlObject &pvl, 
                            Cube &cube) {

    //  Make it reentrant
    m_profiles.clear();
    m_bandpho.clear();

    //  Interate over all Photometric groups
    setIncidenceReference(toDouble(ConfKey(normalProfile(), 
                                           "IncRef", 
                                           toString(30.0))));
    setEmissionReference(toDouble(ConfKey(normalProfile(), 
                                          "EmaRef", 
                                          toString(0.0))));
    setPhaseReference(toDouble(ConfKey(normalProfile(), 
                                       "PhaRef", 
                                       toString(incidenceReference()))));

    PvlObject &phoObj = pvl.findObject("PhotometricModel");
    DbProfile phoProf = DbProfile(phoObj);
    PvlObject::PvlGroupIterator algo = phoObj.beginGroup();
    while (algo != phoObj.endGroup()) {
      if (algo->name().toLower() == "algorithm") {
        m_profiles.push_back(DbProfile(phoProf, DbProfile(*algo)));
      }
      ++algo;
    }

    Pvl *label = cube.label();
    PvlKeyword center = label->findGroup("BandBin", Pvl::Traverse)["Center"];
    QString errs("");
    for (int i = 0; i < cube.bandCount(); i++) {
      Parameters parms = findParameters(toDouble(center[i]));
      if (parms.isValid()) {
        parms.band = i + 1;
        //_camera->SetBand(i + 1);
        parms.phoStd = photometry(parms, 
                                  incidenceReference(), 
                                  emissionReference(), 
                                  phaseReference());
        m_bandpho.push_back(parms);
      }
      else { // Appropriate photometric parameters not found
        ostringstream mess;
        mess << "Band " << i + 1 << " with wavelength Center = " << center[i]
             << " does not have PhotometricModel Algorithm group/profile";
        IException e(IException::User, mess.str(), _FILEINFO_);
        errs += e.toString() + "\n";
      }
    }

    // Check for errors and throw them all at the same time
    if (!errs.isEmpty()) {
      errs += " --> Errors in the input PVL file \"" + pvl.fileName() + "\"";
      throw IException(IException::User, errs, _FILEINFO_);
    }
    return;
  }


  /**
   * Method to get photometric property given angles.
   *
   * This routine computes the photometric property at the given
   * cube location after ensuring a proper parameter container is
   * found for the specified band.
   *
   * @param incidenceAngle Incidence angle at cube location
   * @param emissionAngle  Emission angle at cube location
   * @param phaseAngle     Phase angle at cube location
   * @param bandNumber     Band number in cube for lookup purposes.
   *
   * @return @b double Returns photometric correction using parameters.
   *
   * @author 2016-10-07 Driss Takir
   */
  double Lommelseeliger::photometry(double incidenceAngle, 
                                    double emissionAngle, 
                                    double phaseAngle,
                                    int bandNumber) const {
    // Test for valid band
    if ((bandNumber <= 0) || (bandNumber > (int) m_bandpho.size())) {
      QString mess = "Provided band " + toString(bandNumber) + " out of range.";
      throw IException(IException::Programmer, mess, _FILEINFO_);
    }
    return photometry(m_bandpho[bandNumber - 1], incidenceAngle, emissionAngle, phaseAngle);
  }


  /**
   * Performs actual photometric correction calculation 
   * This routine computes photometric correction using parameters
   * for the Lommel Seeliger equation. 
   * Container of band-specific Lommel Seeliger parameters
   * Incidence angle in degrees Emission angle in degrees param g
   * Phase angle in degrees double Photometric correction
   * parameter
   *
   * @param parms          Container of band-specific Hillier parameters.
   * @param incidenceAngle Incidence angle, in degrees.
   * @param emissionAngle  Emission angle, in degrees.
   * @param phaseAngle     Phase angle, in degrees.
   *
   * @return @b double Photometric correction parameter.
   *
   * @author 2016-10-07 Driss Takir
   */
  double Lommelseeliger::photometry(const Parameters &parms, 
                                    double incidenceAngle, 
                                    double emissionAngle, 
                                    double phaseAngle) const {

    //  Ensure problematic values are adjusted
    if (incidenceAngle == 0.0) {
      incidenceAngle = 10.E-12;
    }

    if (emissionAngle == 0.0) {
      emissionAngle = 10.E-12;
    }

    // Convert to radians
    incidenceAngle *= Isis::DEG2RAD;
    emissionAngle *= Isis::DEG2RAD;
    phaseAngle *= parms.phaUnit; //  Apply unit normalizer

    // Compute Lommel-Seeliger components
    double mu = cos(emissionAngle);
    double mu0 = cos(incidenceAngle);

    double Falpha = exp(parms.BETA * phaseAngle 
                        + parms.GAMMA * phaseAngle * phaseAngle 
                        + parms.DELTA * phaseAngle * phaseAngle * phaseAngle);

    // Simple Hillier photometric polynomial equation with exponential opposition
    //  surge term.
    double rcal = M_PI * parms.ALS * Falpha * (mu0 / (mu + mu0));
      return rcal;
  }

 
  /**
   * Return parameters used for all band.
   *
   * Method creates 
   * keyword vectors of band specific parameters used 
   * in the photometric correction. 
   * Output PVL container write keywords 
   *
   * @author 2016-10-07 Driss Takir
   */                           
  void Lommelseeliger::report(PvlContainer &pvl) {
    pvl.addComment("I/F = M_PI * ALS * F(alpha) * (mu0/(mu0+mu) * F(phase))");
    pvl.addComment(" where:");
    pvl.addComment("  mu0 = cos(incidence)");
    pvl.addComment("  mu = cos(incidence)");
    pvl.addComment("  F(phase) = exp(BETA * phase + GAMMA * phase^2 + DELTA * phase^3");

    pvl += PvlKeyword("Algorithm", "Lommelseeliger");
    PvlKeyword units("LommelseeligerUnits");
    PvlKeyword phostd("PhotometricStandard");
    PvlKeyword bbc("BandBinCenter");
    PvlKeyword bbct("BandBinCenterTolerance");
    PvlKeyword bbn("BandNumber");
    PvlKeyword ALS("ALS");
    PvlKeyword BETA("BETA");
    PvlKeyword GAMMA("GAMMA");
    PvlKeyword DELTA("DELTA");
    for (unsigned int i = 0; i < m_bandpho.size(); i++) {
      Parameters &p = m_bandpho[i];
      units.addValue(p.units);
      phostd.addValue(toString(p.phoStd));
      bbc.addValue(toString(p.wavelength));
      bbct.addValue(toString(p.tolerance));
      bbn.addValue(toString(p.band));
      ALS.addValue(toString(p.ALS));
      BETA.addValue(toString(p.BETA));
      GAMMA.addValue(toString(p.GAMMA));
      DELTA.addValue(toString(p.DELTA));
    }
    pvl += units;
    pvl += phostd;
    pvl += bbc;
    pvl += bbct;
    pvl += bbn;
    pvl += ALS;
    pvl += BETA;
    pvl += GAMMA;
    pvl += DELTA;
    return;
  }


  /**
   * Determine Hillier parameters given a wavelength. 
   *  
   * This method determines the set of Lommel Seeliger parameters 
   * to use for a given wavelength.  It iterates through 
   * all band profiles as read from the PVL file and 
   * computes the difference between the "wavelength" 
   * parameter and the BandBinCenter keyword. The 
   * absolute value of this value is checked against the
   * BandBinCenterTolerance paramter and if it is less 
   * than or equal to it, a Parameter container is 
   * returned. 
   *  
   * @param wavelength Wavelength used to find paramete set 
   *  
   * @return Hillier::Parameters Container of valid 
   * values.  If not found, a value of iProfile = -1 is 
   * returned. 
   *
   * @author 2016-10-07 Driss Takir
   */  
  Lommelseeliger::Parameters Lommelseeliger::findParameters(const double wavelength) const {
    for (unsigned int i = 0; i < m_profiles.size(); i++) {
      const DbProfile &p = m_profiles[i];
      if (p.exists("BandBinCenter")) {
        double center = toDouble(ConfKey(p, "BandBinCenter", toString(Null)));
        double tolerance = toDouble(ConfKey(p, "BandBinCenterTolerance", toString(1.0E-6)));
        if (fabs(wavelength - center) <= fabs(tolerance)) {
          Parameters pars = extract(p);
          pars.iProfile = i;
          pars.wavelength = wavelength;
          pars.tolerance = tolerance;
          return pars;
        }
      }
    }
    // Not found if we reach here
    return Parameters();
  }


  /**
   * Extracts necessary Hillier parameters from profile. 
   *  
   * Given a profile read from the input PVL file, this method 
   * extracts needed parameters (from Keywords) in the 
   * PVL profile and creates a container of the converted values. 
   * 
   * @param p Profile to extract/convert 
   *  
   * @return Lommel Seeliger::Parameters Container of extracted 
   * values 
   *
   * @author 2016-10-07 Driss Takir
   */  
  Lommelseeliger::Parameters Lommelseeliger::extract(const DbProfile &p) const {
    Parameters pars;
    pars.ALS = toDouble(ConfKey(p, "ALS", toString(0.0)));
    pars.BETA = toDouble(ConfKey(p, "BETA", toString(0.0)));
    pars.GAMMA = toDouble(ConfKey(p, "GAMMA", toString(0.0)));
    pars.DELTA = toDouble(ConfKey(p, "DELTA", toString(0.0)));
    pars.wavelength = toDouble(ConfKey(p, "BandBinCenter", toString(Null)));
    pars.tolerance = toDouble(ConfKey(p, "BandBinCenterTolerance", toString(Null)));
    //  Determine equation units - defaults to Radians
    pars.units = ConfKey(p, "LommelseeligerUnits", QString("Radians"));
    pars.phaUnit = (pars.units.toLower() == "degrees") ? 1.0 : Isis::DEG2RAD;
    return pars;
  }
} // namespace Isis

