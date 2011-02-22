#ifndef ControlCubeGraphNode_h
#define ControlCubeGraphNode_h

/**
 * @file
 * $Revision: 1.8 $
 * $Date: 2008/06/18 18:54:11 $
 *
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

template< typename A, typename B > class QHash;
template< typename T > class QList;

class QString;

namespace Isis {
  class ControlMeasure;
  class ControlPoint;
  class iString;

  /**
   * @brief Serial Number with added functionality for Control Networks
   *
   * This class is extends the Serial Number class in order to directly point
   * between its associated measures in a Control Network and back.
   *
   * @ingroup ControlNetwork
   *
   * @author 2011-01-14 Travis Addair and Christopher Austin
   *
   * @see ControlPoint ControlMeasure
   *
   * @internal
   *   @history 2011-01-14 Travis Addair and Christopher Austin - original
   *                version
   *   @history 2011-02-18 Eric Hyer - This class now also acts as a vertex
   *                class for a graph where edges are the connections between
   *                images.  This means that connections are stored to other
   *                ControlCubeGraphNode objects who have measures which have
   *                the same parent (point) as measures here.
   *   @history 2011-02-22 Eric Hyer - Addad isConnected() method
   *
   */
  class ControlCubeGraphNode {
    public:
      explicit ControlCubeGraphNode(iString sn);
      ControlCubeGraphNode(const ControlCubeGraphNode &other);
      virtual ~ControlCubeGraphNode();

      void addMeasure(ControlMeasure *measure);
      void removeMeasure(ControlMeasure *measure);
      void addConnection(ControlMeasure *measure);
      void removeConnection(ControlMeasure *measure);

      bool contains(ControlPoint *point);
      iString getSerialNumber();
      int size();
      QList< ControlMeasure * > getMeasures() const;
      bool isConnected(ControlCubeGraphNode *other) const;

      ControlMeasure *getMeasure(ControlPoint *point);
      const ControlMeasure *getMeasure(ControlPoint *point) const;
      ControlMeasure *operator[](ControlPoint *point);
      const ControlMeasure *operator[](ControlPoint *point) const;

      const ControlCubeGraphNode &operator=(ControlCubeGraphNode);


    private:
      void nullify();
      void updateConnections(void
          (ControlCubeGraphNode::*updateFunc)(ControlMeasure *),
          ControlMeasure *measure);


    private:
      iString *serialNumber;

      //! ControlMeasures hashed by ControlPoint ID
      QHash< ControlPoint *, ControlMeasure * > * measures;

      /**
       * Stores a list of ControlMeasures which establish a conection to the
       * ControlCubeGraphNode that the list is hashed by
       */
      QHash< ControlCubeGraphNode *, QList< ControlMeasure * > > * connections;

  };
}

#endif
