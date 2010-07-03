#include "Isis.h"
#include "PvlGroup.h"
#include "UserInterface.h"
#include "Cube.h"
#include "Chip.h"
#include "Progress.h"
#include "iException.h"
#include "AutoReg.h"
#include "AutoRegFactory.h"
#include "Brick.h"

using namespace std; 
using namespace Isis;

void IsisMain ()
{
    // Import cube data & PVL information
    Cube cube;
    UserInterface &ui = Application::GetUserInterface();
    cube.Open(ui.GetFilename("FROM"),"rw");
    Pvl* regdef;
    // If regdef was supplied by the user, use it. else, use the template.
    if (ui.WasEntered("REGDEF")) {
        regdef = new Pvl(ui.GetFilename("REGDEF"));    
    }
    else {
        regdef = new Pvl("$base/templates/autoreg/findrx.def");
    }
    PvlGroup &reseaus = cube.Label()->FindGroup("Reseaus",Pvl::Traverse);

    // If the Keyword sizes don't match up, throw errors.
    int nres = reseaus["Line"].Size();
    if (nres != reseaus["Sample"].Size()) {
      string msg = "Sample size incorrect [Sample size " + 
                    iString(reseaus["Sample"].Size()) + " != " + " Line size " + 
                    iString(reseaus["Line"].Size()) + "]";
      throw Isis::iException::Message(Isis::iException::Pvl,msg, _FILEINFO_);
    }
    if (nres != reseaus["Type"].Size()) {
      string msg = "Type size incorrect [Type size " + 
                    iString(reseaus["Type"].Size()) + " != " + " Line size " + 
                    iString(reseaus["Line"].Size()) + "]";
      throw Isis::iException::Message(Isis::iException::Pvl,msg, _FILEINFO_);
    }
    if (nres != reseaus["Valid"].Size()) {
      string msg = "Valid size incorrect [Valid size " + 
                    iString(reseaus["Valid"].Size()) + " != " + " Line size " + 
                    iString(reseaus["Line"].Size()) + "]";
      throw Isis::iException::Message(Isis::iException::Pvl,msg, _FILEINFO_);
    }

    // Auto Registration
    AutoReg *ar = AutoRegFactory::Create(*regdef);
    Cube pattern;
    pattern.Open(reseaus["Template"], "r");
    ar->PatternChip()->TackCube(5.0,5.0);
    
    // Display the progress...10% 20% etc.
    Progress prog;
    prog.SetMaximumSteps(nres);
    prog.CheckStatus();
    

    //If the mark reseaus option is set...then create a brick.
    Brick* white = NULL;
    if (ui.GetBoolean("MARK") == true) {
        white = new Brick(1,1,1, Isis::UnsignedByte);
        (*white)[0] = Isis::Hrs;
    }

    double percent = ar->PatternValidPercent();

    // And the loop...
    for (int res=0; res<nres; ++res) 
    {
        // Output chips
        ar->SearchChip()->TackCube(reseaus["Sample"][res], reseaus["Line"][res]);
        ar->SearchChip()->Load(cube);
        ar->PatternChip()->Load(pattern, 0, 1.0, res+1);
        int type = iString(reseaus["Type"][res]);
        // If the reseaus is in the center (type 5) use full percent value
        if (type== 5) ar->SetPatternValidPercent(percent);
        // else if the reseaus is on an edge (type 2,4,6, or 8) use half percent value
        else if (type%2 == 0) ar->SetPatternValidPercent(percent/2.0);
        // else the reseaus on a corner (type 1,3,7, or 9) use a quarter percent value
        else ar->SetPatternValidPercent(percent/4.0);
        if (ar->Register()==AutoReg::Success) {
            reseaus["Sample"][res] = ar->CubeSample();
            reseaus["Line"][res] = ar->CubeLine();
            reseaus["Valid"][res] = 1;
        }                                                                             
        else {
            reseaus["Valid"][res] = 0;
        }

        // And if the reseaus are to be marked...mark em
        if (white != NULL) {
            double line = reseaus["Line"][res];
            double sample = reseaus["Sample"][res];
            white->SetBasePosition(int(sample), int(line), 1);
            cube.Write(*white);
        } 
        prog.CheckStatus();

    }

    // Change status to "Refined", corrected!
    reseaus["Status"] = "Refined";
     
    pattern.Close();
    cube.Close();
}
