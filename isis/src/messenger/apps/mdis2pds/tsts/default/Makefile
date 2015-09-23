# Tests defaults for mdis2pds
#
# defaults to type=automatic, 
#             min = -1, 
#             max =  1, 
#             minper=0, 
#             maxper=100, 
#             bits=32
#
# Note that the first test has the old keyword(s), ContaminationEvent
# while the second test has the new keyword(s), EmpiricalCorrection
APPNAME = mdis2pds

oldCalibrationLabelOutput.cub.TOLERANCE = 0.0000000002
newCalibrationLabelOutput.cub.TOLERANCE = 0.0000000002
oldCalibrationLabelOutput.pvl.IGNORELINES = PRODUCT_CREATION_TIME
newCalibrationLabelOutput.pvl.IGNORELINES = PRODUCT_CREATION_TIME
#NOPREFERENCES=

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/EN0108840044M.cal.cub \
	to=$(OUTPUT)/oldCalibrationLabelOutput > /dev/null;
	catlab from=$(OUTPUT)/oldCalibrationLabelOutput.IMG \
	to=$(OUTPUT)/oldCalibrationLabelOutput.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/oldCalibrationLabelOutput.IMG \
	to=$(OUTPUT)/oldCalibrationLabelOutput > /dev/null;
	$(RM) $(OUTPUT)/oldCalibrationLabelOutput.IMG;

	$(APPNAME) from=$(INPUT)/EW0089565626A.cal.cub \
	to=$(OUTPUT)/newCalibrationLabelOutput > /dev/null;
	catlab from=$(OUTPUT)/newCalibrationLabelOutput.IMG \
	to=$(OUTPUT)/newCalibrationLabelOutput.pvl > /dev/null;
	pds2isis from=$(OUTPUT)/newCalibrationLabelOutput.IMG \
	to=$(OUTPUT)/newCalibrationLabelOutput > /dev/null;
	$(RM) $(OUTPUT)/newCalibrationLabelOutput.IMG;
