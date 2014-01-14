/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
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
#include "ImageList.h"

#include <QAction>
#include <QColorDialog>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFuture>
#include <QInputDialog>
#include <QLabel>
#include <QProgressDialog>
#include <QtConcurrentMap>
#include <QXmlStreamWriter>

#include "Color.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Project.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {
  /**
   * Create an image list from an image list name and path (does not read Images).
   *
   * @param name The ImageList's name (i.e. import1, import2, ...)
   * @param path The ImageList's folder name (i.e. import1, import2, ...)
   * @param parent The Qt-relationship parent
   */
  ImageList::ImageList(QString name, QString path, QObject *parent) : QObject(parent) {
    m_name = name;
    m_path = path;
  }


  /**
   * Create a blank image list.
   *
   * @param parent The Qt-relationship parent
   */
  ImageList::ImageList(QObject *parent) : QObject(parent) {
  }


  /**
   * Create an image list from a list of images
   *
   * @param images The list of images
   * @param parent The Qt-relationship parent
   */
  ImageList::ImageList(QList<Image *> images, QObject *parent) : QObject(parent) {
    append(images);
  }


  /**
   * Create an image list from XML
   *
   * @param project The project with the image list
   * @param xmlReader The XML reader currently at an <imageList /> tag.
   * @param parent The Qt-relationship parent
   */
  ImageList::ImageList(Project *project, XmlStackedHandlerReader *xmlReader, QObject *parent) :
      QObject(parent) {
    xmlReader->pushContentHandler(new XmlHandler(this, project));
  }


  /**
   * Copy constructor.
   *
   * @param other The ImageList to copy
   */
  ImageList::ImageList(const ImageList &other) :
      QList<Image *>(other) {
    m_name = other.m_name;
    m_path = other.m_path;
  }


  /**
   * Create an image list from a list of cube file names. This is slow (serial) and not recommended.
   */
  ImageList::ImageList(QStringList &fileNames) {
    foreach (QString fileName, fileNames) {
      try {
        Image *image = new Image(fileName);
        append(image);
      }
      catch (IException &) {
      }
    }
  }


  /**
   * Destructor. This does not free the Images from memory.
   */
  ImageList::~ImageList() {
  }


  /**
   * @see QList<Image *>::append()
   */
  void ImageList::append(Image * const & value) {
    QList<Image *>::append(value);
    emit countChanged(count());
  }


  /**
   * @see QList<Image *>::append()
   */
  void ImageList::append(const QList<Image *> &value) {
    QList<Image *>::append(value);
    emit countChanged(count());
  }


  /**
   * @see QList<Image *>::clear()
   */
  void ImageList::clear() {
    bool countChanging = count();
    QList<Image *>::clear();
    if (countChanging) {
      emit countChanged(count());
    }
  }


  /**
   * @see QList<Image *>::erase()
   */
  QList<Image *>::iterator ImageList::erase(iterator pos) {
    iterator result = QList<Image *>::erase(pos);
    emit countChanged(count());
    return result;
  }


  /**
   * @see QList<Image *>::erase()
   */
  QList<Image *>::iterator ImageList::erase(iterator begin, iterator end) {
    iterator result = QList<Image *>::erase(begin, end);
    emit countChanged(count());
    return result;
  }


  /**
   * @see QList<Image *>::insert()
   */
  void ImageList::insert(int i, Image * const & value) {
    QList<Image *>::insert(i, value);

    emit countChanged(count());
  }


  /**
   * @see QList<Image *>::insert()
   */
  QList<Image *>::iterator ImageList::insert(iterator before, Image * const & value) {
    iterator result = QList<Image *>::insert(before, value);
    emit countChanged(count());
    return result;
  }


  /**
   * @see QList<Image *>::prepend()
   */
  void ImageList::prepend(Image * const & value) {
    QList<Image *>::prepend(value);
    emit countChanged(count());
  }


  /**
   * @see QList<Image *>::push_back()
   */
  void ImageList::push_back(Image * const & value) {
    QList<Image *>::push_back(value);
    emit countChanged(count());
  }


  /**
   * @see QList<Image *>::push_front()
   */
  void ImageList::push_front(Image * const & value) {
    QList<Image *>::push_front(value);
    emit countChanged(count());
  }


  /**
   * @see QList<Image *>::removeAll()
   */
  int ImageList::removeAll(Image * const & value) {
    int result = QList<Image *>::removeAll(value);

    if (result != 0) {
      emit countChanged(count());
    }

    return result;
  }


  /**
   * @see QList<Image *>::removeAt()
   */
  void ImageList::removeAt(int i) {
    QList<Image *>::removeAt(i);
    emit countChanged(count());
  }


  /**
   * @see QList<Image *>::removeFirst()
   */
  void ImageList::removeFirst() {
    QList<Image *>::removeFirst();
    emit countChanged(count());
  }


  /**
   * @see QList<Image *>::removeLast()
   */
  void ImageList::removeLast() {
    QList<Image *>::removeLast();
    emit countChanged(count());
  }


  /**
   * @see QList<Image *>::removeOne()
   */
  bool ImageList::removeOne(Image * const & value) {
    bool result = QList<Image *>::removeOne(value);

    if (result) {
      emit countChanged(count());
    }

    return result;
  }


  /**
   * @see QList<Image *>::swap()
   */
  void ImageList::swap(QList<Image *> &other) {
    QList<Image *>::swap(other);

    if (count() != other.count()) {
      emit countChanged(count());
    }
  }


  /**
   * @see QList<Image *>::takeAt()
   */
  Image *ImageList::takeAt(int i) {
    Image * result = QList<Image *>::takeAt(i);
    emit countChanged(count());
    return result;
  }


  /**
   * @see QList<Image *>::takeFirst()
   */
  Image *ImageList::takeFirst() {
    Image *result = QList<Image *>::takeFirst();
    emit countChanged(count());
    return result;
  }


  /**
   * @see QList<Image *>::takeLast()
   */
  Image *ImageList::takeLast() {
    Image *result = QList<Image *>::takeLast();
    emit countChanged(count());
    return result;
  }


  /**
   * @see QList<Image *>::operator+=()
   */
  ImageList &ImageList::operator+=(const QList<Image *> &other) {
    QList<Image *>::operator+=(other);

    if (other.count()) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * @see QList<Image *>::operator+=()
   */
  ImageList &ImageList::operator+=(Image * const &other) {
    QList<Image *>::operator+=(other);
    emit countChanged(count());
    return *this;
  }


  /**
   * @see QList<Image *>::operator<<()
   */
  ImageList &ImageList::operator<<(const QList<Image *> &other) {
    QList<Image *>::operator<<(other);

    if (other.count()) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * @see QList<Image *>::operator<<()
   */
  ImageList &ImageList::operator<<(Image * const &other) {
    QList<Image *>::operator<<(other);
    emit countChanged(count());
    return *this;
  }


  /**
   * @see QList<Image *>::operator=()
   */
  ImageList &ImageList::operator=(const QList<Image *> &rhs) {
    bool countChanging = (rhs.count() != count());
    QList<Image *>::operator=(rhs);

    if (countChanging) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * Assignment operator
   *
   * @param rhs The right hand side of the '=' operator
   * @return *this
   */
  ImageList &ImageList::operator=(const ImageList &rhs) {
    bool countChanging = (rhs.count() != count());
    QList<Image *>::operator=(rhs);

    m_name = rhs.m_name;
    m_path = rhs.m_path;

    if (countChanging) {
      emit countChanged(count());
    }

    return *this;
  }


  /**
   * Gets a list of pre-connected actions that have to do with display,
   *   such as color, alpha, outline, fill, etc.
   */
  QList<QAction *> ImageList::supportedActions(Project *project) {
    QList<QAction *> actions;

    // It turns out connect() statements cannot be templated, hence they aren't inside of
    //   createWorkOrder().
    if (allSupport(ImageDisplayProperties::Color)) {
      QAction *alphaAction = createWorkOrder(project, ImageListActionWorkOrder::ChangeTransparency);
      if (!project) {
        connect(alphaAction, SIGNAL(triggered()),
                this, SLOT(askAndUpdateAlpha()));
      }
      actions.append(alphaAction);

      QAction *colorAction = createWorkOrder(project, ImageListActionWorkOrder::ChangeColor);
      if (!project) {
        connect(colorAction, SIGNAL(triggered()),
                this, SLOT(askAndUpdateColor()));
      }
      actions.append(colorAction);


      QAction *ranColorAction = createWorkOrder(project, ImageListActionWorkOrder::RandomizeColor);
      if (!project) {
        connect(ranColorAction, SIGNAL(triggered()),
                this, SLOT(showRandomColor()));
      }
      actions.append(ranColorAction);
    }


    if (allSupport(ImageDisplayProperties::ShowLabel)) {
      QAction *labelVisibleAction = createWorkOrder(project,
                                                    ImageListActionWorkOrder::ToggleShowLabel);
      if (!project) {
        connect(labelVisibleAction, SIGNAL(triggered()),
                this, SLOT(saveAndToggleShowLabel()));
      }
      actions.append(labelVisibleAction);
    }


    if (allSupport(ImageDisplayProperties::ShowFill)) {
      QAction *fillAction = createWorkOrder(project, ImageListActionWorkOrder::ToggleShowFilled);
      if (!project) {
        connect(fillAction, SIGNAL(triggered()),
                this, SLOT(saveAndToggleShowFill()));
      }
      actions.append(fillAction);
    }


    if (allSupport(ImageDisplayProperties::ShowDNs)) {
      QAction *cubeDataAction = createWorkOrder(project,
                                                ImageListActionWorkOrder::ToggleShowCubeData);
      if (!project) {
        connect(cubeDataAction, SIGNAL(triggered()),
                this, SLOT(saveAndToggleShowDNs()));
      }
      actions.append(cubeDataAction);
    }


    if (allSupport(ImageDisplayProperties::ShowOutline)) {
      QAction *outlineAction = createWorkOrder(project,
                                               ImageListActionWorkOrder::ToggleShowOutline);
      if (!project) {
        connect(outlineAction, SIGNAL(triggered()),
                this, SLOT(saveAndToggleShowOutline()));
      }
      actions.append(outlineAction);
    }

    actions.append(NULL);

    if (!project) {
      if (allSupport(ImageDisplayProperties::ZOrdering)) {
        QAction *moveToTopAct = new QAction(tr("Bring to Front"), this);
        QAction *moveUpAct = new QAction(tr("Bring Forward"), this);
        QAction *moveToBottomAct = new QAction(tr("Send to Back"), this);
        QAction *moveDownAct = new QAction(tr("Send Backward"), this);

        foreach (Image *image, *this) {
          connect(moveToTopAct, SIGNAL(triggered()),
                  image->displayProperties(), SIGNAL(moveToTop()));

          connect(moveUpAct, SIGNAL(triggered()),
                  image->displayProperties(), SIGNAL(moveUpOne()));

          connect(moveToBottomAct, SIGNAL(triggered()),
                  image->displayProperties(), SIGNAL(moveToBottom()));

          connect(moveDownAct, SIGNAL(triggered()),
                  image->displayProperties(), SIGNAL(moveDownOne()));
        }
        actions.append(moveToTopAct);
        actions.append(moveUpAct);
        actions.append(moveToBottomAct);
        actions.append(moveDownAct);
      }

      actions.append(NULL);

      if (size() == 1 && allSupport(ImageDisplayProperties::Zooming)) {
        QAction *zoomFit = new QAction(tr("Zoom Fit"), this);
        connect(zoomFit, SIGNAL(triggered()),
                first()->displayProperties(), SIGNAL(zoomFit()));
        actions.append(zoomFit);
      }
    }

    return actions;
  }


  /**
   * Returns true if all of the given displays support the property
   *
   * @param prop The property we're testing for support for
   * @param displays The displays we're doing the test on
   */
  bool ImageList::allSupport(ImageDisplayProperties::Property prop) {
    if (isEmpty())
      return false;

    foreach (Image *image, *this) {
      if (!image->displayProperties()->supports(prop))
        return false;
    }

    return true;
  }


  /**
   * Set the human-readable name of this image list. This is really only useful for project
   *   image lists (not anonymous temporary ones).
   *
   * @param newName The name to give this image list
   */
  void ImageList::setName(QString newName) {
    m_name = newName;
  }


  /**
   * Set the relative path (from the project root) to this image list's folder. This is really only
   *   useful for project image lists (not anonymous temporary ones).
   *
   * @param newPath The path to the images in this image list
   */
  void ImageList::setPath(QString newPath) {
    m_path = newPath;
  }


  /**
   * Get the human-readable name of this image list
   *
   * @return The name of the image list (or an empty string if anonymous).
   */
  QString ImageList::name() const {
    return m_name;
  }


  /**
   * Get the path to these images in the image list (relative to project root). This only applies to
   *   an image list from the project.
   *
   * @return The path to the images in the image list (or an empty string if unknown).
   */
  QString ImageList::path() const {
    return m_path;
  }


  /**
   * Delete all of the contained Images from disk (see Image::deleteFromDisk())
   */
  void ImageList::deleteFromDisk(Project *project) {
    foreach (Image *image, *this) {
      image->deleteFromDisk();
    }

    if (!m_path.isEmpty()) {
      QFile::remove(project->imageDataRoot() + "/" + m_path + "/images.xml");

      QDir dir;
      dir.rmdir(project->imageDataRoot() + "/" + m_path);
    }
  }


  /**
   * Convert this image list into XML format for saving/restoring capabilities.
   *
   * This writes:
   * <pre>
   *   <imageList name="..." path="..."/>
   * </pre>
   * to the given xml stream, and creates an 'images.xml' inside the folder with the images.
   * Inside the images.xml, this writes:
   *
   * <pre>
   *   <images>
   *     ...
   *   </images>
   * </pre>
   */
  void ImageList::save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot)
      const {
    stream.writeStartElement("imageList");
    stream.writeAttribute("name", m_name);
    stream.writeAttribute("path", m_path);

    FileName settingsFileName(
        Project::imageDataRoot(newProjectRoot.toString()) + "/" + m_path + "/images.xml");

    if (!settingsFileName.dir().mkpath(settingsFileName.path())) {
      throw IException(IException::Io,
                       QString("Failed to create directory [%1]")
                         .arg(settingsFileName.path()),
                       _FILEINFO_);
    }

    QFile imageListContentsFile(settingsFileName.toString());

    if (!imageListContentsFile.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
      throw IException(IException::Io,
          QString("Unable to save image information for [%1] because [%2] could not be opened for "
                  "writing")
            .arg(m_name).arg(settingsFileName.original()),
          _FILEINFO_);
    }

    QXmlStreamWriter imageDetailsWriter(&imageListContentsFile);
    imageDetailsWriter.setAutoFormatting(true);
    imageDetailsWriter.writeStartDocument();

    int countWidth = QString("%1L").arg(count()).size() - 1;
    QChar paddingChar('0');

    QLabel *progressLabel = new QLabel;

    QProgressDialog progressDialog;
    progressDialog.setLabel(progressLabel);
    progressDialog.setRange(-1, count());
    progressDialog.setValue(-1);

    imageDetailsWriter.writeStartElement("images");
    // Mapped is way faster than hundreds/thousands of run() calls... so use mapped for performance
    QFuture<void *> future = QtConcurrent::mapped(*this,
                                                  CopyImageDataFunctor(project, newProjectRoot));

    for (int i = 0; i < count(); i++) {
      int newProgressValue = progressDialog.value() + 1;
      progressLabel->setText(
          tr("Saving Image Information for [%1] - %L2/%L3 done")
            .arg(m_name)
            .arg(newProgressValue, countWidth, 10, paddingChar)
            .arg(count()));
      progressDialog.setValue(newProgressValue);
      future.resultAt(i);
    }

    progressLabel->setText(tr("Finalizing..."));
    progressDialog.setRange(0, 0);
    progressDialog.setValue(0);

    foreach (Image *image, *this) {
      image->save(imageDetailsWriter, project, newProjectRoot);
    }

    imageDetailsWriter.writeEndElement();

    imageDetailsWriter.writeEndDocument();

    stream.writeEndElement();
  }


  ImageList::CopyImageDataFunctor::CopyImageDataFunctor(const Project *project,
                                                        FileName newProjectRoot) {
    m_project = project;
    m_newProjectRoot = newProjectRoot;
  }


  ImageList::CopyImageDataFunctor::CopyImageDataFunctor(const CopyImageDataFunctor &other) {
    m_project = other.m_project;
    m_newProjectRoot = other.m_newProjectRoot;
  }


  ImageList::CopyImageDataFunctor::~CopyImageDataFunctor() {
  }


  void *ImageList::CopyImageDataFunctor::operator()(Image * const &imageToCopy) {
    imageToCopy->copyToNewProjectRoot(m_project, m_newProjectRoot);
    return NULL;
  }


  ImageList::CopyImageDataFunctor &ImageList::CopyImageDataFunctor::operator=(
      const CopyImageDataFunctor &rhs) {
    m_project = rhs.m_project;
    m_newProjectRoot = rhs.m_newProjectRoot;
    return *this;
  }


  void ImageList::applyAlphas(QStringList alphaValues) {
    if (count() == alphaValues.count()) {
      for (int i = 0; i < count(); i++) {
        ImageDisplayProperties *dispProps = (*this)[i]->displayProperties();
        QColor color = dispProps->getValue(ImageDisplayProperties::Color).value<QColor>();
        color.setAlpha(alphaValues[i].toInt());
        dispProps->setColor(color);
      }
    }
  }


  void ImageList::applyColors(QStringList colorValues, int column) {
    if (count() == colorValues.count()) {
      for (int i = 0; i < count(); i++) {
        QString colorData = colorValues[i].split(" ")[column];
        (*this)[i]->displayProperties()->setColor(Color::fromRGBAString(colorData));
      }
    }
  }


  void ImageList::applyShowLabel(QStringList showLabelValues) {
    if (count() == showLabelValues.count()) {
      for (int i = 0; i < count(); i++) {
        ImageDisplayProperties *dispProps = (*this)[i]->displayProperties();
        dispProps->setShowLabel(showLabelValues[i] == "shown");
      }
    }
  }


  void ImageList::applyShowFill(QStringList showFillValues) {
    if (count() == showFillValues.count()) {
      for (int i = 0; i < count(); i++) {
        ImageDisplayProperties *dispProps = (*this)[i]->displayProperties();
        dispProps->setShowFill(showFillValues[i] == "shown");
      }
    }
  }


  void ImageList::applyShowDNs(QStringList showDNsValues) {
    if (count() == showDNsValues.count()) {
      for (int i = 0; i < count(); i++) {
        ImageDisplayProperties *dispProps = (*this)[i]->displayProperties();
        dispProps->setShowDNs(showDNsValues[i] == "shown");
      }
    }
  }


  void ImageList::applyShowOutline(QStringList showOutlineValues) {
    if (count() == showOutlineValues.count()) {
      for (int i = 0; i < count(); i++) {
        ImageDisplayProperties *dispProps = (*this)[i]->displayProperties();
        dispProps->setShowOutline(showOutlineValues[i] == "shown");
      }
    }
  }


  /**
   * Prompt the user for a new alpha value. If the user selects
   *   a new alpha then this sets alphaResult and returns true.
   */
  bool ImageList::askAlpha(int *alphaResult) const {
    bool result = false;

    if (!isEmpty()) {
      ImageDisplayProperties *dispProps = first()->displayProperties();
      *alphaResult = QInputDialog::getInt(NULL, "Transparency Value",
          "Set the cube's transparency\nValues are 0 (invisible) to 255 (solid)",
          dispProps->getValue(ImageDisplayProperties::Color).value<QColor>().alpha(),
          0, 255, 1, &result);
    }

    return result;
  }


  bool ImageList::askNewColor(QColor *colorResult) const {
    *colorResult = QColor();

    if (!isEmpty()) {
      ImageDisplayProperties *dispProps = first()->displayProperties();
      *colorResult = QColorDialog::getColor(
          dispProps->getValue(ImageDisplayProperties::Color).value<QColor>(), NULL,
          "Cube Display Color",
          QColorDialog::ShowAlphaChannel);
    }

    return colorResult->isValid();
  }


  QStringList ImageList::saveAndApplyAlpha(int newAlpha) {
    QStringList results;
    foreach (Image *image, *this) {
      ImageDisplayProperties *displayProperties = image->displayProperties();

      QColor displayColor = displayProperties->getValue(
          ImageDisplayProperties::Color).value<QColor>();

      results.append(QString::number(displayColor.alpha()));

      displayColor.setAlpha(newAlpha);
      displayProperties->setColor(displayColor);
    }

    return results;
  }


  QStringList ImageList::saveAndApplyColor(QColor newColor) {
    QStringList results;

    if (newColor.isValid()) {
      foreach (Image *image, *this) {
        ImageDisplayProperties *displayProperties = image->displayProperties();

        QColor displayColor = displayProperties->getValue(
            ImageDisplayProperties::Color).value<QColor>();

        results.append(Color::toRGBAString(displayColor));

        displayProperties->setColor(newColor);
      }
    }

    return results;
  }


  QStringList ImageList::saveAndApplyRandomColor() {
    QStringList results;

    foreach (Image *image, *this) {
      QColor ranColor = ImageDisplayProperties::randomColor();

      ImageDisplayProperties *displayProperties = image->displayProperties();

      QColor displayColor = displayProperties->getValue(
          ImageDisplayProperties::Color).value<QColor>();

      // Preserve alpha
      ranColor.setAlpha(displayColor.alpha());

      // QColor::name() doesn't preserve alpha.
      results.append(
          QString("%1 %2").arg(Color::toRGBAString(displayColor))
                          .arg(Color::toRGBAString(ranColor)));

      displayProperties->setColor(ranColor);
    }

    return results;
  }


  /**
   * Prompt the user for a new alpha value. If the user selects
   *   a new alpha then every image's display properties is
   *   updated.
   */
  void ImageList::askAndUpdateAlpha() {
    int newAlpha = 255;

    if (askAlpha(&newAlpha)) {
      saveAndApplyAlpha(newAlpha);
    }
  }


  /**
   * Prompt the user for a new color. If the user selects
   *   a new color then every image's display properties is
   *   updated.
   */
  void ImageList::askAndUpdateColor() {
    QColor newColor;
    askNewColor(&newColor);
    saveAndApplyColor(newColor);
  }


  /**
   * This applies a new semi-random color to every image's display property for every image in this
   *   image list.
   */
  void ImageList::showRandomColor() {
    foreach (Image *image, *this) {
      QColor ranColor = ImageDisplayProperties::randomColor();
      image->displayProperties()->setColor(ranColor);
    }
  }


  /**
   * Change the visibility of DNs. This synchronizes all
   *   of the values where at least one is guaranteed to be toggled.
   */
  QStringList ImageList::saveAndToggleShowDNs() {
    QStringList results;

    if (!isEmpty()) {
      ImageDisplayProperties *firstDisplayProperties = first()->displayProperties();
      bool newValue = !firstDisplayProperties->getValue(ImageDisplayProperties::ShowDNs).toBool();

      foreach (Image *image, *this) {
        ImageDisplayProperties *displayProps = image->displayProperties();

        bool value = displayProps->getValue(ImageDisplayProperties::ShowDNs).toBool();
        results.append(value? "shown" : "hidden");

        image->displayProperties()->setShowDNs(newValue);
      }
    }

    return results;
  }


  /**
   * Change the visibility of the fill area. This synchronizes all
   *   of the values where at least one is guaranteed to be toggled.
   */
  QStringList ImageList::saveAndToggleShowFill() {
    QStringList results;

    if (!isEmpty()) {
      ImageDisplayProperties *firstDisplayProps = first()->displayProperties();
      bool newValue = !firstDisplayProps->getValue(ImageDisplayProperties::ShowFill).toBool();

      foreach (Image *image, *this) {
        ImageDisplayProperties *displayProps = image->displayProperties();

        bool value = displayProps->getValue(ImageDisplayProperties::ShowFill).toBool();
        results.append(value? "shown" : "hidden");

        image->displayProperties()->setShowFill(newValue);
      }
    }

    return results;
  }


  /**
   * Change the visibility of the display name. This synchronizes all
   *   of the values where at least one is guaranteed to be toggled.
   */
  QStringList ImageList::saveAndToggleShowLabel() {
    QStringList results;

    if (!isEmpty()) {
      ImageDisplayProperties *firstDisplayProps = first()->displayProperties();
      bool newValue = !firstDisplayProps->getValue(ImageDisplayProperties::ShowLabel).toBool();

      foreach (Image *image, *this) {
        ImageDisplayProperties *displayProps = image->displayProperties();

        bool value = displayProps->getValue(ImageDisplayProperties::ShowLabel).toBool();
        results.append(value? "shown" : "hidden");

        image->displayProperties()->setShowLabel(newValue);
      }
    }

    return results;
  }


  /**
   * Change the visibility of the outline. This synchronizes all
   *   of the values where at least one is guaranteed to be toggled.
   */
  QStringList ImageList::saveAndToggleShowOutline() {
    QStringList results;

    if (!isEmpty()) {
      ImageDisplayProperties *firstDisplayProps = first()->displayProperties();
      bool newValue = !firstDisplayProps->getValue(ImageDisplayProperties::ShowOutline).toBool();

      foreach (Image *image, *this) {
        ImageDisplayProperties *displayProps = image->displayProperties();

        bool value = displayProps->getValue(ImageDisplayProperties::ShowOutline).toBool();
        results.append(value? "shown" : "hidden");

        image->displayProperties()->setShowOutline(newValue);
      }
    }

    return results;
  }


  /**
   * Create an XML Handler (reader) that can populate the ImageList class data. See
   *   ImageList::save() for the expected format.
   *
   * @param imageList The image list we're going to be initializing
   * @param project The project that contains the image list
   */
  ImageList::XmlHandler::XmlHandler(ImageList *imageList, Project *project) {
    m_imageList = imageList;
    m_project = project;
  }


  /**
   * Handle an XML start element. This expects <imageList/> and <image/> elements (it reads both
   *   the project XML and the images.xml file).
   *
   * @return If we should continue reading the XML (usually true).
   */
  bool ImageList::XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
                                           const QString &qName, const QXmlAttributes &atts) {
    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {
      if (localName == "imageList") {
        QString name = atts.value("name");
        QString path = atts.value("path");

        if (!name.isEmpty()) {
          m_imageList->setName(name);
        }

        if (!path.isEmpty()) {
          m_imageList->setPath(path);
        }
      }
      else if (localName == "image") {
        m_imageList->append(new Image(m_project->imageDataRoot() + "/" + m_imageList->path(),
                                      reader()));
      }
    }

    return true;
  }


  /**
   * Handle an XML end element. This handles <imageList /> by opening and reading the images.xml
   *   file.
   *
   * @return If we should continue reading the XML (usually true).
   */
  bool ImageList::XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
                                         const QString &qName) {
    if (localName == "imageList") {
      XmlHandler handler(m_imageList, m_project);

      XmlStackedHandlerReader reader;
      reader.pushContentHandler(&handler);
      reader.setErrorHandler(&handler);

      QString imageListXmlPath = m_project->imageDataRoot() + "/" + m_imageList->path() +
                                 "/images.xml";
      QFile file(imageListXmlPath);

      if (!file.open(QFile::ReadOnly)) {
        throw IException(IException::Io,
                         QString("Unable to open [%1] with read access")
                           .arg(imageListXmlPath),
                         _FILEINFO_);
      }

      QXmlInputSource xmlInputSource(&file);
      if (!reader.parse(xmlInputSource))
        throw IException(IException::Io,
                         tr("Failed to open image list XML [%1]").arg(imageListXmlPath),
                         _FILEINFO_);
    }

    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }
}
