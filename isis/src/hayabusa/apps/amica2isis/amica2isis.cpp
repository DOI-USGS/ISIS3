#include "Isis.h"

#include <cstdio>
#include <QString>
#include <algorithm>

#include "Brick.h"
#include "FileName.h"
#include "ImportFits.h"
#include "iTime.h"
#include "OriginalLabel.h"
#include "PixelType.h"
#include "ProcessImportPds.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void IsisMain ()
{
  ProcessImportPds p;
  UserInterface &ui = Application::GetUserInterface();

  // Get input file and set translation processing
  FileName inFile = ui.GetFileName("FROM");
  Pvl label;
  p.SetPdsFile (inFile.expanded(), "", label);

  // Add FITS header 
  QString fitsImage = inFile.path() + "/" + (QString) label.findKeyword("^IMAGE"); 
  FileName fitsFile(fitsImage);
  ImportFits fits(fitsFile, "FitsLabel");
  label.addGroup(fits.label());

  QString instid;
  QString missid;
  try {
    instid = (QString) label.findKeyword ("INSTRUMENT_ID", PvlObject::Traverse);
    missid = (QString) label.findKeyword ("INSTRUMENT_HOST_NAME", PvlObject::Traverse); 
  }
  catch (IException &e) {
    QString msg = "Unable to read [INSTRUMENT_ID] or [INSTRUMENT_HOST_NAME] " 
                  "from input file [" + inFile.expanded() + "]";
    throw IException(e, IException::Io,msg, _FILEINFO_);
  }

  instid = instid.simplified().trimmed();
  missid = missid.simplified().trimmed();
  if (missid != "HAYABUSA" && instid != "AMICA") {
    QString msg = "Input file [" + inFile.expanded() + 
                  "] does not appear to be a " +
                  "Hayabusa/AMICA PDS label file.";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  QString target;
  if (ui.WasEntered("TARGET")) {
    target = ui.GetString("TARGET");
  }

  // Set up image translation.  Omit the inclusion of this .lbl file and we 
  // add the label with the FITS header augmentation as the orginal label
  // further below.
  p.OmitOriginalLabel();
  Cube *outcube = p.SetOutputCube ("TO");

  p.StartProcess();

  // Now flip the image lines since the first line is actually the last line.
  // Easiest way to do this is read the entire image array, flip lines and
  // rewrite the result back out.
  Brick image(outcube->sampleCount(), outcube->lineCount(), 1, Double);

  // Image extents
  int nsamps(outcube->sampleCount());
  int nlines(outcube->lineCount());
  int halfLines(nlines/2);

  //  Just in case there is more than 1 band in the image...
  image.begin();
  while (!image.end() ) {
    outcube->read(image);

    int samp0(0);                  // Index at first pixel of first line
    int samp1((nlines-1)*nsamps);  // Index at first pixel of last line
    for (int line = 0 ; line < halfLines ; line++ ) {
      for (int i = samp0, j = samp1, n = 0 ; n < nsamps ; i++, j++, n++) {
        swap(image[i], image[j]);
      }
      samp0 += nsamps;
      samp1 -= nsamps;
    }
    
    outcube->write(image);  
    image.next();
  }

  // Get the directory where the Hayabusa translation tables are.
  PvlGroup dataDir (Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["Hayabusa"] + "/translations/";

  // Create a PVL to store the translated labels in
  Pvl outLabel;

  // Translate the BandBin group
  FileName transFile (transDir + "amicaBandBin.trn");
  PvlTranslationManager bandBinXlater (label, transFile.expanded());
  bandBinXlater.Auto(outLabel);

  // Translate the Archive group
  transFile = transDir + "amicaArchive.trn";
  PvlTranslationManager archiveXlater (label, transFile.expanded());
  archiveXlater.Auto(outLabel);

  // Translate the Instrument group
  transFile = transDir + "amicaInstrument.trn";
  PvlTranslationManager instrumentXlater (label, transFile.expanded());
  instrumentXlater.Auto(outLabel);

  //  Create YearDoy keyword in Archive group
  iTime stime(outLabel.findGroup("Instrument", Pvl::Traverse)["StartTime"][0]);
  PvlKeyword yeardoy("YearDoy", toString(stime.Year()*1000 + stime.DayOfYear()));
  (void) outLabel.findGroup("Archive", Pvl::Traverse).addKeyword(yeardoy);


  //  Update target if user specifies it
  if (!target.isEmpty()) {
    PvlGroup &igrp = outLabel.findGroup("Instrument",Pvl::Traverse);
    igrp["TargetName"] = target;
  }

  // Write the BandBin, Archive, and Instrument groups
  // to the output cube label
  outcube->putGroup(outLabel.findGroup("BandBin",Pvl::Traverse));
  outcube->putGroup(outLabel.findGroup("Archive",Pvl::Traverse));
  outcube->putGroup(outLabel.findGroup("Instrument",Pvl::Traverse));

  // Use the HAYABUSA_AMICA frame code rather than HAYABUSA_AMICA_IDEAL
  PvlGroup kerns("Kernels");
  kerns += PvlKeyword("NaifFrameCode","-130102");
  outcube->putGroup(kerns);

  // Now write the FITS augmented label as the original label
  OriginalLabel oldLabel(label);
  outcube->write(oldLabel);

  //  All done...
   p.EndProcess();
}

