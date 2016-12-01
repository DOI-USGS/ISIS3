# this tests sumspice using a single cube and sumfile as inputs on a Hayabusa image
# Set up a TABLEDUMP variable to use test preferences, since the make system
# will only set up test preferences for APPNAME
# @internal
#   @history 2016-12-01 Ian Humphrey - Set up the TABLEDUMP variable so that ISIS's
#                           TestPreferences are used when running the tabledump commands.
APPNAME = sumspice
TABLEDUMP = $(ISISROOT)/bin/tabledump -preference=$(ISISROOT)/src/base/objs/Preference/TestPreferences

include $(ISISROOT)/make/isismake.tsts

commands:
	# copy cube to output directory since the input cube is altered by this program
	$(CP) $(INPUT)/st_2395699394_v.lev0.cub $(OUTPUT)/st_2395699394_v.lev0.cub;

	# first run of sumspice updates the time
	$(APPNAME) from=$(OUTPUT)/st_2395699394_v.lev0.cub \
	sumfile=$(INPUT)/N2395699394.SUM \
	mode=updatetime \
	-log=$(OUTPUT)/sumspiceUpdateClocksLog.pvl \
	> /dev/null;

	# re-spiceinit to get appropriate kernels and sun position table
	spiceinit from=$(OUTPUT)/st_2395699394_v.lev0.cub \
	shape=user \
	model=$(ISIS3DATA)/hayabusa/kernels/dsk/hay_a_amica_5_itokawashape_v1_0_512q.bds \
	> /dev/null;

	# save off labels from the first run 
	catlab from=$(OUTPUT)/st_2395699394_v.lev0.cub \
	to=$(OUTPUT)/updatedClocksCubeLabel.pvl \
	> /dev/null;

	# save off tables from the first run
	echo -e "InstrumentPointingTable" \
	> $(OUTPUT)/updatedClocksTables.txt;
	$(TABLEDUMP) from=$(OUTPUT)/st_2395699394_v.lev0.cub \
	name="InstrumentPointing" \
	>> $(OUTPUT)/updatedClocksTables.txt;

	echo -e "\nInstrumentPosition" \
	>> $(OUTPUT)/updatedClocksTables.txt;
	$(TABLEDUMP) from=$(OUTPUT)/st_2395699394_v.lev0.cub \
	name="InstrumentPosition" \
	>> $(OUTPUT)/updatedClocksTables.txt;

	echo -e "\nSunPosition" \
	>> $(OUTPUT)/updatedClocksTables.txt;
	$(TABLEDUMP) from=$(OUTPUT)/st_2395699394_v.lev0.cub \
	name="SunPosition" \
	>> $(OUTPUT)/updatedClocksTables.txt;

	echo -e "\nBodyRotation" \
	>> $(OUTPUT)/updatedClocksTables.txt;
	$(TABLEDUMP) from=$(OUTPUT)/st_2395699394_v.lev0.cub \
	name="BodyRotation" \
	>> $(OUTPUT)/updatedClocksTables.txt;

	# second run of sumspice updates the tables
	$(APPNAME) from=$(OUTPUT)/st_2395699394_v.lev0.cub \
	sumfile=$(INPUT)/N2395699394.SUM \
	mode=updatespice \
	-log=$(OUTPUT)/sumspiceUpdateSpiceLog.pvl \
	> /dev/null;

	# save off labels from the second run
	catlab from=$(OUTPUT)/st_2395699394_v.lev0.cub \
	to=$(OUTPUT)/updatedSpiceCubeLabel.pvl \
	> /dev/null;

	# save off tables from second run
	echo -e "InstrumentPointingTable" \
	> $(OUTPUT)/updatedSpiceTables.txt;
	$(TABLEDUMP) from=$(OUTPUT)/st_2395699394_v.lev0.cub \
	name="InstrumentPointing" \
	>> $(OUTPUT)/updatedSpiceTables.txt;

	echo -e "\nInstrumentPosition" \
	>> $(OUTPUT)/updatedSpiceTables.txt;
	$(TABLEDUMP) from=$(OUTPUT)/st_2395699394_v.lev0.cub \
	name="InstrumentPosition" \
	>> $(OUTPUT)/updatedSpiceTables.txt;

	echo -e "\nSunPosition" \
	>> $(OUTPUT)/updatedSpiceTables.txt;
	$(TABLEDUMP) from=$(OUTPUT)/st_2395699394_v.lev0.cub \
	name="SunPosition" \
	>> $(OUTPUT)/updatedSpiceTables.txt;

	echo -e "\nBodyRotation" \
	>> $(OUTPUT)/updatedSpiceTables.txt;
	$(TABLEDUMP) from=$(OUTPUT)/st_2395699394_v.lev0.cub \
	name="BodyRotation" \
	>> $(OUTPUT)/updatedSpiceTables.txt;

	# There are comments that contain run timestamps. These can't be ignored using the DIFF file
	# since they are in the comments, not a keyword.
	$(CAT) $(OUTPUT)/updatedSpiceCubeLabel.pvl | grep -v "with SUMFILE" >& $(OUTPUT)/tempLabel.txt;
	$(MV) $(OUTPUT)/tempLabel.txt $(OUTPUT)/updatedSpiceCubeLabel.pvl;
