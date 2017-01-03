
****INTRODUCTION****

The VIMS IR wavelengths have been gradually shifting throughout the Cassini mission.
A model of this shifting over time (in years) exists, but it changes how the calibration
is done for the mission.  In this application, the calibration files are stored as cube
files.  In RC17, four calibration cubes are read from by the application.  In 
RC19 (which takes into account the wavelength shift), a calibration cube for each year
of the mission is generated from a text file.   In the event that there will be future
calibration files that need to be generated, this README provides instructions for 
generating those files.



****HOW TO MAKE THE CALIBRATION CUBES****

After the calibration files are extracted, there should be four separate folders in 
the calibration folder (cal_files), each containing 14 files calibration files:

cal_files
   |
   |----------band-wavelengths
   |                |
   |                |---------------wavelengths.2004.txt
   |                |---------------wavelengths.2005.txt
   |                |                         :
   |                |                         :
   |                |---------------wavelengths.2017.txt
   |
   |----------RC19-mults
   |                |
   |                |---------------RC19-2004.txt
   |                |---------------RC19-2005.txt
   |                |                       :
   |                |                       :
   |                |---------------RC19-2017.txt
   |                |---------------AAA.readme.txt
   |
   |----------wave-cal
   |                |
   |                |---------------wave-cal.2004.txt
   |                |---------------wave-cal.2005.txt
   |                |                        :
   |                |                        :
   |                |---------------wave-cal.2017.txt        
   |                |---------------AAA.readme.txt
   |
   |----------solar-spectrum
   |                |
   |                |---------------solar.2004.txt
   |                |---------------solar.2005.txt
   |                |                    :
   |                |                    :
   |                |---------------solar.2017.txt 
   |                |---------------AAA.readme.txt
   

To generate the cubes, first call setisis to set an ISIS version.  Then run the makecubes.py 
Python script.  The syntax for this script is as follows:

python makecubes.py <path to folder> <prefix> <version> 

<path to folder>:  The path to the the subfolder in cal_files.  
<prefix>        :  A prefix that uniquely identifies the calibration txt files
                   from other files.  If we are in the RC19-mults folder,
                   the prefix would be RC19.  
<version>       :  A four digit number identifying the current calibration version.  Currently,
                   the version is 0001.  If this is the next version, it would be 0002.

Example:  If the cal_files and the makecubes.py script are both located in /scratch and we want to 
generate the solar-spectrum cubes for version 0002, the command for doing that looks like:

$python makecubes.py /scratch/cal_files/solar-spectrum solar 0002 


After the cubes are generated, then copy them to the appropriate folder in:

$ISIS3DATA/cassini/calibration/vims/RC19

RC19 has the same subfolders as those enumerated above.  Obviously you will need to be logged
in as isis3mgr to do this.


****THE PVL MULTIPLIER FILE****

In $ISIS3DATA/cassini/calibration/vims, there is a file called:  vimsCalibration????.trn which
looks like this:

Group=CalibrationMultipliers
    version = RC19
    RC = 29554
    solar = 1000.0
    wave-cal=1000.0
EndGroup

End

The AAA.readme.txt files contain the multiplier values which are in this file.  The calibration
version also needs to be updated.  Note:  the calibration version here is not the same as the
file version which is used as an argument to the makecubes script.




   
