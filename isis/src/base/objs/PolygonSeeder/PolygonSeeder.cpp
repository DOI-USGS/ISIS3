#include "Pvl.h"
#include "PvlGroup.h"
#include "Plugin.h"
#include "iException.h"
#include "PolynomialBivariate.h"
#include "LeastSquares.h"
#include "Filename.h"
#include "ProjectionFactory.h"
#include "PolygonTools.h"

#include "PolygonSeeder.h"


namespace Isis {
  /**
   * Create PolygonSeeder object.  Because this is a pure virtual class you can
   * not create an PolygonSeeder class directly.  Instead, see the
   * PolygonSeederFactory class.
   *
   * @param pvl  A pvl object containing a valid PolygonSeeder specification
   *
   * @todo
   */
  PolygonSeeder::PolygonSeeder(Pvl &pvl) {
    invalidInput = NULL;
    invalidInput = new Pvl(pvl);

    p_algorithmName = "Unknown";

    Parse(pvl);
  }


  //! Copy constructor
  PolygonSeeder::PolygonSeeder(const PolygonSeeder &other) {
    p_algorithmName = other.p_algorithmName;
    p_minimumThickness = other.p_minimumThickness;
    p_minimumArea = other.p_minimumArea;
  }


  //! Destroy PolygonSeeder object
  PolygonSeeder::~PolygonSeeder() {
    if(invalidInput) {
      delete invalidInput;
      invalidInput = NULL;
    }
  }


  /**
   * Initialize parameters in the PolygonSeeder class using a PVL specification.
   * An example of the PVL required for this is:
   *
   * @code
   * Object = AutoSeed
   *   Group = Algorithm
   *     Name      = Grid
   *     Tolerance = 0.7
   *   EndGroup
   * EndObject
   * @endcode
   *
   * There are many other options that can be set via the pvl and are
   * described in other documentation (see below).
   *
   * @param pvl The pvl object containing the specification
   **/
  void PolygonSeeder::Parse(Pvl &pvl) {

    std::string errorSpot;

    try {
      // Get info from Algorithm group
      errorSpot = "Algorithm";
      PvlGroup &algo = pvl.FindGroup("PolygonSeederAlgorithm", Pvl::Traverse);

      // algo is such a cool name for a PvlGroup that it begs to be out done
      PvlGroup &invalgo = invalidInput->FindGroup("PolygonSeederAlgorithm",
                          Pvl::Traverse);

      // Set the algorithm name
      errorSpot = "Name";
      p_algorithmName = (std::string) algo["Name"];

      if(invalgo.HasKeyword("Name"))
        invalgo.DeleteKeyword("Name");

      // Set the minimum thickness (Area / max(extent X, extent Y)**2
      errorSpot = "MinimumThickness";
      p_minimumThickness = 0.0;
      if(algo.HasKeyword("MinimumThickness")) {
        p_minimumThickness = (double) algo["MinimumThickness"];
      }

      if(invalgo.HasKeyword("MinimumThickness"))
        invalgo.DeleteKeyword("MinimumThickness");

      // Set the minimum area
      errorSpot = "MinimumArea";
      p_minimumArea = 0.0;
      if(algo.HasKeyword("MinimumArea")) {
        p_minimumArea = (double) algo["MinimumArea"];
      }

      if(invalgo.HasKeyword("MinimumArea"))
        invalgo.DeleteKeyword("MinimumArea");
    }
    catch(iException &e) {
      std::string msg = "Improper format for PolygonSeeder PVL [";
      msg +=  pvl.Filename() + "]. Location [" + errorSpot + "]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    return;
  }


  /**
   * Check the polygon to see if it meets standard criteria.
   *
   * @param xymp The multipoly containing the coordinates in x/y units instead
   *             of lon/lat
   * @param xyBoundBox The bounding box of the multipoly
   *
   * @return std::string A string with an appropriate message to throw if
   * a test was unsuccessful or an empty string if all tests passed.
   */
  std::string PolygonSeeder::StandardTests(const geos::geom::MultiPolygon *xymp,
      const geos::geom::Envelope *xyBoundBox) {
    if(xymp->getArea() < MinimumArea()) {
      std::string msg = "Polygon did not meet the minimum area of [";
      msg += Isis::iString(MinimumArea()) + "]";
      return msg;
    }

    double thickness =
      xymp->getArea() /
      pow(std::max(xyBoundBox->getWidth(), xyBoundBox->getHeight()), 2.0);
    if(thickness < MinimumThickness()) {
      std::string msg = "Polygon did not meet the minimum thickness ratio of [";
      msg += Isis::iString(MinimumThickness()) + "]";
      return msg;
    }

    return "";
  }


  /**
   * Return the minimum allowed thickness of the polygon. This value is set
   * from the "MinimumThickness" keyword in the PVL. The seeding algorithm
   * will not seed polygons that have a thickness ratio less than this
   */
  double PolygonSeeder::MinimumThickness() {
    return p_minimumThickness;
  }


  /**
   * Return the minimum allowed area of the polygon. This value is set
   * from the "MinimumArea" keyword in the PVL. The seeding algorithm will
   * not seed polygons that have an area less than this.
   */
  double PolygonSeeder::MinimumArea() {
    return p_minimumArea;
  }

  PvlGroup PolygonSeeder::PluginParameters(std::string grpName) {
    PvlGroup pluginInfo(grpName);

    PvlKeyword name("Name", p_algorithmName);
    PvlKeyword minThickness("MinimumThickness", p_minimumThickness);
    PvlKeyword minArea("MinimumArea", p_minimumArea);

    pluginInfo.AddKeyword(name);
    pluginInfo.AddKeyword(minThickness);
    pluginInfo.AddKeyword(minArea);

    return pluginInfo;
  }


  /**
   * The constructor was passed a pvl (from a def file probably)
   * Returns a copy of this pvl minus what was used
   */
  Pvl PolygonSeeder::InvalidInput() {
    return *invalidInput;
  }


  //! Assignment operator
  const PolygonSeeder &PolygonSeeder::operator=(const PolygonSeeder &other) {
    p_algorithmName = other.p_algorithmName;
    p_minimumThickness = other.p_minimumThickness;
    p_minimumArea = other.p_minimumArea;

    return *this;
  }

}
