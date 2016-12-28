/* This program was originally written by Kris Becker for LRO mission and has been modified by Driss Takir (USGS) to 
     * incorporate the four photometric models (Lommel-Seeliger, 
     * Rolo, Minnaert, and McEwen) adopted by the OSIRIS-REx 
     * project. 
     * Build 3.0- 8/15/2016 
*/ 
/**
 * @file
 * $Revision$
 * $Date$
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
    </p>
    <p>
      This application provides features that allow multiband cubes to be 
      photometrically corrected with a properly formatted PVL input file much 
      like that of ISIS program <b>photomet</b>. This application restricts 
      much of the options available to the more sophiscated photomet 
      application. 
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
#include "PvlObject.h"
#include "PhotometricFunction.h"

using namespace std;

namespace Isis {
    /**
     * @brief Method to get photometric property given angles
     *
     * This routine computes the photometric property at the given
     * cube location after ensuring a proper parameter container is
     * found for the specified band.
     *
     * @author Driss Takir - 10/7/2016
     *
     * @param i     Incidence angle at cube location
     * @param e     Emission angle at cube location
     * @param g     Phase angle at cube location
     * @param band  Band number in cube (actually is band index) for
     *              lookup purposes
     *
     * @return double Returns photometric correction using
     *         parameters
     */
  double Lommelseeliger::photometry ( double i, double e, double g, int band ) const {
  // Test for valid band
  if ((band <= 0) || (band > (int) _bandpho.size())) {
    std::string mess = "Provided band " + IString(band) + " out of range.";
    throw IException(IException::Programmer, mess, _FILEINFO_);
  }
    return photometry(_bandpho[band - 1], i, e, g);
  }

  /**
   * @brief Performs actual photometric correction calculation 
   * This routine computes photometric correction using parameters
   * for the Lommel Seeliger equation. Driss Takir- 10/7/2016
   * Container of band-specific Lommel Seeliger parameters
   * Incidence angle in degrees Emission angle in degrees param g
   * Phase angle in degrees double Photometric correction
   * parameter
   */
  double Lommelseeliger::photometry ( const Parameters &parms, double i, double e, double g ) const {
    //  Ensure problematic values are adjusted
    if (i == 0.0) i = 10.E-12;
    if (e == 0.0) e = 10.E-12;

    // Convert to radians
    i *= rpd_c();
    e *= rpd_c();
    g *= parms.phaUnit; //  Apply unit normalizer

    // Compute Lommel-Seeliger components
    double mu = cos(e);
    double mu0 = cos(i);

    double Falpha = exp(parms.BETA * g + parms.GAMMA * g * g + parms.DELTA * g * g *g);

    // Simple Hillier photometric polynomial equation with exponential opposition
    //  surge term.
    double rcal = M_PI * parms.ALS * Falpha * (mu0 / (mu + mu0));
      return (rcal);
  }
 
  /**
  * @brief Return parameters used for all band 
  * Method creates 
  * keyword vectors of band specific parameters used 
  * in the photometric correction. 
  * Driss Takir - 10/7/2016 
  * Output PVL container write keywords 
  */                           
  void Lommelseeliger::Report ( PvlContainer &pvl ) {
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
    for (unsigned int i = 0; i < _bandpho.size(); i++) {
      Parameters &p = _bandpho[i];
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
   * @brief Determine Hillier parameters given a wavelength 
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
   *  Driss Takir - 10/07/2016
   *  
   *  @param wavelength Wavelength used to find paramete
   * set 
   *  
   * @return Hillier::Parameters Container of valid 
   * values.  If not found, a value of iProfile = -1 is 
   * returned. 
   */  

  Lommelseeliger::Parameters Lommelseeliger::findParameters ( const double wavelength ) const {
    for (unsigned int i = 0; i < _profiles.size(); i++) {
      const DbProfile &p = _profiles[i];
      if (p.exists("BandBinCenter")) {
        double p_center = toDouble(ConfKey(p, "BandBinCenter", toString(Null)));
        double tolerance = toDouble(ConfKey(p, "BandBinCenterTolerance", toString(1.0E-6)));
        if (fabs(wavelength - p_center) <= fabs(tolerance)) {
          Parameters pars = extract(p);
          pars.iProfile = i;
          pars.wavelength = wavelength;
          pars.tolerance = tolerance;
          return (pars);
        }
      }
    }
    // Not found if we reach here
    return (Parameters());
  }

  /**
  * @brief Extracts necessary Hillier parameters from profile 
  *  
  * Given a profile read from the input PVL file, this method 
  * extracts needed parameters (from Keywords) in the 
  * PVL profile and creates a container of the converted values. 
  * 
  * @author Driss Takir - 10/7/2016 
  * 
  * @param p Profile to extract/convert 
  *  
  * @return Lommel Seeliger::Parameters Container of extracted 
  * values 
  */  
  Lommelseeliger::Parameters Lommelseeliger::extract ( const DbProfile &p ) const {
    Parameters pars;
    pars.ALS = toDouble(ConfKey(p, "ALS", toString(0.0)));
    pars.BETA = toDouble(ConfKey(p, "BETA", toString(0.0)));
    pars.GAMMA = toDouble(ConfKey(p, "GAMMA", toString(0.0)));
    pars.DELTA = toDouble(ConfKey(p, "DELTA", toString(0.0)));
    pars.wavelength = toDouble(ConfKey(p, "BandBinCenter", toString(Null)));
    pars.tolerance = toDouble(ConfKey(p, "BandBinCenterTolerance", toString(Null)));
    //  Determine equation units - defaults to Radians
    pars.units = ConfKey(p, "LommelseeligerUnits", QString("Radians"));
    pars.phaUnit = (pars.units.toLower() == "degrees") ? 1.0 : rpd_c();
    return (pars);
  }

    /**
     * @brief Initialize class from input PVL and Cube files
     *
     * This method is typically called at class instantiation time,
     * but is reentrant.  It reads the parameter PVL file and
     * extracts Photometric model and Normalization models from it.
     * The cube is needed to match all potential profiles for each
     * band.
     *
     * @author Driss Takir - 10/07/2016
     *
     * @param pvl  Input PVL parameter files
     * @param cube Input cube file to correct
     */
  void Lommelseeliger::init ( PvlObject &pvl, Cube &cube ) {

    //  Make it reentrant
    _profiles.clear();
    _bandpho.clear();

    //  Interate over all Photometric groups
    _iRef = toDouble(ConfKey(_normProf, "IncRef", toString(30.0)));
    _eRef = toDouble(ConfKey(_normProf, "EmaRef", toString(0.0)));
    _gRef = toDouble(ConfKey(_normProf, "PhaRef", toString(_iRef)));


    PvlObject &phoObj = pvl.findObject("PhotometricModel");
    DbProfile phoProf = DbProfile(phoObj);
    PvlObject::PvlGroupIterator algo = phoObj.beginGroup();
    while (algo != phoObj.endGroup()) {
      if (algo->name().toLower() == "algorithm") {
        _profiles.push_back(DbProfile(phoProf, DbProfile(*algo)));
      }
      ++algo;
    }

    Pvl *label = cube.label();
    PvlKeyword center = label->findGroup("BandBin", Pvl::Traverse)["Center"];
    QString errs("");
    for (int i = 0; i < cube.bandCount(); i++) {
      Parameters parms = findParameters(toDouble(center[i]));
      if (parms.IsValid()) {
        parms.band = i + 1;
        //_camera->SetBand(i + 1);
        parms.phoStd = photometry(parms, _iRef, _eRef, _gRef);
        _bandpho.push_back(parms);
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
} // namespace Isis

