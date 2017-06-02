# this tests sumspice using cubelist and sumfilelist inputs on DAWN images
# updated for addition of UPDATE, SUMTIME, and TOLOG 2016-12-06 Kristin Berry
APPNAME = sumspice

include $(ISISROOT)/make/isismake.tsts

commands:
	# copy cubes to output directory since the input cube is altered by this program
	$(CP) $(INPUT)/*.cub $(OUTPUT);
	$(CP) $(INPUT)/cubeNames.txt $(OUTPUT);

	# create input cubelist and sumfilelist
	$(LS) $(OUTPUT)/*.cub > $(OUTPUT)/cubelist.txt;
	$(LS) $(INPUT)/*.SUM > $(OUTPUT)/sumfilelist.txt;

	# first run of sumspice updates the time
	$(APPNAME) fromlist=$(OUTPUT)/cubelist.txt \
	sumfilelist=$(OUTPUT)/sumfilelist.txt \
	sumtime=start\
	update=times \
	timediff=1.0 \
	tolog=$(OUTPUT)/sumspiceUpdateClocksToLog.txt \
	-log=$(OUTPUT)/sumspiceUpdateClocksLog.pvl \
	> /dev/null;

	# re-spiceinit to get appropriate kernels and sun position table
	spiceinit from=$(OUTPUT)/\$$\1.cub \
	shape=user \
	model=$(INPUT)/vesta512.bds \
	pck=$(INPUT)/dawn_vesta_v06c.tpc \
	-batchlist=$(OUTPUT)/cubeNames.txt \
	> /dev/null;

	# save off labels from the first run 
	catlab from=$(OUTPUT)/\$$\1.cub \
	to=$(OUTPUT)/\$$\1_updatedClocks_label.pvl \
	-batchlist=$(OUTPUT)/cubeNames.txt \
	> /dev/null;

	# save off tables from the first run
	echo -e "InstrumentPointingTable" \
	> $(OUTPUT)/updatedClocksTables.txt;
	tabledump from=$(OUTPUT)/\$$\1.cub \
	name="InstrumentPointing" \
	-batchlist=$(OUTPUT)/cubeNames.txt \
	>> $(OUTPUT)/updatedClocksTables.txt;

	echo -e "\nInstrumentPosition" \
	>> $(OUTPUT)/updatedClocksTables.txt;
	tabledump from=$(OUTPUT)/\$$\1.cub \
	name="InstrumentPosition" \
	-batchlist=$(OUTPUT)/cubeNames.txt \
	>> $(OUTPUT)/updatedClocksTables.txt;

	echo -e "\nSunPosition" \
	>> $(OUTPUT)/updatedClocksTables.txt;
	tabledump from=$(OUTPUT)/\$$\1.cub \
	name="SunPosition" \
	-batchlist=$(OUTPUT)/cubeNames.txt \
	>> $(OUTPUT)/updatedClocksTables.txt;

	echo -e "\nBodyRotation" \
	>> $(OUTPUT)/updatedClocksTables.txt;
	tabledump from=$(OUTPUT)/\$$\1.cub \
	name="BodyRotation" \
	-batchlist=$(OUTPUT)/cubeNames.txt \
	>> $(OUTPUT)/updatedClocksTables.txt;

	# second run of sumspice updates the tables
	$(APPNAME) fromlist=$(OUTPUT)/cubelist.txt \
	sumfilelist=$(OUTPUT)/sumfilelist.txt \
	sumtime=start \
	update=spice \
	-log=$(OUTPUT)/sumspiceUpdateSpiceLog.pvl \
	> /dev/null;

	# save off labels from the second run
	catlab from=$(OUTPUT)/\$$\1.cub \
	to=$(OUTPUT)/\$$\1_updatedSpice_label.pvl \
	-batchlist=$(OUTPUT)/cubeNames.txt \
	> /dev/null;

	# save off tables from second run
	echo -e "InstrumentPointingTable" \
	> $(OUTPUT)/updatedSpiceTables.txt;
	tabledump from=$(OUTPUT)/\$$\1.cub \
	name="InstrumentPointing" \
	-batchlist=$(OUTPUT)/cubeNames.txt \
	>> $(OUTPUT)/updatedSpiceTables.txt;

	echo -e "\nInstrumentPosition" \
	>> $(OUTPUT)/updatedSpiceTables.txt;
	tabledump from=$(OUTPUT)/\$$\1.cub \
	name="InstrumentPosition" \
	-batchlist=$(OUTPUT)/cubeNames.txt \
	>> $(OUTPUT)/updatedSpiceTables.txt;

	echo -e "\nSunPosition" \
	>> $(OUTPUT)/updatedSpiceTables.txt;
	tabledump from=$(OUTPUT)/\$$\1.cub \
	name="SunPosition" \
	-batchlist=$(OUTPUT)/cubeNames.txt \
	>> $(OUTPUT)/updatedSpiceTables.txt;

	echo -e "\nBodyRotation" \
	>> $(OUTPUT)/updatedSpiceTables.txt;
	tabledump from=$(OUTPUT)/\$$\1.cub \
	name="BodyRotation" \
	-batchlist=$(OUTPUT)/cubeNames.txt \
	>> $(OUTPUT)/updatedSpiceTables.txt;

	$(RM) $(OUTPUT)/sumfilelist.txt;
