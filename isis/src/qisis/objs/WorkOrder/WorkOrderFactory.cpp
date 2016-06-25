#include "WorkOrderFactory.h"

#include "CnetEditorViewWorkOrder.h"
#include "CubeViewportViewWorkOrder.h"
#include "ExportControlNetWorkOrder.h"
#include "ExportImagesWorkOrder.h"
#include "Footprint2DViewWorkOrder.h"
#include "IException.h"
#include "ImageFileListViewWorkOrder.h"
#include "ImageListActionWorkOrder.h"
#include "IString.h"
#include "ImportControlNetWorkOrder.h"
#include "ImportImagesWorkOrder.h"
#include "OpenProjectWorkOrder.h"
#include "RenameProjectWorkOrder.h"
#include "SaveProjectAsWorkOrder.h"
#include "SaveProjectWorkOrder.h"
#include "MoveDownOneSceneWorkOrder.h"
#include "MoveToBottomSceneWorkOrder.h"
#include "MoveToTopSceneWorkOrder.h"
#include "MoveUpOneSceneWorkOrder.h"

namespace Isis {
  /**
   * This instantiates a work order given a project and a type name (class name in a string).
   *
   * Ownership is passed to the caller. The work orders are QObject's so please be mindful of which
   *   thread they are in.
   *
   * @param project The project to give to the work order constructor
   * @param type The work order type (class name) - for example "Isis::ImportImagesWorkOrder"
   * 
   * @throws IException::Unknown "Could not create the work order through WorkOrderFactory"
   * 
   * @return @b WorkOrder Returns the instantiated WorkOrder
   */
  WorkOrder *WorkOrderFactory::create(Project *project, QString type) {
    WorkOrder *result = NULL;

    tryType<CubeViewportViewWorkOrder>(type, project, result);
    tryType<CnetEditorViewWorkOrder>(type, project, result);
    tryType<ExportImagesWorkOrder>(type, project, result);
    tryType<ExportControlNetWorkOrder>(type, project, result);
    tryType<ImageFileListViewWorkOrder>(type, project, result);
    tryType<ImageListActionWorkOrder>(type, project, result);
    tryType<ImportImagesWorkOrder>(type, project, result);
    tryType<ImportControlNetWorkOrder>(type, project, result);
    tryType<Footprint2DViewWorkOrder>(type, project, result);
    tryType<MoveDownOneSceneWorkOrder>(type, project, result);
    tryType<MoveToBottomSceneWorkOrder>(type, project, result);
    tryType<MoveToTopSceneWorkOrder>(type, project, result);
    tryType<MoveUpOneSceneWorkOrder>(type, project, result);
    tryType<OpenProjectWorkOrder>(type, project, result);
    tryType<RenameProjectWorkOrder>(type, project, result);
    tryType<SaveProjectWorkOrder>(type, project, result);
    tryType<SaveProjectAsWorkOrder>(type, project, result);

    if (!result) {
      throw IException(IException::Unknown,
                       QObject::tr("Could not create work order of type [%1] through "
                                   "WorkOrderFactory").arg(type),
                       _FILEINFO_);
    }

    return result;
  }
}
