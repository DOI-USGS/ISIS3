#ifndef Area3D_h
#define Area3D_h

/**
 * @file
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

namespace Isis {
  class Displacement;
  class Distance;

  /**
   * @brief Represents a 3D area (a 3D "cube")
   *
   * This class is designed to encapsulate the concept of a 3D area (i.e.
   *   Cube but not a Cube file).
   *
   * @ingroup Utility
   *
   * @author 2011-06-09 Jai Rideout and Steven Lambright
   *
   * @internal
   *   @history 2012-02-24 Steven Lambright - Changed intersection test
   *                           to be faster on intersection failures.
   */
  class Area3D {
    public:
      Area3D();
      Area3D(const Displacement &startX, const Displacement &startY,
             const Displacement &startZ,
             const Distance &width, const Distance &height,
             const Distance &depth);
      Area3D(const Displacement &startX, const Displacement &startY,
             const Displacement &startZ,
             const Displacement &endX, const Displacement &endY,
             const Displacement &endZ);
      Area3D(const Area3D &areaToCopy);
      virtual ~Area3D();

      Displacement getStartX() const;
      Displacement getStartY() const;
      Displacement getStartZ() const;

      Distance getWidth() const;
      Distance getHeight() const;
      Distance getDepth() const;

      Displacement getEndX() const;
      Displacement getEndY() const;
      Displacement getEndZ() const;

      void setStartX(const Displacement &startX);
      void setStartY(const Displacement &startY);
      void setStartZ(const Displacement &startZ);

      void moveStartX(const Displacement &startX);
      void moveStartY(const Displacement &startY);
      void moveStartZ(const Displacement &startZ);

      void setWidth(const Distance &width);
      void setHeight(const Distance &height);
      void setDepth(const Distance &depth);

      void setEndX(const Displacement &endX);
      void setEndY(const Displacement &endY);
      void setEndZ(const Displacement &endZ);

      void moveEndX(const Displacement &endX);
      void moveEndY(const Displacement &endY);
      void moveEndZ(const Displacement &endZ);

      void setXDimension(const Displacement &startX, const Distance &width);
      void setYDimension(const Displacement &startY, const Distance &height);
      void setZDimension(const Displacement &startZ, const Distance &depth);

      Area3D intersect(const Area3D &otherArea) const;

      bool isValid() const;

      bool operator ==(const Area3D &otherArea) const;
      bool operator !=(const Area3D &otherArea) const;

      Area3D &operator =(const Area3D &areaToCopy);

    protected:
      virtual void setArea(const Displacement &startX,
          const Displacement &startY, const Displacement &startZ,
          const Displacement &endX, const Displacement &endY,
          const Displacement &endZ);

    private:
      void deleteTheData();
      void nullTheData();

      //! The leftmost X position. Either NULL or a valid displacement.
      Displacement *m_startX;
      //! The topmost Y position. Either NULL or a valid displacement.
      Displacement *m_startY;
      //! The frontmost Z position. Either NULL or a valid displacement.
      Displacement *m_startZ;
      //! The rightmost X position. Either NULL or a valid displacement.
      Displacement *m_endX;
      //! The bottommost Y position. Either NULL or a valid displacement.
      Displacement *m_endY;
      //! The backmost Z position. Either NULL or a valid displacement.
      Displacement *m_endZ;
  };
}

#endif

