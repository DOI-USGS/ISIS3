# This tests the following conditions:
#
# To verify the table data, use tabledump.
#
# To verify the label data, use catlab.
APPNAME = pds2hideal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/exported.lbl \
	  to=$(OUTPUT)/reimported.cub > /dev/null;
	tabledump from=$(OUTPUT)/reimported.cub \
	  to=$(OUTPUT)/reimportedInstrumentPointingTable.txt \
	  name=InstrumentPointing > /dev/null;
	tabledump from=$(OUTPUT)/reimported.cub \
	  to=$(OUTPUT)/reimportedInstrumentPositionTable.txt \
	  name=InstrumentPosition > /dev/null;
	tabledump from=$(OUTPUT)/reimported.cub \
	  to=$(OUTPUT)/reimportedBodyRotationTable.txt \
	  name=BodyRotation > /dev/null;
	tabledump from=$(OUTPUT)/reimported.cub \
	  to=$(OUTPUT)/reimportedSunPositionTable.txt \
	  name=SunPosition > /dev/null;
	catlab from=$(OUTPUT)/reimported.cub \
	  to=$(OUTPUT)/reimported.pvl > /dev/null;
