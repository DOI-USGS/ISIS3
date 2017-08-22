#include "ImageListActionWorkOrder.h"

#include <QMessageBox>

#include "Color.h"
#include "ImageList.h"
#include "Project.h"

namespace Isis {
  
  /**
   * @brief Construct a work order for the given project.
   * 
   * @param project The project that the work order is for
   */
  ImageListActionWorkOrder::ImageListActionWorkOrder(Project *project) :
      WorkOrder(project) {
        
    m_isSavedToHistory = false;
  }


  /**
   * @brief Construct a work order for the given project and action.
   * 
   * @param action The action that the work order will perform
   * @param project The project that the work order is for
   */
  ImageListActionWorkOrder::ImageListActionWorkOrder(Action action, Project *project) :
      WorkOrder(project) {      
    
    m_isSavedToHistory = false;
    
    QAction::setText(toString(action));
    QUndoCommand::setText(toString(action));

    QStringList internalData;
    internalData.append(toString(action));
    setInternalData(internalData);
  }


  /**
   * @brief Copy constructor.
   * 
   * @param other The work order to be copied from
   */
  ImageListActionWorkOrder::ImageListActionWorkOrder(
      const ImageListActionWorkOrder &other) : WorkOrder(other) {
              
    m_isSavedToHistory = false; 
        
    foreach (const Image *image, *other.imageList()) {
      connect(this, SIGNAL(bringToFront()), image->displayProperties(), SIGNAL(moveToTop()));
    }
  }


  /**
   * @brief Destructor.
   */
  ImageListActionWorkOrder::~ImageListActionWorkOrder() {
  }


  /**
   * @brief Clone the current work order.
   * 
   * @return @b ImageListActionWorkOrder * A pointer to a copy of the work order
   */
  ImageListActionWorkOrder *ImageListActionWorkOrder::clone() const {
    return new ImageListActionWorkOrder(*this);
  }


  /**
   * @brief Check if the work order can run on a given image list.
   * This work order can be run on any non-empty image list.
   * 
   * @param images The image list to be checked
   * 
   * @return @b bool If the work order can run on the given ImageList.
   */
  bool ImageListActionWorkOrder::isExecutable(ImageList *images) {
    return !images->isEmpty();
  }


  /**
   * @brief Assign an image list to the work order.
   * 
   * When this work order is assigned an image list, update the undo command text.
   * 
   * @param images The image list to be assigned to the work order
   */
  void ImageListActionWorkOrder::setData(ImageList *images) {
    WorkOrder::setData(images);

    if (internalData().count()) {
      QAction::setText(qualifyString(internalData()[0], imageList()));

      QString modifiedString = (qualifyString(internalData()[0], imageList()) + " on %1 images")
          .arg(imageList()->count());
      QUndoCommand::setText(modifiedString);
    }

    foreach (Image *image, *images) {
      connect(this, SIGNAL(bringToFront()), image->displayProperties(), SIGNAL(moveToTop()));
    }
  }


