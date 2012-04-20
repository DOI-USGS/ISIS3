#include "IsisDebug.h"

#include "ControlNet.h"

#include <iostream>
#include <cmath>
#include <sstream>

#include <QtAlgorithms>
#include <QPair>
#include <QQueue>
#include <QSet>
#include <QTime>

#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "Application.h"
#include "CameraFactory.h"
#include "ControlMeasure.h"
#include "ControlNetFile.h"
#include "ControlNetVersioner.h"
#include "ControlPoint.h"
#include "ControlCubeGraphNode.h"
#include "Distance.h"
#include "IException.h"
#include "iTime.h"
#include "Progress.h"
#include "Projection.h"
#include "SerialNumberList.h"
#include "SpecialPixel.h"
#include "Statistics.h"

using namespace std;
using namespace boost::numeric::ublas;


namespace Isis {
  void ControlNet::nullify() {
    points = NULL;
    cubeGraphNodes = NULL;
    pointIds = NULL;
  }

  //!Creates an empty ControlNet object
  ControlNet::ControlNet() {
    nullify();

    points = new QHash< QString, ControlPoint * >;
    cubeGraphNodes = new QHash< QString, ControlCubeGraphNode * >;
    pointIds = new QStringList;

    p_invalid = false;
    p_created = Application::DateTime();
    p_modified = Application::DateTime();
  }


  ControlNet::ControlNet(const ControlNet &other) {
    nullify();

    points = new QHash< QString, ControlPoint * >;
    cubeGraphNodes = new QHash< QString, ControlCubeGraphNode * >;
    pointIds = new QStringList;

    QHashIterator< QString, ControlPoint * > pointsIterator(*other.points);
    while (pointsIterator.hasNext()) {
      pointsIterator.next();

      ControlPoint *newPoint = new ControlPoint(*pointsIterator.value());
      newPoint->parentNetwork = this;
      QString newPointId = newPoint->GetId();
      points->insert(newPointId, newPoint);
      pointIds->append(newPointId);

      // create graph for non-ignored points and measures
      if (!newPoint->IsIgnored()) {
        QList< ControlMeasure * > measures = newPoint->getMeasures();
        for (int i = 0; i < measures.size(); i++) {
          ControlMeasure *measure = measures[i];
          QString serial = measure->GetCubeSerialNumber();

          if (!measure->IsIgnored()) {
            if (cubeGraphNodes->contains(serial)) {
              (*cubeGraphNodes)[serial]->addMeasure(measure);
            }
            else {
              ControlCubeGraphNode *newControlCubeGraphNode =
                new ControlCubeGraphNode(serial);

              newControlCubeGraphNode->addMeasure(measure);
              cubeGraphNodes->insert(serial, newControlCubeGraphNode);
            }
          }
        }
      }
    }

    p_targetName = other.p_targetName;
    p_targetRadii = other.p_targetRadii;
    p_networkId = other.p_networkId;
    p_created = other.p_created;
    p_modified = other.p_modified;
    p_description = other.p_description;
    p_userName = other.p_userName;
    p_invalid = other.p_invalid;
    p_cameraMap = other.p_cameraMap;
    p_cameraList = other.p_cameraList;
  }


  /**
   * Creates a ControlNet object with the given list of control points and cubes
   *
   * @param ptfile Name of file containing a Pvl list of control points
   * @param progress A pointer to the progress of reading in the control points
   */
  ControlNet::ControlNet(const iString &ptfile, Progress *progress) {
    nullify();

    points = new QHash< QString, ControlPoint * >;
    cubeGraphNodes = new QHash< QString, ControlCubeGraphNode * >;
    pointIds = new QStringList;

    p_invalid = false;
    ReadControl(ptfile, progress);
  }


  ControlNet::~ControlNet() {
    if (points) {
      QHashIterator< QString, ControlPoint * > i(*points);
      while (i.hasNext()) {
        i.next();
        delete(*points)[i.key()];
        (*points)[i.key()] = NULL;
      }
      delete points;
      points = NULL;
    }

    if (cubeGraphNodes) {
      QHashIterator< QString, ControlCubeGraphNode * > i(*cubeGraphNodes);
      while (i.hasNext()) {
        i.next();
        delete(*cubeGraphNodes)[i.key()];
        (*cubeGraphNodes)[i.key()] = NULL;
      }
      delete cubeGraphNodes;
      cubeGraphNodes = NULL;
    }

    if (pointIds) {
      delete pointIds;
      pointIds = NULL;
    }
  }


  /**
   * Reads in the control points from the given file
   *
   * @param ptfile Name of file containing a Pvl list of control points
   * @param progress A pointer to the progress of reading in the control points
   *
   * @throws Isis::iException::User - "Invalid Network Type"
   * @throws Isis::iException::User - "Invalid Control Point"
   * @throws Isis::iException::User - "Invalid Format"
   *
   * @internal
   * @history 2009-04-07 Tracie Sucharski - Keep track of ignored measures.
   * @history 2010-08-06 Tracie Sucharski, Updated for changes made after
   *                           additional working sessions for Control network
   *                           design.
   * @history 2011-04-02 Debbie A. Cook - Added code to set the target radii
   *                           in the surface points of the control points as
   *                           they are read into memory instead of setting
   *                           parent prematurely to be able to set the radii
   *                           in ControlPoint.
   *
   */
  void ControlNet::ReadControl(const iString &filename, Progress *progress) {
    LatestControlNetFile *fileData = ControlNetVersioner::Read(filename);

    ControlNetFileHeaderV0002 &header = fileData->GetNetworkHeader();
    p_networkId     = header.networkid();
    if (header.has_targetname()) {
      SetTarget(header.targetname());
    }
    else {
      SetTarget("");
    }

    p_userName      = header.username();
    p_created       = header.created();
    p_modified      = header.lastmodified();
    p_description   = header.description();

    QList< ControlPointFileEntryV0002 > &fileDataPoints =
      fileData->GetNetworkPoints();

    if (fileDataPoints.size() > 0) {
      if (progress != NULL) {
        progress->SetText("Loading Control Points...");
        progress->SetMaximumSteps(fileDataPoints.size());
        progress->CheckStatus();
      }

      ControlPointFileEntryV0002 fileDataPoint;
      foreach(fileDataPoint, fileDataPoints) {
        AddPoint(new ControlPoint(fileDataPoint,
              p_targetRadii[0], p_targetRadii[1], p_targetRadii[2]));

        if (progress != NULL)
          progress->CheckStatus();
      }
    }

    delete fileData;
    fileData = NULL;
  }


