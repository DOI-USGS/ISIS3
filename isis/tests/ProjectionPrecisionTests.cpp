/** This work is free and unencumbered software released into the public domain.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

// Precision of the spherical projections close to the poles, the equator and the
// projection centres. Expected values are from a 50-digit computation on the same
// input, on a sphere of radius 6371000 m. The inverse is checked as a great-circle
// distance in metres, because close to the poles the longitude is ill-conditioned.

#include <cmath>
#include <memory>

#include <QString>

#include "LambertAzimuthalEqualArea.h"
#include "Mollweide.h"
#include "ObliqueCylindrical.h"
#include "Orthographic.h"
#include "PointPerspective.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "TProjection.h"
#include "TransverseMercator.h"

#include "gtest/gtest.h"

using namespace Isis;

namespace {

  struct PrecisionCase {
    const char *projection;
    double centerLatitude;
    double scaleFactor;
    double distance;        // PointPerspective, km above the surface
    double poleLatitude;    // ObliqueCylindrical
    double poleLongitude;   // ObliqueCylindrical
    double lon, lat;        // degrees
    double x, y;            // metres
  };

  const PrecisionCase cases[] = {
    // acos() near 1 close to the equator
    {"TransverseMercator", 0, 1, 0, 0, 0, 2.0, 1e-07, 222435.02960015144, 0.011126270487815284},
    // |lon| > 90 on the equator gave y = 0 instead of pi * R
    {"TransverseMercator", 0, 1, 0, 0, 0, 120.0, 0.0, 8390338.761308007, 20015086.79602057},
    // latitude from asin() near 1 close to the pole
    {"Orthographic", 45, 1, 0, 0, 0, 10.0, 89.9999999, 0.0019308795231307598, 4504977.295196278},
    // latitude from asin() near 1 close to the pole
    {"LambertAzimuthalEqualArea", 45, 1, 0, 0, 0, 60.0, 89.9999999, 0.01042317987605592, 4876152.290059438},
    // latitude from asin() near 1 close to the pole
    {"PointPerspective", 45, 1, 3000, 0, 0, 100.0, 89.9999999, 0.0067512397377098715, 2777408.316916929},
    // south polar aspect: the inverse mirrored the longitude
    {"PointPerspective", -90, 1, 3000, 0, 0, 30.0, -60.0, 1239959.8632428828, 2147673.4824828296},
    // within 1e-10 * R of the centre the inverse returned the centre
    {"PointPerspective", 0, 1, 3000, 0, 0, 0.0, 2e-09, 0.0, 0.00022238985328911748},
    // the Newton iteration used the wrong derivative and did not converge near the poles
    {"Mollweide", 0, 1, 0, 0, 0, -179.0, -89.99999, -591.078391808469, -9009954.60097763},
    // the Newton iteration used the wrong derivative and did not converge near the poles
    {"Mollweide", 0, 1, 0, 0, 0, 100.0, 89.99, 33021.10324921167, 9009905.592261195},
    // the Newton iteration used the wrong derivative and did not converge near the poles
    {"Mollweide", 0, 1, 0, 0, 0, 45.0, 89.3, 252308.41375123063, 8995812.573492255},
    // oblique latitude from asin() near 1 close to the oblique pole
    {"ObliqueCylindrical", 0, 1, 0, 40, 10, 10.0, 39.9999999, 0.0, 10007543.386890793}
  };

  const double radius = 6371000.0;

  double distance(double lon1, double lat1, double lon2, double lat2) {
    double a = sin((lat2 - lat1) / 2);
    double b = sin((lon2 - lon1) / 2);
    return 2 * radius * asin(sqrt(a * a + cos(lat1) * cos(lat2) * b * b));
  }

  std::unique_ptr<TProjection> makeProjection(const PrecisionCase &c) {
    Pvl label;
    label.addGroup(PvlGroup("Mapping"));
    PvlGroup &m = label.findGroup("Mapping");
    QString name = c.projection;
    m += PvlKeyword("ProjectionName", name);
    m += PvlKeyword("EquatorialRadius", QString::number(radius, 'g', 17));
    m += PvlKeyword("PolarRadius", QString::number(radius, 'g', 17));
    m += PvlKeyword("LatitudeType", "Planetocentric");
    m += PvlKeyword("LongitudeDirection", "PositiveEast");
    m += PvlKeyword("LongitudeDomain", "180");
    m += PvlKeyword("MinimumLatitude", "-90");
    m += PvlKeyword("MaximumLatitude", "90");
    // the non-polar LambertAzimuthalEqualArea rejects a range that contains the antipode
    m += PvlKeyword("MinimumLongitude", "-179.999");
    m += PvlKeyword("MaximumLongitude", "179.999");
    m += PvlKeyword("CenterLatitude", QString::number(c.centerLatitude, 'g', 17));
    m += PvlKeyword("CenterLongitude", "0");
    if (name == "TransverseMercator") {
      m += PvlKeyword("ScaleFactor", QString::number(c.scaleFactor, 'g', 17));
      return std::make_unique<TransverseMercator>(label);
    }
    if (name == "Orthographic") return std::make_unique<Orthographic>(label);
    if (name == "LambertAzimuthalEqualArea") return std::make_unique<LambertAzimuthalEqualArea>(label);
    if (name == "PointPerspective") {
      m += PvlKeyword("Distance", QString::number(c.distance, 'g', 17));
      return std::make_unique<PointPerspective>(label);
    }
    if (name == "Mollweide") return std::make_unique<Mollweide>(label);
    m += PvlKeyword("PoleLatitude", QString::number(c.poleLatitude, 'g', 17));
    m += PvlKeyword("PoleLongitude", QString::number(c.poleLongitude, 'g', 17));
    m += PvlKeyword("PoleRotation", "0");
    return std::make_unique<ObliqueCylindrical>(label);
  }
}

TEST(ProjectionPrecision, ForwardAndInverse) {
  const double d2r = M_PI / 180.0;
  for (const PrecisionCase &c : cases) {
    SCOPED_TRACE(QString("%1 center latitude %2, (%3, %4)").arg(c.projection).arg(c.centerLatitude)
                 .arg(c.lon, 0, 'g', 17).arg(c.lat, 0, 'g', 17).toStdString());
    std::unique_ptr<TProjection> p = makeProjection(c);

    ASSERT_TRUE(p->SetGround(c.lat, c.lon));
    EXPECT_LT(hypot(p->XCoord() - c.x, p->YCoord() - c.y), 1e-6) << "forward, metres";

    ASSERT_TRUE(p->SetCoordinate(c.x, c.y));
    EXPECT_LT(distance(p->Longitude() * d2r, p->Latitude() * d2r, c.lon * d2r, c.lat * d2r), 1e-6)
        << "inverse, metres";
  }
}
