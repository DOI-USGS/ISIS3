/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2009/07/15 17:33:52 $
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

#include "ConcurrentControlNetReader.h"

#include <algorithm>
#include <iostream>

#include <QFuture>
#include <QFutureWatcher>
#include <QString>
#include <QtConcurrentRun>
#include <QtConcurrentMap>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlNetFileV0002.pb.h"
#include "ControlNetVersioner.h"
#include "ControlPoint.h"
#include "Distance.h"
#include "Filename.h"
#include "Projection.h"


using std::cerr;
using std::cout;
using std::swap;


namespace Isis {
  
  /**
   * Allocates memory at construction instead of as needed
   */
  ConcurrentControlNetReader::ConcurrentControlNetReader() {
    nullify();

    m_originalThreadCount = 1;

    m_readWatcher = new QFutureWatcher<LatestControlNetFile *>;
    m_builderWatcher = new QFutureWatcher< QAtomicPointer< ControlNet > >;

    connect(m_readWatcher, SIGNAL(finished()),
        this, SLOT(startBuildingNetwork()));
    connect(m_builderWatcher, SIGNAL(finished()),
        this, SLOT(networkBuilt()));
    connect(m_builderWatcher, SIGNAL(progressRangeChanged(int, int)),
        this, SIGNAL(progressRangeChanged(int, int)));
    connect(m_builderWatcher, SIGNAL(progressValueChanged(int)),
        this, SIGNAL(progressValueChanged(int)));
  }


  /**
   * This destructor will cancel all running threads and block.
   */
  ConcurrentControlNetReader::~ConcurrentControlNetReader() {
    
    if (m_readWatcher)
    {
      m_readWatcher->cancel();
      m_readWatcher->waitForFinished();
    }
    
    if (m_builderWatcher)
    {
      m_builderWatcher->cancel();
      m_builderWatcher->waitForFinished();
    }
      
    delete m_readWatcher;
    m_readWatcher = NULL;

    delete m_builderWatcher;
    m_builderWatcher = NULL;

    delete m_versionerFile; // should be NULL anyway
    m_versionerFile = NULL;
  }


  /**
   * @param filename The filename of the network to read
   */
  void ConcurrentControlNetReader::read(QString filename) {
    m_originalThreadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(1);
    QFuture<LatestControlNetFile *> result =
        QtConcurrent::run(ReadNetworkFunctor(filename));
    m_readWatcher->setFuture(result);
  }


  /**
   * Initializes members to NULL
   */
  void ConcurrentControlNetReader::nullify() {
    m_readWatcher = NULL;
    m_builderWatcher = NULL;
    m_versionerFile = NULL;
  }


  /**
   * Called when we have a binary container for the network, and in turn
   * creates the network using QtConcurrent::mappedReduced()
   */
  void ConcurrentControlNetReader::startBuildingNetwork() {
    m_versionerFile = m_readWatcher->result();
    QString targetName;
   
    if (m_versionerFile->GetNetworkHeader().has_targetname())
      targetName = QString::fromStdString(
          m_versionerFile->GetNetworkHeader().targetname());

    QFuture< QAtomicPointer< ControlNet > > result;

    result = QtConcurrent::mappedReduced(m_versionerFile->GetNetworkPoints(),
        NetworkBuilder(targetName, QThread::currentThread()),
        &NetworkBuilder::addToNetwork,
        QtConcurrent::SequentialReduce |
        QtConcurrent::OrderedReduce);

    m_builderWatcher->setFuture(result);
  }


  /**
   * Called when the ControlNet is finished being populated with points.  This
   * method sets the header data and finally emits networkReadFinished()
   */
  void ConcurrentControlNetReader::networkBuilt() {
    ControlNet *net = m_builderWatcher->result();
    if (!net)
      net = new ControlNet;

    ControlNetFileHeaderV0002 &header = m_versionerFile->GetNetworkHeader();

    if (header.has_targetname())
      net->SetTarget(header.targetname());
    else
      net->SetTarget("");

    net->SetDescription(header.description());
    net->SetUserName(header.username());
    net->SetCreatedDate(header.created());
    net->SetNetworkId(header.networkid());
    net->SetModifiedDate(header.lastmodified());

    delete m_versionerFile;
    m_versionerFile = NULL;

    QThreadPool::globalInstance()->setMaxThreadCount(m_originalThreadCount);
    emit networkReadFinished(net);
  }