  /**
   * Writes out the control network
   *
   * @param ptfile Name of file containing a Pvl list of control points
   * @param pvl    Boolean indicating whether to write in pvl format
   *               (Default=false)
   *
   * @throws Isis::iException::Programmer - "Invalid Net
   *             Enumeration"
   * @throws Isis::iException::Io - "Unable to write PVL
   *             infomation to file"
   *
   * @history 2010-10-05 Tracie Sucharski - Renamed old WRite method to WritePvl
   *                     and created this new method to determine format to
   *                     be written.
   */
  void ControlNet::Write(const iString &ptfile, bool pvl) {
    LatestControlNetFile *fileData = new LatestControlNetFile();

    ControlNetFileHeaderV0002 &header = fileData->GetNetworkHeader();

    header.set_networkid(p_networkId);
    header.set_targetname(p_targetName);
    header.set_username(p_userName);
    header.set_created(p_created);
    header.set_lastmodified(p_modified);
    header.set_description(p_description);

    QList< ControlPointFileEntryV0002 > &fileDataPoints =
      fileData->GetNetworkPoints();

    for (int i = 0; i < pointIds->size(); i++) {
      ControlPoint *point = points->value(pointIds->at(i));

      ControlPointFileEntryV0002 pointFileEntry = point->ToFileEntry();
      fileDataPoints.append(pointFileEntry);
    }

    ControlNetVersioner::Write(ptfile, *fileData, pvl);

    delete fileData;
    fileData = NULL;
  }


