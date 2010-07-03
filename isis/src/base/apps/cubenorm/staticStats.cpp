/*	StaticStats

PIRL CVS ID: $Id: staticStats.cpp,v 1.1 2007/01/11 20:59:17 kbecker Exp $

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
#include "staticStats.h"

using namespace PIRL;

  const char* const
	StaticStats::ID =
	   "PIRL::StaticStats ($Revision: 1.1 $ $Date: 2007/01/11 20:59:17 $)";
  StaticStats::StaticStats(double meanIn, double stdDevIn, int validPixelsIn,
  			   double minimumIn, double maximumIn){
    myMean = meanIn;
    myStandardDeviation = stdDevIn;
    myValidPixels = validPixelsIn;
    myMinimum = minimumIn;
    myMaximum = maximumIn;
  }
  double StaticStats::Average(){
    return myMean;
  }
  void StaticStats::setMean(double meanIn){
    myMean = meanIn;
  }
  double StaticStats::StandardDeviation(){
    return myStandardDeviation;
  }
  void StaticStats::setStandardDeviation(double standardDeviationIn){
    myStandardDeviation = standardDeviationIn;
  }
  int StaticStats::ValidPixels(){
    return myValidPixels;
  }
  void StaticStats::setValidPixels(int validPixelsIn){
    myValidPixels = validPixelsIn;
  }
  double StaticStats::Minimum(){
    return myMinimum;
  }
  void StaticStats::setMinimum(double minimumIn){
    myMinimum = minimumIn;
  }
  double StaticStats::Maximum(){
    return myMaximum;
  }
  void StaticStats::setMaximum(double maximumIn){
    myMaximum = maximumIn;
  }