  /**
   * @brief If needed, prompt the user for input and save it.  ChangeTransparency, ChangeColor,
   * and potentially ToggleShowLabel will prompt for input from the user. This was renamed from 
   * execute() to setupExecution() according to WorkOrder's redesign.
   * 
   * @return @b bool If the work order successfully set the needed the information.
   */
  bool ImageListActionWorkOrder::setupExecution() {
    bool result = WorkOrder::setupExecution() && !internalData().isEmpty();

    if (!internalData().isEmpty()) {
      QStringList state = internalData();
      QString actionString = internalData()[0];

      switch(fromActionString(actionString)) {
        case UnknownAction:
          result = false;
          break;

        case ChangeTransparency: {
          int alpha = 255;
          result = imageList()->askAlpha(&alpha);
          state.append(QString::number(alpha));
          break;
        }

        case ChangeColor: {
          QColor color;
          result = imageList()->askNewColor(&color);

          // QColor::name() doesn't preserve alpha.
          if (color.isValid()) {
            state.append(Color::toRGBAString(color));
          }
          break;
        }

        case RandomizeColor:
          break;

        case ToggleShowLabel: {
          int maxRecommendedLabels = 2000;
          if (qualifyString(actionString, imageList()).startsWith(tr("Show")) &&
              imageList()->count() > maxRecommendedLabels) {
            QMessageBox::StandardButton selectedOpt = QMessageBox::warning(NULL,
                tr("Potentially Slow Operation"),
                tr("You are asking to show the labels on %L1 images. When viewing these images in "
                   "a 2D footprint view, these images will take at least 3x longer to render. This "
                   "is a significant performance loss. Showing more than a few labels at a time is "
                   "not recommended. Are you sure you want to show the labels on these %L1 images?")
                  .arg(imageList()->count()),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

            if (selectedOpt == QMessageBox::No) {
              result = false;
            }
          }
          break;
        }

        case ToggleShowFilled:
          break;

        case ToggleShowCubeData:
          break;

        case ToggleShowOutline:
          break;
          
        case MoveToTop:
          break;
          
        case MoveUpOne:
          break;
          
        case MoveToBottom:
          break;
          
        case MoveDownOne:
          break;
          
        case ZoomFit:
          break;
      }

      setInternalData(state);
    }

    return result;
  }


  /**
   * @brief Perform the action stored in the work order and update the work order's
   * internal data with the results of the action. 
   * 
   * @see ImageList::saveAndApplyAlpha
   * @see ImageList::saveAndApplyColor
   * @see ImageList::saveAndApplyRandomColor
   * @see ImageList::saveAndToggleShowLabel
   * @see ImageList::saveAndToggleShowFill
   * @see ImageList::saveAndToggleShowDNs
   * @see ImageList::saveAndToggleShowOutline
   */
  void ImageListActionWorkOrder::execute() {
    QStringList state = internalData();
    QString actionString = internalData()[0];

    switch(fromActionString(actionString)) {
      case UnknownAction:
        break;

      case ChangeTransparency:
        state = state.mid(0, 2);
        state.append(imageList()->saveAndApplyAlpha(state[1].toInt()));
        break;

      case ChangeColor:
        state = state.mid(0, 2);
        state.append(imageList()->saveAndApplyColor(Color::fromRGBAString(state[1])));
        break;

      case RandomizeColor:
        if (state.count() > 1) {
          // Apply previously randomly generated colors if available
          imageList()->applyColors(state.mid(1), 1);
        }
        else {
          state.append(imageList()->saveAndApplyRandomColor());
        }
        break;

      case ToggleShowLabel:
        state = state.mid(0, 1);
        state.append(imageList()->saveAndToggleShowLabel());
        break;

      case ToggleShowFilled:
        state = state.mid(0, 1);
        state.append(imageList()->saveAndToggleShowFill());
        break;

      case ToggleShowCubeData:
        state = state.mid(0, 1);
        state.append(imageList()->saveAndToggleShowDNs());
        break;

      case ToggleShowOutline:
        state = state.mid(0, 1);
        state.append(imageList()->saveAndToggleShowOutline());
        break;
        
      case MoveToTop:
        break;
        
      case MoveUpOne:
        break;
        
      case MoveToBottom:
        break;
        
      case MoveDownOne:
        break;
        
      case ZoomFit:
        break;
    }

    setInternalData(state);
  }


  /**
   * @brief Undo the action stored in the work order and update the work order's
   * internal data with the results of the undo. 
   * 
   * @see ImageList::applyAlphas
   * @see ImageList::applyColors
   * @see ImageList::applyShowLabel
   * @see ImageList::applyShowFill
   * @see ImageList::applyShowDNs
   * @see ImageList::applyShowOutline
   */
  void ImageListActionWorkOrder::undoExecution() {
    QStringList state = internalData();
    QString actionString = internalData()[0];

    switch(fromActionString(actionString)) {
      case UnknownAction:
        break;

      case ChangeTransparency:
        imageList()->applyAlphas(state.mid(2));
        break;

      case ChangeColor:
        imageList()->applyColors(state.mid(2));
        break;

      case RandomizeColor:
        // Apply colors before randomization occurred
        imageList()->applyColors(state.mid(1), 0);
        break;

      case ToggleShowLabel:
        imageList()->applyShowLabel(state.mid(1));
        break;

      case ToggleShowFilled:
        imageList()->applyShowFill(state.mid(1));
        break;

      case ToggleShowCubeData:
        imageList()->applyShowDNs(state.mid(1));
        break;

      case ToggleShowOutline:
        imageList()->applyShowOutline(state.mid(1));
        break;
        
      case MoveToTop:
        break;
        
      case MoveUpOne:
        break;
        
      case MoveToBottom:
        break;
        
      case MoveDownOne:
        break;
        
      case ZoomFit:
        break;
    }

    setInternalData(state);
  }


  /**
   * @brief Determine whether a toggle action should show or hide.
   * 
   * @param unqualifiedString The action string.
   * @param imageList The image list that the action will be performed on.
   * 
   * @return @b QString The qualified action string
   */
  QString ImageListActionWorkOrder::qualifyString(QString unqualifiedString,
                                                  ImageList *imageList) {
    QString result = unqualifiedString;

    if (imageList && imageList->count()) {
      ImageDisplayProperties *firstDisplay = imageList->first()->displayProperties();
      Action act = fromActionString(unqualifiedString);

      if (act == ToggleShowLabel) {
        if (firstDisplay->getValue(ImageDisplayProperties::ShowLabel).toBool()) {
          result = tr("Hide Label");
        }
        else {
          result = tr("Show Label");
        }
      }

      if (act == ToggleShowFilled) {
        if (firstDisplay->getValue(ImageDisplayProperties::ShowFill).toBool()) {
          result = tr("Show Unfilled");
        }
        else {
          result = tr("Show Filled");
        }
      }

      if (act == ToggleShowCubeData) {
        if (firstDisplay->getValue(ImageDisplayProperties::ShowDNs).toBool()) {
          result = tr("Hide Cube Data");
        }
        else {
          result = tr("Show Cube Data");
        }
      }

      if (act == ToggleShowOutline) {
        if (firstDisplay->getValue(ImageDisplayProperties::ShowOutline).toBool()) {
          result = tr("Hide Outline");
        }
        else {
          result = tr("Show Outline");
        }
      }
    }

    return result;
  }


  /**
   * @brief Convert an action to a string.
   * 
   * @param action The action to be converted
   * 
   * @return @b QString The string converted from an action
   */
  QString ImageListActionWorkOrder::toString(Action action) {
    QString result;

    switch(action) {
      case UnknownAction:
        result = tr("???");
        break;

      case ChangeTransparency:
        result = tr("Change Transparency");
        break;

      case ChangeColor:
        result = tr("Change Color");
        break;

      case RandomizeColor:
        result = tr("Randomize Color");
        break;

      case ToggleShowLabel:
        result = tr("Toggle Label");
        break;

      case ToggleShowFilled:
        result = tr("Toggle Show Filled");
        break;

      case ToggleShowCubeData:
        result = tr("Toggle Show Cube Data");
        break;

      case ToggleShowOutline:
        result = tr("Toggle Show Outline");
        break;
        
      case MoveToTop:
        result = tr("Bring to Front");
        break;
        
      case MoveUpOne:
        result = tr("Bring Forward");
        break;
        
      case MoveToBottom:
        result = tr("Send to Back");
        break;
        
      case MoveDownOne:
        result = tr("Send Backward");
        break;
        
      case ZoomFit:
        result = tr("Zoom Fit");
        break;
    }

    return result;
  }


  /**
   * @brief Convert a string to an action.
   * 
   * @param actionString The string to be converted
   * 
   * @return @b action The action converted from the actionString
   */
  ImageListActionWorkOrder::Action ImageListActionWorkOrder::fromActionString(
      QString actionString) {
    Action result = UnknownAction;

    for (Action act = FirstAction;
         result == UnknownAction && act <= LastAction;
         act = (Action)(act + 1)) {
      if (toString(act).toUpper() == actionString.toUpper()) {
        result = act;
      }
    }

    return result;
  }
}