  /**
   * Adds a ControlPoint to the ControlNet
   *
   * @param point Control point to be added
   *
   * @throws Isis::IException::Programmer - "ControlPoint must
   *             have unique Id"
   */
  void ControlNet::AddPoint(ControlPoint *point) {
    if (!point) {
      iString msg = "Null pointer passed to ControlNet::AddPoint!";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (ContainsPoint(point->GetId())) {
      iString msg = "ControlPoint must have unique Id";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    QString pointId = point->GetId();
    points->insert(pointId, point);
    pointIds->append(pointId);

    point->parentNetwork = this;

    // notify control network of new (non-ignored) measures
    foreach(ControlMeasure * measure, point->getMeasures()) {
      measureAdded(measure);
    }
    emit networkStructureModified();
  }


  /**
   * Updates the ControlCubeGraphNode for the measure's serial number to
   * reflect the addition.  If there is currently no ControlCubeGraphNode for
   * this measure's serial, then a new ControlCubeGraphNode is created with
   * this measure as its first.
   *
   * @param measure The measure added to the network.
   */
  void ControlNet::measureAdded(ControlMeasure *measure) {
    if (!measure) {
      iString msg = "NULL measure passed to "
          "ControlNet::AddControlCubeGraphNode!";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    ControlPoint *point = measure->Parent();
    if (!point) {
      iString msg = "Control measure with NULL parent passed to "
          "ControlNet::AddControlCubeGraphNode!";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (!ContainsPoint(point->GetId())) {
      iString msg = "ControlNet does not contain the point [";
      msg += point->GetId() + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // make sure there is a node for every measure in this measure's parent
    for (int i = 0; i < point->GetNumMeasures(); i++) {
      QString sn = point->GetMeasure(i)->GetCubeSerialNumber();
      if (!cubeGraphNodes->contains(sn)) {
        cubeGraphNodes->insert(sn, new ControlCubeGraphNode(sn));
      }
    }

    // add the measure to the corresponding node
    QString serial = measure->GetCubeSerialNumber();
    ControlCubeGraphNode *node = (*cubeGraphNodes)[serial];
    node->addMeasure(measure);

    // in this measure's node add connections to the other nodes reachable from
    // its point
    if (!point->IsIgnored() && !measure->IsIgnored()) {
      for (int i = 0; i < point->GetNumMeasures(); i++) {
        ControlMeasure *cm = point->GetMeasure(i);
        if (!cm->IsIgnored()) {
          QString sn = cm->GetCubeSerialNumber();
          ControlCubeGraphNode *neighborNode = (*cubeGraphNodes)[sn];

          if (neighborNode != node) {
            node->addConnection(neighborNode, point);
            neighborNode->addConnection(node, point);
          }
        }
      }
    }
  }


  /**
   * Updates the connections for the ControlCubeGraphNode associated with the
   * measure's serial number to reflect the unignoration.
   *
   * @param measure The measure unignored from the network.
   */
  void ControlNet::measureUnIgnored(ControlMeasure *measure) {
    if (!measure) {
      iString msg = "NULL measure passed to "
          "ControlNet::AddControlCubeGraphNode!";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    ControlPoint *point = measure->Parent();
    if (!point) {
      iString msg = "Control measure with NULL parent passed to "
          "ControlNet::AddControlCubeGraphNode!";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (!ContainsPoint(point->GetId())) {
      iString msg = "ControlNet does not contain the point [";
      msg += point->GetId() + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // make sure there is a node for every measure in this measure's parent
    for (int i = 0; i < point->GetNumMeasures(); i++) {
      QString sn = point->GetMeasure(i)->GetCubeSerialNumber();
      if (!cubeGraphNodes->contains(sn)) {
        iString msg = "Node does not exist for [";
        msg += measure->GetCubeSerialNumber() + "]";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }

    if (!point->IsIgnored()) {
      QString serial = measure->GetCubeSerialNumber();
      ControlCubeGraphNode *node = (*cubeGraphNodes)[serial];

      // in this measure's node add connections to the other nodes reachable
      // from its point
      for (int i = 0; i < point->GetNumMeasures(); i++) {
        ControlMeasure *cm = point->GetMeasure(i);
        if (!cm->IsIgnored()) {
          QString sn = cm->GetCubeSerialNumber();
          ControlCubeGraphNode *neighborNode = (*cubeGraphNodes)[sn];
          if (neighborNode != node) {
            node->addConnection(neighborNode, point);
            neighborNode->addConnection(node, point);
          }
        }
      }
    }
  }


  /**
   * Updates the key reference (poind Id) from the old one to what the point
   * id was changet to. This method should only be called from ControlPoint's
   * SetId().
   *
   * @param point The point that needs to be updated.
   * @param oldId The pointId that the point had.
   */
  void ControlNet::UpdatePointReference(ControlPoint *point, iString oldId) {
    points->remove(oldId);
    (*points)[point->GetId()] = point;
    (*pointIds)[pointIds->indexOf((QString) oldId)] = (QString)point->GetId();
  }


  /**
   * Updates the ControlCubeGraphNode for this measure's serial number to
   * reflect the deletion.  If this is the only measure left in the containing
   * ControlCubeGraphNode, then the ControlCubeGraphNode is deleted as well.
   *
   * @param measure The measure removed from the network.
   */
  void ControlNet::measureDeleted(ControlMeasure *measure) {
    ASSERT(measure);

    QString serial = measure->GetCubeSerialNumber();
    ASSERT(cubeGraphNodes->contains(serial));
    ControlCubeGraphNode *node = (*cubeGraphNodes)[serial];

    // remove connections to and from this node
    if (!measure->IsIgnored() && !measure->Parent()->IsIgnored()) {
      // Break connections
      measureIgnored(measure);
    }

    // Remove the measure from the node.  If this caused the node to be empty,
    // then delete the node.
    node->removeMeasure(measure);
    if (!node->getMeasureCount()) {
      delete node;
      node = NULL;
      cubeGraphNodes->remove(serial);
    }
  }

  void ControlNet::measureIgnored(ControlMeasure *measure) {
    if (!measure) {
      iString msg = "NULL measure passed to "
          "ControlNet::AddControlCubeGraphNode!";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    ControlPoint *point = measure->Parent();
    if (!point) {
      iString msg = "Control measure with NULL parent passed to "
          "ControlNet::AddControlCubeGraphNode!";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    iString serial = measure->GetCubeSerialNumber();
    if (!cubeGraphNodes->contains(serial)) {
      iString msg = "Node does not exist for [";
      msg += serial + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    ControlCubeGraphNode *node = (*cubeGraphNodes)[serial];

    // remove connections to and from this node
    for (int i = 0; i < point->GetNumMeasures(); i++) {
      QString sn = point->GetMeasure(i)->GetCubeSerialNumber();
      if (cubeGraphNodes->contains(sn)) {
        ControlCubeGraphNode *neighborNode = (*cubeGraphNodes)[sn];
        if (node != neighborNode) {
          neighborNode->removeConnection(node, point);
          node->removeConnection(neighborNode, point);
        }
      }
    }
  }


  void ControlNet::emitNetworkStructureModified() {
    emit networkStructureModified();
  }


  /**
   * Random breadth-first search.  This meathod starts at a random serial, and
   * returns a list with the start serial as well as any serial which is
   * directly or indirectly connected to it.  The list returned will contain
   * all the serials in the network if and only if the network is completely
   * connected.  Otherwise, the list returned will be the entire island on
   * which a random image resides.
   *
   * @returns A List of connected graph nodes
   */
  QList< ControlCubeGraphNode * > ControlNet::RandomBFS(
    QList< ControlCubeGraphNode * > nodes) const {
    qsrand(42);
    Shuffle(nodes);

    // for keeping track of visited nodes
    QMap< ControlCubeGraphNode *, bool > searchList;
    for (int i = 0; i < nodes.size(); i++)
      searchList.insert(nodes[i], false);

    // for storing nodes as they are found
    QSet< ControlCubeGraphNode * > results;

    QQueue< ControlCubeGraphNode * > q;
    q.enqueue(nodes[0]);
    while (q.size()) {
      ControlCubeGraphNode *curNode = q.dequeue();
      if (!results.contains(curNode)) {
        // add to results
        results.insert(curNode);
        searchList[curNode] = true;

        // add all the neighbors to the queue
        QList< ControlCubeGraphNode * > neighbors = curNode->getAdjacentNodes();
        Shuffle(neighbors);
        for (int i = 0; i < neighbors.size(); i++)
          q.enqueue(neighbors[i]);
      }
    } // end of breadth-first search

    return results.values();
  }


  /**
   * Shuffles the QStrings in a QList< QString >
   *
   * @param list The list to be shuffled
   */
  void ControlNet::Shuffle(QList< ControlCubeGraphNode * > & list) const {
    for (int i = list.size() - 1; i > 0; i--) {
      // standard form is qrand() / (RAND_MAX + 1.0) * (max + 1 - min) + min
      // min is always zero here so it is simplified to...
      int j = (int)(qrand() / (RAND_MAX + 1.0) * (i + 1));
      qSwap(list[j], list[i]);
    }
  }


  /**
   * Calculate the band width and critical edges needed by the adjacency matrix
   * that could store cube connectivity if that matrix used the same ordering
   * as is in the provided list.  Note that no adjacency matrices are ever used
   * in this class!
   *
   * Critical edges are edges that contribute to band width.
   *
   * @param serials A list of cube serial numbers.
   *
   * @returns A QPair such that the first element is the needed bandwidth for
   *          the serials how they are currently ordered in the list, and the
   *          second element is the number of critical edges.
   */
  QPair< int, int > ControlNet::CalcBWAndCE(QList< QString > serials) const {
    for (int i = 0; i < serials.size(); i++)
      ASSERT(cubeGraphNodes->contains(serials[i]));

    int bw = 0;
    QList< int > colWidths;

    for (int i = 0; i < serials.size(); i++) {
      int colWidth = 0;
      ControlCubeGraphNode *node1 = (*cubeGraphNodes)[serials[i]];
      for (int j = 0; j < serials.size(); j++) {
        if (i != j) {
          ControlCubeGraphNode *node2 = (*cubeGraphNodes)[serials[j]];
          int colDiff = abs(i - j);
          if (node1->isConnected(node2) && colDiff > colWidth)
            colWidth = colDiff;
        }
      }
      colWidths.append(colWidth);
      if (colWidth > bw)
        bw = colWidth;
    }

    int criticalEdges = 0;
    foreach(int width, colWidths) {
      if (width == bw)
        criticalEdges++;
    }

    return qMakePair(bw, criticalEdges);
  }


  /**
   * Delete a ControlPoint from the network by the point's address.
   *
   * @param point The point to delete
   */
  int ControlNet::DeletePoint(ControlPoint *point) {
    if (points->values().contains(point)) {
      return DeletePoint(point->GetId());
    }
    else {
      iString msg = "point [";
      msg += (long) point;
      msg += "] does not exist in the network";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }


  /**
   * Delete a ControlPoint from the network using the point's Id.
   *
   * @param pointId The Point Id of the ControlPoint to be deleted.
   */
  int ControlNet::DeletePoint(iString pointId) {
    if (!points->contains(pointId)) {
      iString msg = "point Id [" + pointId + "] does not exist in the network";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    ControlPoint *point = (*points)[pointId];

    if (point->IsEditLocked())
      return ControlPoint::PointLocked;

    bool wasIgnored = point->IsIgnored();

    // notify CubeSerialNumbers of the loss of this point
    foreach(ControlMeasure * measure, point->getMeasures()) {
      measureDeleted(measure);
    }

    // See if removing this point qualifies for a re-check of validity
    bool check = false;
    if (p_invalid && point->IsInvalid())
      check = true;

    // delete point
    points->remove(pointId);
    pointIds->removeAt(pointIds->indexOf(pointId));
    delete point;
    point = NULL;

    // Check validity if needed (There were two or more points with the same
    // Id - see if this is still the case)
    if (check) {
      p_invalid = false;

      // check for 2 or more points with same Id
      QList< QString > keys = points->keys();
      for (int i = 0; i < keys.size() && !p_invalid; i++) {
        if (points->count(keys[i]) > 1)
          p_invalid = true;
      }
    }

    if (!wasIgnored)
      emit networkStructureModified();

    return ControlPoint::Success;
  }


  /**
   * Delete a ControlPoint from the network using the point's index.
   *
   * @param index The index of the Control Point to be deleted.
   */
  int ControlNet::DeletePoint(int index) {
    if (index < 0 || index >= pointIds->size()) {
      iString msg = "Index [" + iString(index) + "] out of range";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return DeletePoint(pointIds->at(index));
  }


  /**
   * @param pointId the point Id to check for in the network.
   *
   * @returns True if the point is in the network, false otherwise.
   */
  bool ControlNet::ContainsPoint(iString pointId) const {
    return points->contains(pointId);
  }


  /**
   * This method searches through all the cube serial numbers in the network.
   * Serials which are connected to other serials through points are grouped
   * together in the same lists.  The list containing the lists of strings is
   * nothing more than a list of islands such that each island is a list of
   * serials which are connected to each other.  If the control network is
   * completely connected, then this list will only have one element (a list
   * of all the serials in the network).
   *
   * @returns A list of cube islands as serial numbers
   */
  QList< QList< QString > > ControlNet::GetSerialConnections() const {
    QList< QList< QString > > islandStrings;
    QList< QList< ControlCubeGraphNode * > > islands = GetNodeConnections();
    for (int i = 0; i < islands.size(); i++) {
      QList< QString > newIsland;
      islandStrings.append(newIsland);
      for (int j = 0; j < islands[i].size(); j++)
        islandStrings[i].append(islands[i][j]->getSerialNumber());
    }
    return islandStrings;
  }


  /**
   * This method searches through all the cube serial numbers in the network.
   * Serials which are connected to other serials through points are grouped
   * together in the same lists.  The list containing the lists of strings is
   * nothing more than a list of islands such that each island is a list of
   * serials which are connected to each other.  If the control network is
   * completely connected, then this list will only have one element (a list
   * of all the serials in the network).
   *
   * @returns A list of cube islands as graph nodes
   */
  QList< QList< ControlCubeGraphNode * > > ControlNet::GetNodeConnections() const {
    QList< ControlCubeGraphNode * > notYetFound = cubeGraphNodes->values();
    QList< QList< ControlCubeGraphNode * > > islands;

    do {
      // extract an island from the nodes which are not yet found
      QList< ControlCubeGraphNode * > island = RandomBFS(notYetFound);

      // remove newly found nodes from notYetFound
      for (int i = 0; i < island.size(); i++)
        notYetFound.removeOne(island[i]);

      // Add island to list of islands
      islands.append(island);
    }
    while (notYetFound.size());

    return islands;
  }


  /**
   * This method uses Kruskal's Algorithm to construct a minimum spanning tree
   * of the given island, with control measures acting as the edges between
   * graph nodes.   Because measures do not directly connect graph nodes, but
   * rather connect graph nodes to control points, points are considered
   * "intermediate vertices".  When building the tree, we treat points like
   * normal graph node vertices, but after the tree is built, we prune away any
   * measures that, in conjunction with a point, form an "incomplete edge".
   * Such an edge goes from a graph node to a point, but does not have another
   * edge going from that point to another node.  Since the primary purpose of
   * this tree is to evaluate image connectivity, such edges are unnecessary.
   * A "complete edge" consists of two measures and a point, and connects two
   * graph nodes, or images.
   *
   * The cost of each edge is determined by the provided less-than comparison
   * function.  If a Control Measure is less-than another, it is said to have a
   * lower cost, and is thus more likely to appear in the minimum spanning tree.
   *
   * Minimum spanning trees are constructed on islands, not networks, so it is
   * important that the caller first retrieve the list of islands in the network
   * from the GetNodeConnections() method before invoking this method.  Creating
   * a minimum spanning tree for an entire network with multiple islands would
   * be impractical, as this method does not account for the possibility that
   * some nodes are disconnected in the input.
   *
   * A common usage of the minimum spanning tree is to measure the importance of
   * a given measure or point to the overall connectivity of a network.  If a
   * measurement is not in the MST, we do not need to worry about creating
   * additional islands by removing it.
   *
   * It is important that the user choose their less-than function carefully.
   * If a poor less-than function is used, then "bad" measures could end up in
   * the MST while "good" measures are excluded, thus giving the user the false
   * impression that a good measure can be safely deleted, while a bad measure
   * must be preserved.  The application "cnetwinnow", for example, tries to
   * remove as many measures with high residuals as possible without increasing
   * the island count of the network, so its less-than function compares the
   * residual magnitude of the two input measures.
   *
   * @param island The list of graph nodes forming the island to be minimized
   * @param lessThan A comparison function telling us if one measure is better
   *                 than another
   * 
   * @return The set of all measures (edges) in the minimum spanning tree
   */
  QSet< ControlMeasure * > ControlNet::MinimumSpanningTree(
      QList< ControlCubeGraphNode *> &island,
      bool lessThan(const ControlMeasure *, const ControlMeasure *)) const {

    // Kruskal's Algorithm
    QSet< ControlMeasure * > minimumTree;

    // We start with a map containing all the unconnected vertices (nodes and
    // points), each its own single-element tree.  Our goal is join them all
    // together into one tree connecting every vertex.  We map into the forest
    // with point ID and serial number so the two types of vertices can share a
    // common, nearly constant-time lookup interface.
    QMap< QString, ControlVertex * > forest;

    // Get a list of all the candidate edges on the island, and a set of their
    // associated Control Points (to avoid duplication, as measures share common
    // points).  Keep a count of how many measures in the MST are connected to
    // each point, as we'll want to prune off points with only one such
    // conenction.
    QList< ControlMeasure * > edges;
    QMap< ControlPoint *, int > uniquePoints;
    for (int i = 0; i < island.size(); i++) {
      // Add each graph node as a tree in the forest
      ControlCubeGraphNode *node = island[i];
      forest.insert(node->getSerialNumber(), new ControlVertex(node));

      // Every graph node has a list of measures: these are our edges
      QList< ControlMeasure * > measures = node->getMeasures();
      for (int j = 0; j < measures.size(); j++) {
        edges.append(measures[j]);

        // Every measure has a point: these act as intermediate vertices.  We
        // keep a count of how many edges in the MST come off this point.
        // Points with less than 2 edges are considered incomplete, as they do
        // not form a connection from one graph node to another, or a "complete
        // edge"
        uniquePoints.insert(measures[j]->Parent(), 0);
      }
    }

    // Sort the edges in increasing cost with the provided less-than function
    qSort(edges.begin(), edges.end(), lessThan);

    // Add every unique point on the island as a tree in the forest
    QList< ControlPoint * > pointList = uniquePoints.keys();
    for (int i = 0; i < pointList.size(); i++) {
      ControlPoint *point = pointList[i];
      forest.insert(point->GetId(), new ControlVertex(point));
    }

    // Every time we join two trees together, we reduce the total number of
    // trees by one, but our forest data structure is unchanged, so keep a
    // record of the actual forest size to decrement manually as we go along
    int trees = forest.size();

    // Keep trying to join trees until there is only one tree remaining or we're
    // out of edges to try
    while (trees > 1 && edges.size() > 0) {
      // Try to add our lowest-cost edge to the minimum spanning tree
      ControlMeasure *leastEdge = edges.takeFirst();

      // Each edge connects two vertices: a point and a graph node.  So grab the
      // trees for each node and check that they're disjoint, and thus able to
      // be joined.
      iString pointId = leastEdge->Parent()->GetId();
      ControlVertex *pointVertex = forest[pointId];

      iString serialNum = leastEdge->ControlSN()->getSerialNumber();
      ControlVertex *nodeVertex = forest[serialNum];

      // When the edge joins two different trees (i.e., they have different
      // roots), then add the edge to the minimum spanning tree and join the
      // trees into one.  Otherwise, we have formed a cycle and should thus
      // discard the edge.
      if (pointVertex->getRoot() != nodeVertex->getRoot()) {
        ControlVertex::join(pointVertex, nodeVertex);
        trees--;
        minimumTree.insert(leastEdge);
        uniquePoints[pointVertex->getPoint()]++;
      }
    }

    // Prune edges that go from a graph node to a point, but not from that
    // point to another graph node.  We care about image (graph node)
    // connectivity, not point connectivity.  A complete edge consists of two
    // measures and a point, so remove any incomplete edges.
    QList< ControlMeasure * > unprunedEdges = minimumTree.values();
    for (int i = 0; i < unprunedEdges.size(); i++) {
      if (uniquePoints[unprunedEdges[i]->Parent()] < 2)
        // The point this edge is connected to does not go on to another node,
        // so prune it
        minimumTree.remove(unprunedEdges[i]);
    }

    // Clean up our vertices.  This will not delete any of the points, measures,
    // or graph nodes.  All of that is owned by the network.
    QList< ControlVertex * > vertexList = forest.values();
    for (int i = 0; i < vertexList.size(); i++) delete vertexList[i];

    // Sanity check: an island with n > 1 nodes must, by definition, have an MST
    // of e edges such that n <= e <= 2n
    int n = island.size();
    int e = minimumTree.size();
    if (n > 1) {
      if (e < n || e > 2 * n) {
        iString msg = "An island of n = [" + iString(n) +
          "] > 1 nodes must have a minimum spanning tree of e edges such that "
          " n <= e <= 2n, but e = [" + iString(e) + "]";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }

    return minimumTree;
  }


  /**
   * @returns The total number of edges in the bi-directional graph for images
   */
  int ControlNet::getEdgeCount() const {
    int total = 0;
    foreach (ControlCubeGraphNode * node, *cubeGraphNodes) {
      total += node->getAdjacentNodes().size();
    }

    return total;
  }



  /**
   * Used for verifying graph intergrity
   *
   * @returns A string representation of the cube graph
   */
  iString ControlNet::CubeGraphToString() const {
    QStringList serials;
    QHashIterator < QString, ControlCubeGraphNode * > i(*cubeGraphNodes);
    while (i.hasNext()) {
      i.next();
      serials << i.value()->getSerialNumber();
    }
    qSort(serials);

    QString str;
    for (int i = 0; i < serials.size(); i++) {
      str += "  " + serials[i] + "\n"
          + (*cubeGraphNodes)[serials[i]]->connectionsToString() + "\n";
    }

    return str;
  }


  /**
   * Use this method to get a complete list of all the cube serial numbers in
   * the network.  Note that the order in which the serials are ordered in the
   * returned list is arbitrary and could change each time this method is
   * called (but the operation is done in constant time).
   *
   * @returns A list of the Cube Serial Numbers in the ControlNet.
   */
  QList< QString > ControlNet::GetCubeSerials() const {
    return cubeGraphNodes->keys();
  }


  /**
   * @returns A list of all the cube graph nodes in the network
   */
  QList< ControlCubeGraphNode * > ControlNet::GetCubeGraphNodes() {
    return cubeGraphNodes->values();
  }


  /**
   * Does a check to ensure that the given serial number is contained within
   * the network.  If it is not, then an exception is thrown
   *
   * @param serialNumber the cube serial number to validate
   */
  void ControlNet::ValidateSerialNumber(iString serialNumber) const {
    if (!cubeGraphNodes->contains(serialNumber)) {
      iString msg = "Cube Serial Number [" + serialNumber + "] not found in "
          "the network";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Get all the measures pertaining to a given cube serial number
   *
   * @returns A list of all measures which are in a given cube
   */
  QList< ControlMeasure * > ControlNet::GetMeasuresInCube(iString serialNumber) {
    ValidateSerialNumber(serialNumber);
    return (*cubeGraphNodes)[serialNumber]->getMeasures();
  }


  /**
   * Copies the content of the a ControlMeasureLessThanFunctor
   *
   * @param other, reference to the other ControlMeasureLessThanFunctor
   */
  ControlNet::ControlMeasureLessThanFunctor & 
    ControlNet::ControlMeasureLessThanFunctor::operator=(ControlMeasureLessThanFunctor const &other) {
    
    if (this != &other) {
      this->m_accessor = other.m_accessor;
    }

    return *this;
  }



  /**
   * The () operator for the Control Measure less than functor
   * 
   * @param a, ControlMeasure* pointer to the first control measure
   * @param b, ControlMeasure* pointer to the sencond control measure
   */
  bool ControlNet::ControlMeasureLessThanFunctor::operator()
    (ControlMeasure* const &a, ControlMeasure* const &b) {
    
    return (a->*this->m_accessor)() < (b->*this->m_accessor)();

  }

  /**
   * Get a sorted list of all the measures that have values in a given ragen
   *
   * @param statFunc A pointer to a control Measure acessor
   * @param min the minimum value of acessor return for list inclusion
   * @param max the maximum value of acessor return for list inclusion
   */
  QList< ControlMeasure * > ControlNet::sortedMeasureList(double(ControlMeasure::*statFunc)() const,
                                                          double min,double max) {

    QList< ControlMeasure * >measures;

    //build a list of all the pointers to the relevant measures
      //get the number of object points
    int nObjPts =  this->GetNumPoints();
    for (int i=0;i<nObjPts;i++) { //for each Object point
      ControlPoint *point = this->GetPoint(i);
      if (point->IsIgnored()) continue;  //if the point is ignored then continue

        //get the number of measures
      int nObs = point->GetNumMeasures();
      for (int j=0;j<nObs;j++) {  //for every measure
        ControlMeasure *measure = point->GetMeasure(j);
        if (measure->IsIgnored())  continue;
        double temp = (measure->*statFunc)();
        if (temp > min)
        if (min <= temp && temp <= max) measures.push_back(measure);
      }
    }

    //sort the measures
    ControlMeasureLessThanFunctor lessThan(statFunc);
    qSort(measures.begin(),measures.end(),lessThan);

    return measures;
  }


  /**
   * Essentially removes a cube from the networkid
   *
   * @param serialNumber The cube serial number to be removed from the network
   */
  void ControlNet::DeleteMeasuresWithId(iString serialNumber) {
    ValidateSerialNumber(serialNumber);

    ControlCubeGraphNode *csn = (*cubeGraphNodes)[serialNumber];
    QList< ControlMeasure * > measures = csn->getMeasures();
    foreach(ControlMeasure * measure, measures) {
      measure->Parent()->Delete(measure);
    }
  }


  /**
   * Compute error for each point in the network
   *
   * @history 2010-01-11  Tracie Sucharski, Renamed from ComputeErrors
   */
  void ControlNet::ComputeResiduals() {
    // TODO:  Make sure the cameras have been initialized

    QHashIterator< QString, ControlPoint * > i(*points);
    while (i.hasNext()) {
      i.next();
      i.value()->ComputeResiduals();
    }
  }


  /**
   * Compute aprior values for each point in the network
   *
   * @history 2010-08-23  Ken Edmundson, skipping ignored points
   */
  void ControlNet::ComputeApriori() {
    // TODO:  Make sure the cameras have been initialized
    QHashIterator< QString, ControlPoint * > i(*points);
    while (i.hasNext()) {
      i.next();
      ControlPoint *point = i.value();
      if ( !point->IsIgnored() )
        point->ComputeApriori();
    }
  }


  /**
   * Compute the average error of all points in the network
   * @return <B>double</B> Average error of points
   *
   * @history 2010-01-12  Tracie Sucharski - Renamed from AverageError
   */
  double ControlNet::AverageResidual() {
    // TODO:  Make sure the cameras have been initialized
    double avgResidual = 0.0;
    int count = 0;
    QHashIterator< QString, ControlPoint * > i(*points);
    while (i.hasNext()) {
      i.next();
      ControlPoint *point = i.value();
      if (!point->IsIgnored()) {
        avgResidual += point->GetStatistic(
            &ControlMeasure::GetResidualMagnitude).Average();
        count++;
      }
    }

    if (count != 0)
      avgResidual /= count;

    return avgResidual;
  }


  /**
   * Returns the camera list from the given image number
   *
   * @param index The image number
   *
   * @return Isis::Camera* The pointer to the resultant camera list
   */
  Isis::Camera *ControlNet::Camera(int index) {
    return p_cameraList[index];
  }


  /**
   * Return the Created Date
   *
   * @author Sharmila Prasad (10/6/2010)
   *
   * @return std::string
   */
  iString ControlNet::CreatedDate() const {
    return p_created;
  }


  /**
   * Return the description of the network
   *
   * @return The description of this Control Network
   */
  iString ControlNet::Description() const {
    return p_description;
  }


  /**
   * Finds and returns a pointer to the closest ControlPoint to the
   * ControlMeasure with the given serial number and line sample location
   *
   * @param serialNumber The serial number of the the file the ControlMeasure is
   *                     on
   * @param sample The sample number of the ControlMeasure
   * @param line The line number of the ControlMeasure
   *
   * @return <B>ControlPoint*</B> Pointer to the ControlPoint
   *         closest to the given line, sample position
   */
  ControlPoint *ControlNet::FindClosest(iString serialNumber,
      double sample, double line) {
    if (!cubeGraphNodes->contains(serialNumber)) {
      iString msg = "serialNumber [";
      msg += serialNumber;
      msg += "] not found in ControlNet";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    const double SEARCH_DISTANCE = 99999999.0;
    double minDist = SEARCH_DISTANCE;
    ControlPoint *closestPoint = NULL;

    ControlCubeGraphNode *csn = (*cubeGraphNodes)[serialNumber];
    QList< ControlMeasure * > measures = csn->getMeasures();
    for (int i = 0; i < measures.size(); i++) {
      ControlMeasure *measureToCheck = measures[i];

      //Find closest line sample & return that controlpoint
      double dx = fabs(sample - measureToCheck->GetSample());
      double dy = fabs(line - measureToCheck->GetLine());

      double dist = sqrt((dx * dx) + (dy * dy));

      if (dist < minDist) {
        minDist = dist;
        closestPoint = measureToCheck->Parent();
      }
    }

    if (!closestPoint) {
      iString msg = "No point found within ";
      msg += iString(SEARCH_DISTANCE);
      msg += "pixels of sample/line [";
      msg += iString(sample);
      msg += ", ";
      msg += iString(line);
      msg += "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return closestPoint;
  }


  //! Return if the control point is invalid
  bool ControlNet::IsValid() const {
    return !p_invalid;
  }


  /**
   * Determine the maximum error of all points in the network
   * @return <B>double</B> Max error of points
   *
   * @history 2010-01-12  Tracie Sucharski - Renamed from MaximumError
   */
  double ControlNet::GetMaximumResidual() {
    // TODO:  Make sure the cameras have been initialized

    double maxResidual = 0.0;
    foreach(ControlPoint * p, *points) {
      double residual = p->GetStatistic(
          &ControlMeasure::GetResidualMagnitude).Maximum();
      if (residual > maxResidual)
        maxResidual = residual;
    }

    return maxResidual;
  }


  iString ControlNet::GetNetworkId() const {
    return p_networkId;
  }


  /**
   * Return the total number of edit locked measures for all control points in the
   * network
   *
   * @return Number of edit locked measures
   */
  int ControlNet::GetNumEditLockMeasures() {
    int numLockedMeasures = 0;
    foreach(ControlPoint * p, *points) {
      numLockedMeasures += p->GetNumLockedMeasures();
    }

    return numLockedMeasures;
  }


  /**
   * Returns the number of edit locked control points
   *
   * @return Number of edit locked control points
   */
  int ControlNet::GetNumEditLockPoints() {
    int editLockPoints = 0;
    foreach(ControlPoint * p, *points) {
      if (p->IsEditLocked())
        editLockPoints++;
    }

    return editLockPoints;
  }


  /**
   * Return the total number of ignored measures for all control points in the
   * network
   *
   * @return Number of valid measures
   */
  int ControlNet::GetNumIgnoredMeasures() {
    int numIgnoredMeasures = 0;
    foreach(ControlPoint * p, *points) {
      numIgnoredMeasures += p->GetNumMeasures() - p->GetNumValidMeasures();
    }

    return numIgnoredMeasures;
  }


  /**
   * Return the number of measures in image specified by serialNumber
   *
   * @return Number of valid measures in image
   */
  int ControlNet::GetNumberOfMeasuresInImage(const std::string &serialNumber) {
    return p_cameraMeasuresMap[serialNumber];
  }


  /**
   * Return the number of jigsaw rejected measures in image specified by serialNumber
   *
   * @return Number of jigsaw rejected measures in image
   */
  int ControlNet::GetNumberOfJigsawRejectedMeasuresInImage(const std::string &serialNumber) {
    return p_cameraRejectedMeasuresMap[serialNumber];
  }


  /**
   * Sets jigsaw rejected flag to false for all points and measures.
   * Called by BundleAdjust::Init method
   *
   */
  void ControlNet::ClearJigsawRejected() {
    foreach(ControlPoint * p, *points) {
      p->ClearJigsawRejected();
    }
  }


  /**
   * Increment number of jigsaw rejected measures in image specified by serialNumber
   *
   */
  void ControlNet::IncrementNumberOfRejectedMeasuresInImage(const std::string &serialNumber) {
    p_cameraRejectedMeasuresMap[serialNumber]++;
  }


  /**
   * Decrement number of jigsaw rejected measures in image specified by serialNumber
   *
   */
  void ControlNet::DecrementNumberOfRejectedMeasuresInImage(const std::string &serialNumber) {
    if (p_cameraRejectedMeasuresMap[serialNumber] > 0)
      p_cameraRejectedMeasuresMap[serialNumber]--;
  }


  /**
   * Returns the total number of measures for all control points in the network
   *
   * @return Number of control measures
   */
  int ControlNet::GetNumMeasures() const {
    int numMeasures = 0;
    foreach(ControlPoint * p, *points) {
      numMeasures += p->GetNumMeasures();
    }

    return numMeasures;
  }


  //! Return the number of control points in the network
  int ControlNet::GetNumPoints() const {
    return points->size();
  }


  /**
   * Return the number of valid (non-ignored) measures for all control points
   * in the network
   *
   * @return Number of valid measures
   *
   * @internal
   * @history 2011-03-17 Debbie A. Cook - Modified to not count ignored measures.
   */
  int ControlNet::GetNumValidMeasures() {
    int numValidMeasures = 0;
    foreach(ControlPoint * p, *points) {
      if (!p->IsIgnored())
        numValidMeasures += p->GetNumValidMeasures();
    }

    return numValidMeasures;
  }


  /**
   * Returns the number of non-ignored control points
   *
   * @return Number of valid control points
   */
  int ControlNet::GetNumValidPoints() {
    int validPoints = 0;
    foreach(ControlPoint * p, *points) {
      if (!p->IsIgnored())
        validPoints++;
    }

    return validPoints;
  }


  //! Return the target name
  iString ControlNet::GetTarget() const {
    return p_targetName;
  }


  //! Return the user name
  iString ControlNet::GetUserName() const {
    return p_userName;
  }


  //! Return QList of ControlPoints
  QList< ControlPoint * > ControlNet::getPoints() const {
    return points->values();
  }


  //! Return QList of ControlPoint Ids used in hash, in order of addition
  QList< QString > ControlNet::GetPointIds() const {
    return *pointIds;
  }


  /**
   * Set the creation time
   *
   * @param date The date this Control Network was created
   */
  void ControlNet::SetCreatedDate(const iString &date) {
    p_created = date;
  }


  /**
    * Set the description of the network
    *
    * @param desc The description of this Control Network
    */
  void ControlNet::SetDescription(const iString &newDescription) {
    p_description = newDescription;
  }


  /**
   * Creates the ControlNet's image cameras based on an input file
   *
   * @param imageListFile The list of images
   */
  void ControlNet::SetImages(const iString &imageListFile) {
    SerialNumberList list(imageListFile);
    SetImages(list);
  }


  /**
   * Creates the ControlNet's image camera's based on the list of Serial Numbers
   *
   * @param list The list of Serial Numbers
   * @param progress A pointer to the progress of creating the cameras
   * @throws Isis::iException::System - "Unable to create camera
   *        for cube file"
   * @throws Isis::iException::User - "Control point measure does
   *        not have a cube with a matching serial number"
   * @internal
   *   @history 2009-01-06 Jeannie Walldren - Fixed typo in
   *            exception output.
   */
  void ControlNet::SetImages(SerialNumberList &list, Progress *progress) {
    // Prep for reporting progress
    if (progress != NULL) {
      progress->SetText("Setting input images...");
      progress->SetMaximumSteps(list.Size());
      progress->CheckStatus();
    }
    // Open the camera for all the images in the serial number list
    for (int i = 0; i < list.Size(); i++) {
      iString serialNumber = list.SerialNumber(i);
      iString filename = list.Filename(i);
      Pvl pvl(filename);

      try {
        Isis::Camera *cam = CameraFactory::Create(pvl);
        p_cameraMap[serialNumber] = cam;
        p_cameraMeasuresMap[serialNumber] = 0;
        p_cameraRejectedMeasuresMap[serialNumber] = 0;
        p_cameraList.push_back(cam);
      }
      catch (IException &e) {
        iString msg = "Unable to create camera for cube file ";
        msg += filename;
        throw IException(e, IException::Unknown, msg, _FILEINFO_);
      }

      if (progress != NULL)
        progress->CheckStatus();
    }

    // Loop through all measures and set the camera
    QHashIterator< QString, ControlPoint * > p(*points);
    while (p.hasNext()) {
      p.next();
      ControlPoint *curPoint = p.value();

      QList< QString > serialNums = curPoint->getCubeSerialNumbers();
      for (int m = 0; m < serialNums.size(); m++) {
        ControlMeasure *curMeasure = (*curPoint)[serialNums[m]];

        if (!curMeasure->IsIgnored()) {
          iString serialNumber = curMeasure->GetCubeSerialNumber();
          if (list.HasSerialNumber(serialNumber)) {
            curMeasure->SetCamera(p_cameraMap[serialNumber]);

            // increment number of measures for this image (camera)
            p_cameraMeasuresMap[serialNumber]++;
          }
          else {
            iString msg = "Control point [" + curPoint->GetId() +
                "], measure [" + curMeasure->GetCubeSerialNumber() +
                "] does not have a cube with a matching serial number";
            throw IException(IException::User, msg, _FILEINFO_);
          }
        }
      }
    }
  }


  /**
   * Set the last modified date
   *
   * @param date The last date this Control Network was modified
   */
  void ControlNet::SetModifiedDate(const iString &date) {
    p_modified = date;
  }


  /**
   * Set the network id
   *
   * @param id The Id of this Control Network
   */
  void ControlNet::SetNetworkId(const iString &id) {
    p_networkId = id;
  }


  /**
   * Set the target name
   *
   * @param target The name of the target of this Control Network
   */
  void ControlNet::SetTarget(const iString &target) {
    p_targetName = target;

    p_targetRadii.clear();
    if (p_targetName != "") {
      PvlGroup pvlRadii = Projection::TargetRadii(target);
      p_targetRadii.push_back(Distance(pvlRadii["EquatorialRadius"],
                                       Distance::Meters));
      // The method Projection::Radii does not provide the B radius
      p_targetRadii.push_back(Distance(pvlRadii["EquatorialRadius"],
                                       Distance::Meters));
      p_targetRadii.push_back(Distance(pvlRadii["PolarRadius"],
                                       Distance::Meters));
    }
    else {
      p_targetRadii.push_back(Distance());
      p_targetRadii.push_back(Distance());
      p_targetRadii.push_back(Distance());
    }
  }


  /**
   * Set the user name
   *
   * @param name The name of the user creating or modifying this Control Net
   */
  void ControlNet::SetUserName(const iString &name) {
    p_userName = name;
  }

  const ControlNet &ControlNet::operator=(ControlNet other) {
    if (this == &other)
      return *this;

    if (points) {
      QHashIterator< QString, ControlPoint * > i(*points);
      while (i.hasNext()) {
        i.next();
        delete(*points)[i.key()];
        (*points)[i.key()] = NULL;
      }
      delete points;
      points = NULL;
    }

    if (cubeGraphNodes) {
      QHashIterator< QString, ControlCubeGraphNode * > i(*cubeGraphNodes);
      while (i.hasNext()) {
        i.next();
        delete(*cubeGraphNodes)[i.key()];
        (*cubeGraphNodes)[i.key()] = NULL;
      }
      delete cubeGraphNodes;
      cubeGraphNodes = NULL;
    }

    points = new QHash< QString, ControlPoint * >;
    cubeGraphNodes = new QHash< QString, ControlCubeGraphNode * >;

    QHashIterator < QString, ControlPoint * > i(*other.points);
    while (i.hasNext()) {
      i.next();

      ControlPoint *newPoint = new ControlPoint(*i.value());
      newPoint->parentNetwork = this;
      QString newPointId = newPoint->GetId();
      points->insert(newPointId, newPoint);

      // create graph for non-ignored points and measures
      if (!newPoint->IsIgnored()) {
        QList< ControlMeasure * > measures = newPoint->getMeasures();
        for (int i = 0; i < measures.size(); i++) {
          ControlMeasure *measure = measures[i];
          QString serial = measure->GetCubeSerialNumber();

          if (!measure->IsIgnored()) {
            if (cubeGraphNodes->contains(serial)) {
              (*cubeGraphNodes)[serial]->addMeasure(measure);
            }
            else {
              ControlCubeGraphNode *newControlCubeGraphNode =
                new ControlCubeGraphNode(serial);

              newControlCubeGraphNode->addMeasure(measure);
              cubeGraphNodes->insert(serial, newControlCubeGraphNode);
            }
          }
        }
      }
    }

    p_targetName = other.p_targetName;
    p_targetRadii = other.p_targetRadii;
    p_networkId = other.p_networkId;
    p_created = other.p_created;
    p_modified = other.p_modified;
    p_description = other.p_description;
    p_userName = other.p_userName;
    p_invalid = other.p_invalid;
    p_cameraMap = other.p_cameraMap;
    p_cameraList = other.p_cameraList;

    return *this;
  }


  const ControlPoint *ControlNet::GetPoint(QString id) const {
    if (!points->contains(id)) {
      iString msg = "The control network has no control points with an ID "
          "equal to [" + id + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return points->value(id);
  }


  ControlPoint *ControlNet::GetPoint(QString id) {
    if (!points->contains(id)) {
      iString msg = "The control network has no control points with an ID "
          "equal to [" + id + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return (*points)[id];
  }


  const ControlPoint *ControlNet::GetPoint(int index) const {
    if (index < 0 || index >= pointIds->size()) {
      iString msg = "Index [" + iString(index) + "] out of range";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return GetPoint(pointIds->at(index));
  }


  ControlPoint *ControlNet::GetPoint(int index) {
    if (index < 0 || index >= pointIds->size()) {
      iString msg = "Index [" + iString(index) + "] out of range";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return GetPoint(pointIds->at(index));
  }


  const ControlCubeGraphNode *ControlNet::getGraphNode(
      QString serialNumber) const {
    if (!cubeGraphNodes->contains(serialNumber)) {
      iString msg = "Serial Number [" + serialNumber + "] does not exist in"
          " the network.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return cubeGraphNodes->value(serialNumber);
  }


  /**
   * Get the target radii
   *
   * @returns the radii of the target body
   */
  std::vector<Distance> ControlNet::GetTargetRadii() {
    return p_targetRadii;
  }


  const ControlPoint *ControlNet::operator[](QString id) const {
    return GetPoint(id);
  }


  ControlPoint *ControlNet::operator[](QString id) {
    return GetPoint(id);
  }


  const ControlPoint *ControlNet::operator[](int index) const {
    return GetPoint(index);
  }


  ControlPoint *ControlNet::operator[](int index) {
    return GetPoint(index);
  }
}
