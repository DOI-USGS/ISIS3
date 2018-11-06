#include "Cube.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "LineManager.h"
#include "FileName.h"
#include "IException.h"
#include "Projection.h"
#include "AlphaCube.h"
#include "Table.h"
#include "SubArea.h"

namespace Isis {
  extern void crop(QString from, QString to, int line, int sample, int sinc, int linc, int nsamples=-1, int nlines=-1, bool propSpice=true);
}
