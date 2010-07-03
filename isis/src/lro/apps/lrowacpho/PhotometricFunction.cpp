/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2010/05/18 06:38:05 $
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
#include <memory>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "Camera.h"
#include "PhotometricFunction.h"
#include "DbProfile.h"
#include "PvlObject.h"
#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"

using namespace std;

namespace Isis {

    /**
     * @brief Construct from PVL and Cube file
     *
     * @author Kris Becker - 2/21/2010
     *
     * @param pvl Photometric parameter files
     * @param cube Input cube file
     */
    PhotometricFunction::PhotometricFunction ( PvlObject &pvl, Cube &cube ) {
        _camera = cube.Camera();
    }
    /**
     * @brief Compute photometric DN at given line/sample/band
     *
     * This routine applies the photometric angles to the equation
     * and returns the calibration coefficient at the given  cube
     * location.
     *
     * The return parameter is the photometric standard/photometric
     * correction coefficient at the given pixel location.
     *
     * @author Kris Becker - 2/21/2010
     *
     * @param line   Line of cube image to compute photometry
     * @param sample Sample of cube image to compute photometry
     * @param band   Band of cube image to compute photometry
     *
     * @return double Photometric correction at cube loation
     */
    double PhotometricFunction::Compute ( const double &line, const double &sample, int band ) {
        // Update band if necessary
        if (_camera->Band() != band) {
            _camera->SetBand(band);
        }
        if (!_camera->SetImage(sample, line))
            return (Null);

        double i = _camera->IncidenceAngle();
        double e = _camera->EmissionAngle();
        double g = _camera->PhaseAngle();

        if (i < MinimumIncidenceAngle() || i > MaximumIncidenceAngle() || e < MinimumEmissionAngle() || e
                        > MaximumEmissionAngle() || g < MinimumPhaseAngle() || g > MaximumPhaseAngle())
            return (Null);


        return photometry(i, e, g, band);
    }

} // namespace Isis


