/*
$Id: hifringe.cpp,v 1.6 2008/05/14 21:07:24 slambright Exp $

Copyright (C) 2005, 2006  Arizona Board of Regents on behalf of the
Planetary Image Research Laboratory, Lunar and Planetary Laboratory at
the University of Arizona.

This library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License, version 2.1,
as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
*/

#include "Isis.h"

#include <string>

#include "Process.h"
#include "Statistics.h"
#include "LineManager.h"
#include "Filename.h"
#include "iString.h"

using namespace std; 
using namespace Isis;


void pvlOut(Statistics stats1, Statistics stats2, string name, int start,
            int end, PvlObject * one, PvlObject * two);

static const char* const
	ID = "PIRL::hifringe ($Revision: 1.6 $ $Date: 2008/05/14 21:07:24 $)";

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Isis::Filename fromFile = ui.GetFilename("FROM");
  
  Isis::Cube inputCube;
  inputCube.Open(fromFile.Expanded());
  
  //Check to make sure we got the cube properly
  if (!inputCube.IsOpen()){
    string msg = "Could not open FROM cube" + fromFile.Expanded();
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }
  Process p;  
  Cube *icube = p.SetInputCube("FROM");
  
  int totalSamples = icube->Samples();
  int totalLines   = icube->Lines();
  
  Isis::LineManager lineManager(inputCube);
  lineManager.begin();
  
  int leftFringe, rightFringe;
  int binningMode = icube->GetGroup("Instrument")["Summing"];

  //determine the edges between which no statistics should be gathered
  leftFringe = 48 / binningMode;
  rightFringe = totalSamples - leftFringe;
  
  
  int numSections = ui.GetInteger("SECTIONS");
  if (numSections > 9){
    string msg = "You may have no more than 9 sections per side";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  } 

  int sectionLength = 0;
  if (!ui.WasEntered("LINESIZE")){ //User didn't enter number of lines
    if (numSections == 0 ){
      sectionLength = 0;
	 }
	 else{
      sectionLength = totalLines / numSections;
    }
  }
  else{
    sectionLength = ui.GetInteger("LINESIZE");
    if ((sectionLength * numSections > totalLines) || sectionLength < 1){
      sectionLength = totalLines / numSections;
    }
  }
  
  Statistics sections[numSections][2];
  Statistics leftTotal, rightTotal;
  int sectionStarts[numSections];
  sectionStarts[0] = 0;
  for (int i = 1 ; i < numSections - 1 ; i ++){
    sectionStarts[i] = (totalLines / numSections) * i;
  }
  if (numSections > 0){
    sectionStarts[numSections -1] = totalLines - sectionLength;
  }
  
  int currentSection = 0;
  Buffer leftFringeBuf(leftFringe, 1, 1, lineManager.PixelType()); 
  Buffer rightFringeBuf(leftFringe, 1, 1, lineManager.PixelType());
  
  //Walk down the cube
  for (int lineCount = 0 ; lineCount < totalLines ; lineCount++){
    inputCube.Read(lineManager);
	 //Read the edges into the fringe buffers
	 for (int i = 0 ; i < leftFringe ; i++){
      leftFringeBuf[i] = lineManager[i];
	 }
	 for (int i = rightFringe ; i < totalSamples ; i++){
      rightFringeBuf[i - rightFringe] = lineManager[i]; 
	 }
	 
	 //No matter what, add the fringe buffers to the totals for that side
	 leftTotal.AddData(leftFringeBuf.DoubleBuffer(), leftFringeBuf.size());
	 rightTotal.AddData(rightFringeBuf.DoubleBuffer(), rightFringeBuf.size());
	 
	 if (numSections == 0){
      continue;
	 } 
	 //Index is not too large for this fringe section
    if (lineCount < sectionStarts[currentSection] + sectionLength){
	   //Index is not too small for this fringe section
		if (lineCount >= sectionStarts[currentSection]){
        sections[currentSection][0].AddData(leftFringeBuf.DoubleBuffer(), 
	       leftFringeBuf.size());
		  sections[currentSection][1].AddData(rightFringeBuf.DoubleBuffer(),
		    rightFringeBuf.size());
		}
	 }
	 else{
	   currentSection++;
		//Since sections may butt up against each other, it is possible that
		// we have to add this data to the next section.
	   if (lineCount >= sectionStarts[currentSection]){ 
        sections[currentSection][0].AddData(leftFringeBuf.DoubleBuffer(), 
	       leftFringeBuf.size());
		  sections[currentSection][1].AddData(rightFringeBuf.DoubleBuffer(),
		    rightFringeBuf.size());
		}
	 }
  }
  
  // Write the results to the output file if the user specified one
