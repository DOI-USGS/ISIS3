#include "ImageListActionWorkOrder.h"

#include <QMessageBox>

#include "Color.h"
#include "ImageList.h"
#include "Project.h"

namespace Isis {
  ImageListActionWorkOrder::ImageListActionWorkOrder(Project *project) :
      WorkOrder(project) {
  }


  ImageListActionWorkOrder::ImageListActionWorkOrder(Action action, Project *project) :
      WorkOrder(project) {
    QAction::setText(toString(action));
    QUndoCommand::setText(toString(action));

    QStringList internalData;
    internalData.append(toString(action));
    setInternalData(internalData);
  }


  ImageListActionWorkOrder::ImageListActionWorkOrder(
      const ImageListActionWorkOrder &other) : WorkOrder(other) {
    foreach (const Image *image, *other.imageList()) {
      connect(this, SIGNAL(bringToFront()), image->displayProperties(), SIGNAL(moveToTop()));
    }
  }


  ImageListActionWorkOrder::~ImageListActionWorkOrder() {
  }


  /**
   * Clone the current work order
   */
  ImageListActionWorkOrder *ImageListActionWorkOrder::clone() const {
    return new ImageListActionWorkOrder(*this);
  }


  /**
   * This work order can be run on any non-empty image list.
   */
  bool ImageListActionWorkOrder::isExecutable(ImageList *images) {
    return !images->isEmpty();
  }


  /**
   * When this work order is assigned an image list, update the undo command text.
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
   * Prompt the user for input, if necessary, and remember the answers.
   */
  bool ImageListActionWorkOrder::execute() {
    bool result = WorkOrder::execute() && !internalData().isEmpty();

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
      }

      setInternalData(state);
    }

    return result;
  }


  /**
   * Perform the action.
   */
  void ImageListActionWorkOrder::syncRedo() {
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
    }

    setInternalData(state);
  }


  void ImageListActionWorkOrder::syncUndo() {
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
    }

    setInternalData(state);
  }


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
    }

    return result;
  }


  ImageListActionWorkOrder::Action ImageListActionWorkOrder::fromActionString(
      QString actionString) {
    Action result = UnknownAction;

    for (Action act = UnknownAction;
         result == UnknownAction && act <= ToggleShowOutline;
         act = (Action)(act + 1)) {
      if (toString(act).toUpper() == actionString.toUpper()) {
        result = act;
      }
    }

    return result;
  }
}
