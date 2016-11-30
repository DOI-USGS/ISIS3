# This test has been turned off since USEPVL and SC_PARAMETERS are turned off (#4293, #4307).
#
# This is an observation test that tests sets of images corresponding to three different sensors:
# Apollo Metric, LRO NAC, and Lunar Orbiter (LO).
# 
# Tested parameters:
#   USEPVL is used with SC_PARAMETERS to load settings for the different sensors.
#   Solve for radius of control points (RADIUS=yes) with uncertainty of 500m (POINT_RADIUS_SIGMA).
#   Treat each subframe as a single observation (OBSERVATIONS=yes).
#   Each output file will be prefixed by "temp_" (FILE_PREFIX=temp_).
#   Turn on error propagation (ERRORPROPAGATION=yes).
#
# 2016-08-20 Ian Humphrey - Original version adapted from
#                /work/projects/progteam/kedmundson/jigsaw_tests/multiple_sensors/ObservationMode/LO.
#                Fixes #4122.
# 2016-10-13 Ian Humphrey - Turned this test off, as USEPVL and SC_PARAMETERS have been disabled.
#                Refernces #4293, #4293.
APPNAME = jigsaw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(ECHO) "This test has been turned off." > $(OUTPUT)/truth.txt;
	#$(LS) -1 $(INPUT)/*cub > $(OUTPUT)/AS15_LandingSide_Metric-NAC-LO5H-FIN.lis;
	#$(APPNAME) fromlist=$(OUTPUT)/AS15_LandingSide_Metric-NAC-LO5H-FIN.lis \
	  #cnet=$(INPUT)/AS15_LandingSide_Metric-NAC-LO5H-FIN.net \
	  #onet=$(OUTPUT)/out.net \
	  #usepvl=yes \
	  #sc_parameters=$(INPUT)/APOLLO_LRONAC_LO.pvl \
	  #observations=yes \
	  #radius=yes \
	  #errorpropagation=yes \
	  #point_radius_sigma=500 \
	  #file_prefix=temp \
	  #> /dev/null
	#$(CAT) temp_bundleout.txt  | grep -v "Run Time:" | grep -v "Elapsed Time:" \
	  #| perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2>/dev/null \
	  #| $(SED) 's/\([0-9][0-9]*\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9][0-9]*\)/\1/g' \
	  #| $(SED) s/`date +%Y-%m-%dT`\[0-2\]\[0-9\]:\[0-5\]\[0-9\]:\[0-5\]\[0-9\]/date/ \
	  #| $(SED) 's/\(Iterations:\).*/\1/' \
	  #> $(OUTPUT)/multiSensors_bundleout.txt;
	#$(CAT) temp_residuals.csv | $(SED) 's/,[^,]*\/\([^,\/]*\.cub\)/,\1/g'\
	  #> $(OUTPUT)/multiSensors_residuals.csv;
	#$(MV) temp_bundleout_points.csv $(OUTPUT)/multiSensors_bundleout_point.csv \
	  #> /dev/null;
	#$(RM) print.prt > /dev/null;
	#$(RM) temp_* > /dev/null;
	#$(RM) $(OUTPUT)/AS15_LandingSide_Metric-NAC-LO5H-FIN.lis > /dev/null;
