#include "BundleSolutionInfo.h"

#include <QDataStream>
#include <QDebug>
#include <QList>
#include <QUuid>
#include <QXmlStreamWriter>

#include <hdf5_hl.h> // in the hdf5 library
#include <hdf5.h>
#include <H5Cpp.h>

#include "BundleResults.h"
#include "ControlNet.h"
#include "FileName.h"
#include "ImageList.h"
#include "IString.h"
#include "Project.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {

  BundleSolutionInfo::BundleSolutionInfo(BundleSettingsQsp inputSettings,
                                         FileName controlNetworkFileName,
                                         BundleResults outputStatistics, 
                                         QObject *parent) : QObject(parent) {
    m_id = NULL;
    m_id = new QUuid(QUuid::createUuid());

    m_runTime = "";

    m_controlNetworkFileName = NULL;
    m_controlNetworkFileName = new FileName(controlNetworkFileName);

    m_settings = inputSettings;

    m_statisticsResults = NULL;
    m_statisticsResults = new BundleResults(outputStatistics);

    m_images = NULL;
    m_images = new QList<ImageList *>;
  }



  BundleSolutionInfo::BundleSolutionInfo(Project *project, 
                                         XmlStackedHandlerReader *xmlReader, 
                                         QObject *parent) : QObject(parent) {   // TODO: does xml stuff need project???
    m_id = NULL;
    // what about the rest of the member data ??? should we set defaults ??? CREATE INITIALIZE METHOD

    xmlReader->pushContentHandler(new XmlHandler(this, project));
    xmlReader->setErrorHandler(new XmlHandler(this, project));
  }


  BundleSolutionInfo::BundleSolutionInfo(FileName bundleSolutionInfoFile) {
    m_id = NULL;
    m_id = new QUuid(QUuid::createUuid());

    m_statisticsResults = NULL;
    m_statisticsResults = new BundleResults();

    m_settings = BundleSettingsQsp(new BundleSettings);

    m_images = NULL;
    m_images = new QList<ImageList *>;// change to QList<QStringList> ???

    openH5File(bundleSolutionInfoFile);
  }


  BundleSolutionInfo::BundleSolutionInfo(const BundleSolutionInfo &src)
      : m_id(new QUuid(src.m_id->toString())),
        m_runTime(src.m_runTime),
        m_controlNetworkFileName(new FileName(src.m_controlNetworkFileName->expanded())),
        m_settings(new BundleSettings(*src.m_settings)),
        m_statisticsResults(new BundleResults(*src.m_statisticsResults)),
        m_images(new QList<ImageList *>(*src.m_images)) { // is this correct???

    // m_images = NULL;
    // m_images = new QList<ImageList *>;
    // for (int i = 0; i < src.m_images->size(); i++) {
    //   m_images->append(src.m_images->at(i));
    // }

  }



  BundleSolutionInfo::~BundleSolutionInfo() {
    delete m_id;
    m_id = NULL;

    delete m_controlNetworkFileName;
    m_controlNetworkFileName = NULL;

    delete m_statisticsResults;
    m_statisticsResults = NULL;

    delete m_images;
    m_images = NULL;
  }


  
  BundleSolutionInfo &BundleSolutionInfo::operator=(const BundleSolutionInfo &src) {

    if (&src != this) {

      delete m_id;
      m_id = NULL;
      m_id = new QUuid(src.m_id->toString());

      m_runTime = src.m_runTime;

      delete m_controlNetworkFileName;
      m_controlNetworkFileName = NULL;
      m_controlNetworkFileName = new FileName(src.m_controlNetworkFileName->expanded());

      m_settings = src.m_settings;

      delete m_statisticsResults;
      m_statisticsResults = NULL;
      m_statisticsResults = new BundleResults(*src.m_statisticsResults);

      delete m_images;
      m_images = NULL;
      m_images = new QList<ImageList *>(*src.m_images);
    }
    return *this;
  }



  void BundleSolutionInfo::setOutputStatistics(BundleResults statisticsResults) {
    delete m_statisticsResults;
    m_statisticsResults = NULL;
    m_statisticsResults = new BundleResults(statisticsResults);
  }




  PvlObject BundleSolutionInfo::pvlObject(QString resultsName, QString settingsName, 
                                          QString statisticsName) {

    PvlObject pvl(resultsName);
    pvl += PvlKeyword("RunTime", runTime());
    if (m_controlNetworkFileName->expanded() != "") {
      pvl += PvlKeyword("OutputControlNetwork", controlNetworkFileName());
    }
    pvl += bundleSettings()->pvlObject(settingsName);
    pvl += bundleResults().pvlObject(statisticsName);
    return pvl;

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
  void BundleSolutionInfo::save(QXmlStreamWriter &stream, const Project *project,
                                FileName newProjectRoot) const {

    stream.writeStartElement("bundleSolutionInfo");
    // save ID, cnet file name, and run time to stream
    stream.writeStartElement("generalAttributes");
    stream.writeTextElement("id", m_id->toString());
    stream.writeTextElement("runTime", runTime());
    stream.writeTextElement("fileName", m_controlNetworkFileName->expanded());
    stream.writeEndElement(); // end general attributes

    // save settings to stream
    m_settings->save(stream, project);

    // save statistics to stream
    m_statisticsResults->save(stream, project);

    // save image lists to stream
    if ( !m_images->isEmpty() ) {
      stream.writeStartElement("imageLists");

      for (int i = 0; i < m_images->count(); i++) {
        m_images->at(i)->save(stream, project, "");
      }

      stream.writeEndElement();
    }
    stream.writeEndElement(); //end bundleSolutionInfo
  }



  void BundleSolutionInfo::save(QXmlStreamWriter &stream, const Project *project) const {

    stream.writeStartElement("bundleSolutionInfo");
    // save ID, cnet file name, and run time to stream
    stream.writeStartElement("generalAttributes");
    stream.writeTextElement("id", m_id->toString());
    stream.writeTextElement("runTime", runTime());
    stream.writeTextElement("fileName", m_controlNetworkFileName->expanded());
    stream.writeEndElement(); // end general attributes

    // save settings to stream
    m_settings->save(stream, project);

    // save statistics to stream
    m_statisticsResults->save(stream, project);

    // save image lists to stream
    if ( !m_images->isEmpty() ) {
      stream.writeStartElement("imageLists");

      for (int i = 0; i < m_images->count(); i++) {
        m_images->at(i)->save(stream, project, "");
      }

      stream.writeEndElement();
    }
    stream.writeEndElement(); //end bundleSolutionInfo
  }



  /**
   * Create an XML Handler (reader) that can populate the BundleSettings class data. See
   *   BundleSettings::save() for the expected format.
   *
   * @param bundleSettings The image we're going to be initializing
   * @param imageFolder The folder that contains the Cube
   */
  BundleSolutionInfo::XmlHandler::XmlHandler(BundleSolutionInfo *bundleSolutionInfo, 
                                             Project *project) {
    m_xmlHandlerBundleSolutionInfo = bundleSolutionInfo;
    m_xmlHandlerProject = NULL;
    m_xmlHandlerProject = project;
    m_xmlHandlerCharacters = "";
    m_xmlHandlerImages = NULL;
    m_xmlHandlerBundleResults = NULL;
  }



  BundleSolutionInfo::XmlHandler::~XmlHandler() {
    // bundleSolutionInfo passed in is "this" delete+null will cause problems,no?
//    delete m_xmlHandlerBundleSolutionInfo;
//    m_xmlHandlerBundleSolutionInfo = NULL;

    // we do not delete this pointer since it was set to a passed in pointer in constructor and we
    // don't own it... is that right???
//    delete m_xmlHandlerProject;
    m_xmlHandlerProject = NULL;

    delete m_xmlHandlerImages;
    m_xmlHandlerImages = NULL;

    delete m_xmlHandlerBundleResults;
    m_xmlHandlerBundleResults = NULL;
  }



  /**
   * Handle an XML start element. This expects <image/> and <displayProperties/> elements.
   *
   * @return If we should continue reading the XML (usually true).
   */
  bool BundleSolutionInfo::XmlHandler::startElement(const QString &namespaceURI, 
                                                    const QString &localName,
                                                    const QString &qName,
                                                    const QXmlAttributes &atts) {
    m_xmlHandlerCharacters = "";

    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {

      if (localName == "bundleSettings") {
        m_xmlHandlerBundleSettings =
            BundleSettingsQsp(new BundleSettings(m_xmlHandlerProject, reader()));
      }
      else if (localName == "bundleResults") {
        delete m_xmlHandlerBundleResults;
        m_xmlHandlerBundleResults = NULL;
        m_xmlHandlerBundleResults = new BundleResults(m_xmlHandlerProject, reader()); //TODO: need to add constructor for this???
      }
      else if (localName == "imageList") {
        m_xmlHandlerImages->append(new ImageList(m_xmlHandlerProject, reader()));
      }
    }
    return true;
  }



  bool BundleSolutionInfo::XmlHandler::characters(const QString &ch) {
    m_xmlHandlerCharacters += ch;
    return XmlStackedHandler::characters(ch);
  }



  bool BundleSolutionInfo::XmlHandler::endElement(const QString &namespaceURI, 
                                                  const QString &localName,
                                                  const QString &qName) {
    if (localName == "id") {
      m_xmlHandlerBundleSolutionInfo->m_id = NULL;
      m_xmlHandlerBundleSolutionInfo->m_id = new QUuid(m_xmlHandlerCharacters);
    }
    else if (localName == "runTime") {
      m_xmlHandlerBundleSolutionInfo->m_runTime = m_xmlHandlerCharacters;
    }
    else if (localName == "fileName") {
      m_xmlHandlerBundleSolutionInfo->m_controlNetworkFileName = NULL;
      m_xmlHandlerBundleSolutionInfo->m_controlNetworkFileName = new FileName(m_xmlHandlerCharacters);
    }
    else if (localName == "bundleSettings") {
      m_xmlHandlerBundleSolutionInfo->m_settings =
          BundleSettingsQsp(new BundleSettings(*m_xmlHandlerBundleSettings));
    }
    else if (localName == "bundleResults") {
      m_xmlHandlerBundleSolutionInfo->m_statisticsResults = new BundleResults(*m_xmlHandlerBundleResults);
      delete m_xmlHandlerBundleResults;
      m_xmlHandlerBundleResults = NULL;
    }
    if (localName == "imageLists") {
      for (int i = 0; i < m_xmlHandlerImages->size(); i++) {
        m_xmlHandlerBundleSolutionInfo->m_images->append(m_xmlHandlerImages->at(i));
      }
      m_xmlHandlerImages->clear();
    }
    m_xmlHandlerCharacters = "";
    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }



  /**
   * Get a unique, identifying string associated with this BundleSolutionInfo object.
   *
   * @return A unique ID for this BundleSolutionInfo object
   */
  QString BundleSolutionInfo::id() const {
    return m_id->toString().remove(QRegExp("[{}]"));
  }


  void BundleSolutionInfo::setRunTime(QString runTime) { 
    // ??? validate that a valid time has been given???
    // try {
    //   iTime time(runTime);
    // }
    // catch (...) {
    //   throw IException(IException::Unknown,
    //                    "Invalid bundle adjustment run time [" + runTime + ].",
    //                    _FILEINFO_);
    // }
    m_runTime = runTime;
  }


  QString BundleSolutionInfo::runTime() const {
    return m_runTime;
  }


  QString BundleSolutionInfo::controlNetworkFileName() const {
    return m_controlNetworkFileName->expanded();
  }

  BundleSettingsQsp BundleSolutionInfo::bundleSettings() {
    return m_settings;
  }

  BundleResults BundleSolutionInfo::bundleResults() {
    if (m_statisticsResults) {
      return *m_statisticsResults;
    }
    else {
      throw IException(IException::Unknown, 
                       "Results for this bundle is NULL.",
                       _FILEINFO_);
    }
  }

  QDataStream &BundleSolutionInfo::write(QDataStream &stream) const {
    stream << m_id->toString()
           << m_runTime
           << m_controlNetworkFileName->expanded()
           << *m_settings
           << *m_statisticsResults;
  // TODO: add this capability to Image and ImageList
  //          << *m_images;
    return stream;
  }



  QDataStream &BundleSolutionInfo::read(QDataStream &stream) {

    QString id;
    stream >> id;
    delete m_id;
    m_id = NULL;
    m_id = new QUuid(id);

    stream >> m_runTime;

    QString controlNetworkFileName;
    stream >> controlNetworkFileName;
    delete m_controlNetworkFileName;
    m_controlNetworkFileName = NULL;
    m_controlNetworkFileName = new FileName(controlNetworkFileName);

    BundleSettings settings;
    stream >> settings;
    m_settings = BundleSettingsQsp(new BundleSettings(settings));

    BundleResults statisticsResults;
    stream >> statisticsResults;
    delete m_statisticsResults;
    m_statisticsResults = NULL;
    m_statisticsResults = new BundleResults(statisticsResults);

    // TODO: add this capability to Image and ImageList
    // QList<ImageList*> imageLists;
    // stream >> imageLists;
    // delete m_images;
    // m_images = NULL;
    // m_images = new QList<ImageList *>(imageLists);

    return stream;
  }



  QDataStream &operator<<(QDataStream &stream, const BundleSolutionInfo &bundleSolutionInfo) {
    return bundleSolutionInfo.write(stream);
  }



  QDataStream &operator>>(QDataStream &stream, BundleSolutionInfo &bundleSolutionInfo) {
    return bundleSolutionInfo.read(stream);
  }



  void BundleSolutionInfo::openH5File(FileName bundleSolutionInfoFile) {

    try {
      if (!bundleSolutionInfoFile.fileExists()) {
        QString msg = "No file with the given name was found.";
        throw IException(IException::Io, msg, _FILEINFO_);
      }

      if (QString::compare(bundleSolutionInfoFile.extension(), "hdf", Qt::CaseInsensitive) != 0) {
        QString msg = "The given file is unsupported for constructing BundleSolutionInfo objects. "
                      "Supported file types include [hdf].";
        throw IException(IException::Io, msg, _FILEINFO_);
      }

      // catch H5 exceptions and rethrow
      try {
        /*
         * Turn off the auto-printing when failure occurs so that we can
         * handle the errors appropriately
         */
//        H5::Exception::dontPrint();//??? uncomment
    
        /* 
         * Create and open an H5File object with read-only access. This will throw an
         * H5 exception if the open fails.
         * 
         * The static H5Fopen() function will returns a negative file ID if it fails. So, in this
         * case, we would have to take the extra step of checking whether the return
         * is negative.
         */

        const H5std_string hdfFileName(bundleSolutionInfoFile.expanded().toStdString()); //??? move inside try ???
        H5::H5File hdfFile( hdfFileName, H5F_ACC_RDONLY ); // valgrind: Invalid read of size 4

        // get the BundleSolutionInfo group
        QString root = "/";
        QString bundleRunGroupName = root + "BundleSolutionInfo";
        H5::Group bundleRunGroup = hdfFile.openGroup(bundleRunGroupName.toLatin1());

        /* 
         * Add basic attributes
         */ 
        H5std_string attValue;

        Attribute att = bundleRunGroup.openAttribute("runTime");
        H5::StrType strDataType(H5::PredType::C_S1, att.getStorageSize());
        att.read(strDataType, attValue);
        m_runTime = QString::fromStdString(attValue);

        att = bundleRunGroup.openAttribute("controlNetworkFileName");
        strDataType = H5::StrType(H5::PredType::C_S1, att.getStorageSize());
        att.read(strDataType, attValue);
        m_controlNetworkFileName = new FileName(QString::fromStdString(attValue));

        m_settings->openH5Group(bundleRunGroup, bundleRunGroupName);
        m_statisticsResults->openH5Group(bundleRunGroup, bundleRunGroupName);

        // ???Let's just save off the image list file names for now...
        QString   imagesGroupName = bundleRunGroupName + "/imageLists";
        H5::Group imagesGroup     = bundleRunGroup.openGroup(imagesGroupName.toLatin1());
        int imagesGroupSize = (int)imagesGroup.getNumObjs();

        for (int i = 0; i < imagesGroupSize; i++) {
          H5std_string listGroupName = imagesGroup.getObjnameByIdx(i);
          H5::Group listGroup = imagesGroup.openGroup(listGroupName);
          
          H5std_string attValue;
          att = listGroup.openAttribute("path");
          strDataType = H5::StrType(H5::PredType::C_S1, att.getStorageSize());
          att.read(strDataType, attValue);
          QString listPath = QString::fromStdString(attValue);
          QString listName = QString::fromStdString(listGroupName).remove(imagesGroupName + "/");

          ImageList *imageList = new ImageList(listName, listPath);

          att = listGroup.openAttribute("fileNames");
          strDataType = H5::StrType(H5::PredType::C_S1, att.getStorageSize());
          att.read(strDataType, attValue);
          QStringList fileList = QString::fromStdString(attValue).split(",");
          for (int j = 0; j < fileList.size();j++) {
            imageList->append(new Image(fileList[j]));
          }

          m_images->append(imageList);// delete and null imageList???

        }
  #if 0
        
        QString   imagesGroupName = bundleRunGroupName + "/imageLists";
        H5::Group imagesGroup     = bundleRunGroup.openGroup(imagesGroupName.toLatin1());
        int imagesGroupSize = (int)imagesGroup.getNumObjs();

        for (int i = 0; i < imagesGroupSize; i++) {
          H5std_string listGroupName = imagesGroup.getObjnameByIdx(i);
          H5::Group listGroup = imagesGroup.openGroup(listGroupName);

          H5std_string attValue;
          att = listGroup.openAttribute("path");
          strDataType = H5::StrType(H5::PredType::C_S1, att.getStorageSize());
          att.read(strDataType, attValue);
          QString listPath = QString::fromStdString(attValue);
          QString listName = QString::fromStdString(listGroupName).remove(imagesGroupName + "/");

          ImageList *imageList = new ImageList(listName, listPath);
          imageList->openH5Group(bundleRunGroup, bundleRunGroupName);
          m_images.append(imageList);
        }
#endif

      }
      catch (H5::Exception error) {  //??? how to improve printed msg using major/minor error codes?
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 exception handler has detected an error when invoking the function " 
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
    }
    catch (IException &e) {
      QString msg = "Unable to read bundle solution information from the given HDF5 file ["
                    + bundleSolutionInfoFile.expanded() + "].";
      throw IException(e, IException::Unknown, msg, _FILEINFO_);
    }
  }


  void BundleSolutionInfo::createH5File(FileName outputFileName) const {

    try {
      if (outputFileName.fileExists()) {
        QString msg = "A file already exists with the given name ["
                      + outputFileName.expanded() + "].";
        throw IException(IException::Io, msg, _FILEINFO_);
      }

      // Try block to detect exceptions raised by any of the calls inside it
      try {
        /*
         * Turn off the auto-printing when failure occurs so that we can
         * handle the errors appropriately
         */
        //H5::Exception::dontPrint();

        /*
         * Create a new file using H5F_ACC_EXCL access,
         * default file creation properties, and default file
         * access properties.
         */
        const H5std_string hdfFileName(outputFileName.expanded().toStdString());
//        const H5std_string hdfFileName("./BundleSolutionInfo.hdf");
        H5::H5File hdfFile(hdfFileName, H5F_ACC_EXCL); // valgrind: Invalid read of size 4???

        // create BundleSolutionInfo group
        QString   root               = "/";
        QString   bundleRunGroupName = root + "BundleSolutionInfo";
        H5::Group bundleRunGroup     = hdfFile.createGroup(bundleRunGroupName.toLatin1());

        /* 
         * Add basic attributes
         */
        Attribute     att;
        H5::DataSpace spc(H5S_SCALAR); // single value space
        QString       attValue    = "";

        H5::StrType   strDataType(H5::PredType::C_S1, m_runTime.length());
        att = bundleRunGroup.createAttribute("runTime", strDataType, spc);
        att.write(strDataType, m_runTime.toStdString());

        attValue = m_controlNetworkFileName->expanded();
        strDataType = H5::StrType(H5::PredType::C_S1, attValue.length());
        att = bundleRunGroup.createAttribute("controlNetworkFileName", strDataType, spc);
        att.write(strDataType, attValue.toStdString());

        m_settings->createH5Group(bundleRunGroup, bundleRunGroupName);
        m_statisticsResults->createH5Group(bundleRunGroup, bundleRunGroupName);

        // Let's just save off the image list file names for now...
        QString   imagesGroupName = bundleRunGroupName + "/imageLists";
        H5::Group imagesGroup     = bundleRunGroup.createGroup(imagesGroupName.toLatin1());

        QString listGroupName = "";
        int     stringSize    = 0;

        for (int i = 0; i < m_images->size(); i++) {
          listGroupName = imagesGroupName + "/" + (*m_images)[i]->name();

          H5::Group listGroup = imagesGroup.createGroup(listGroupName.toLatin1());

          attValue = (*m_images)[i]->path();
          strDataType = H5::StrType(H5::PredType::C_S1, attValue.length());
          att = listGroup.createAttribute("path", strDataType, spc);
          att.write(strDataType, attValue.toStdString());

          QStringList fileList;
          for (int j = 0; j < (*m_images)[i]->size(); j++) {
            fileList += (*(*m_images)[i])[j]->fileName(); //???
          }

          QString fileNames = fileList.join(",");
          stringSize = qMax(fileNames.length(), 1);
          strDataType = H5::StrType(H5::PredType::C_S1, stringSize);
          att = listGroup.createAttribute("fileNames", strDataType, spc);
          att.write(strDataType, fileNames.toStdString());

        }
      }
      catch (H5::Exception error) {  //??? how to improve printed msg using major/minor error codes?
        QString msg = "H5 Exception Message: " + QString::fromStdString(error.getDetailMsg());
        IException hpfError(IException::Unknown, msg, _FILEINFO_);
        msg = "H5 exception handler has detected an error when invoking the function "
              + QString::fromStdString(error.getFuncName()) + ".";
        throw IException(hpfError, IException::Unknown, msg, _FILEINFO_);
      }
    }
    catch (IException &e) {
      throw IException(e,
                       IException::Unknown,
                       "Unable to save bundle solution information to an HDF5 file.",
                       _FILEINFO_);
    }
  }

}