  /**
   * Sets up the ReadNetworkFunctor which provides the binary container for
   * the NetworkBuilder.
   * 
   * @param someNetworkFilename filename of a ControlNet file.
   */
  ConcurrentControlNetReader::ReadNetworkFunctor::ReadNetworkFunctor(
    QString someNetworkFilename) {
    m_networkFilename = someNetworkFilename;
  }


  ConcurrentControlNetReader::ReadNetworkFunctor::~ReadNetworkFunctor() {}


  LatestControlNetFile *
  ConcurrentControlNetReader::ReadNetworkFunctor::operator()() const {
    return ControlNetVersioner::Read(Filename(m_networkFilename.toStdString()));
  }


  ConcurrentControlNetReader::NetworkBuilder::NetworkBuilder(QString target,
      QThread *targetThread) {
    nullify();

    m_targetThread = targetThread;

    if (!target.isEmpty()) {
      PvlGroup pvlRadii = Projection::TargetRadii(target.toStdString());
      m_majorRad = new Distance(pvlRadii["EquatorialRadius"], Distance::Meters);
      m_minorRad = new Distance(pvlRadii["EquatorialRadius"], Distance::Meters);
      m_polarRad = new Distance(pvlRadii["PolarRadius"], Distance::Meters);
    }
    else {
      m_majorRad = new Distance();
      m_minorRad = new Distance();
      m_polarRad = new Distance();
    }
  }


  ConcurrentControlNetReader::NetworkBuilder::NetworkBuilder(
    NetworkBuilder const &other) {
    nullify();

    m_targetThread = other.m_targetThread;

    m_majorRad = new Distance(*other.m_majorRad);
    m_minorRad = new Distance(*other.m_minorRad);
    m_polarRad = new Distance(*other.m_polarRad);
  }


  ConcurrentControlNetReader::NetworkBuilder::~NetworkBuilder() {
    if (m_majorRad) {
      delete m_majorRad;
      m_majorRad = NULL;
    }

    if (m_minorRad) {
      delete m_minorRad;
      m_minorRad = NULL;
    }

    if (m_polarRad) {
      delete m_polarRad;
      m_polarRad = NULL;
    }
  }


  ControlPoint *ConcurrentControlNetReader::NetworkBuilder::operator()(
    const ControlPointFileEntryV0002 &versionerPoint) const {
    ControlPoint * cp = new ControlPoint(versionerPoint, *m_majorRad, *m_minorRad,
        *m_polarRad);
    cp->moveToThread(m_targetThread);

    for (int measureIndex = 0;
         measureIndex < cp->GetNumMeasures();
         measureIndex++) {
      cp->GetMeasure(measureIndex)->moveToThread(m_targetThread);
    }

    return cp;
  }


  ConcurrentControlNetReader::NetworkBuilder &
  ConcurrentControlNetReader::NetworkBuilder::operator=(
    NetworkBuilder other) {
    swap(m_targetThread, other.m_targetThread);

    swap(*m_majorRad, *other.m_majorRad);
    swap(*m_minorRad, *other.m_minorRad);
    swap(*m_polarRad, *other.m_polarRad);

    return *this;
  }


  void ConcurrentControlNetReader::NetworkBuilder::addToNetwork(
    QAtomicPointer< ControlNet > & network,
    ControlPoint *const &item) {
    if (!network) {
      network = new ControlNet;
      network->moveToThread(item->thread());
    }

    network->AddPoint(item);
  }


  void ConcurrentControlNetReader::NetworkBuilder::nullify() {
    m_majorRad = NULL;
    m_minorRad = NULL;
    m_polarRad = NULL;
    m_targetThread = NULL;
  }
}