PvlObject leftSide("LeftSide"), rightSide("RightSide");
  for (int i = 0 ; i < numSections ; i++){
    iString sectionNumber = i + 1;
	 string sectionName = "Section" + sectionNumber; 
    pvlOut(sections[i][0], //Stats to add to the left Object
		     sections[i][1], //Stats to add to the right Object
           sectionName,    //Name for the new groups
			  sectionStarts[i], //start line
			  sectionStarts[i] + sectionLength, //end line
			  &leftSide,      //Object to add left group to
			  &rightSide      //Object to add right group to 
			  );
  }
  pvlOut(leftTotal,   //Stats to add to the left Object
 		   rightTotal,  //Stats to add to the right Object
         "Total",     //Name for the new groups
	  	   0,           //start line
			totalLines,  //end line
			&leftSide,   //Object to add left group to
			&rightSide   //Object to add right group to 
			);
  Pvl outputPvl;
  PvlGroup sourceInfo("SourceInfo");
  
  sourceInfo += PvlKeyword("From", fromFile.Expanded());
  sourceInfo += icube->GetGroup("Archive")["ProductId"];
  outputPvl.AddGroup(sourceInfo);
  if (numSections > 0){
    outputPvl.AddObject(leftSide);
    outputPvl.AddObject(rightSide);
  }
  else{
    PvlGroup leftGroup = leftSide.FindGroup("Total");
    PvlGroup rightGroup = rightSide.FindGroup("Total");
	 leftGroup.SetName("LeftSide");
	 rightGroup.SetName("RightSide");
	 outputPvl.AddGroup(leftGroup);
	 outputPvl.AddGroup(rightGroup);
  }
  outputPvl.Write(ui.GetFilename("TO"));
}

void pvlOut(Statistics stats1, Statistics stats2, string name, int start,
            int end, PvlObject *one, PvlObject *two){
  PvlGroup left(name);
  left += PvlKeyword ("StartLine", start + 1);
  left += PvlKeyword ("EndLine", end);
  left += PvlKeyword ("TotalPixels", stats1.TotalPixels());
  left += PvlKeyword("ValidPixels",stats1.ValidPixels());
  if (stats1.ValidPixels() > 0) {
    left += PvlKeyword("Mean", stats1.Average());
    left += PvlKeyword("StandardDeviation", stats1.StandardDeviation());
    left += PvlKeyword("Minimum", stats1.Minimum());
    left += PvlKeyword("Maximum", stats1.Maximum());
  }
  one->AddGroup(left);

  PvlGroup right(name);
  right += PvlKeyword ("StartLine", start + 1);
  right += PvlKeyword ("EndLine", end);
  right += PvlKeyword ("TotalPixels", stats2.TotalPixels());
  right += PvlKeyword("ValidPixels",stats2.ValidPixels());
  if (stats2.ValidPixels() > 0) {
    right += PvlKeyword("Mean", stats2.Average());
    right += PvlKeyword("StandardDeviation", stats2.StandardDeviation());
    right += PvlKeyword("Minimum", stats2.Minimum());
    right += PvlKeyword("Maximum", stats2.Maximum());
  }
  two->AddGroup(right);
}
