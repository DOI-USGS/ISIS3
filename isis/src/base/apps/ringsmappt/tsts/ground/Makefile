# Test for ringsmappt ground (interprets coordinate as (ringradius,ringlongitude) )
APPNAME = ringsmappt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.lev2.cub \
	  to=$(OUTPUT)/groundInputAndUniversalCoordAppended.pvl \
	  format=pvl \
	  append=yes \
	  type=ground \
	  ringradius=102282693.8240891993045807 \
	  ringlongitude=234.9416974425 \
	  coordsys=inputfilesys > /dev/null;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.lev2.cub \
	  to=$(OUTPUT)/groundInputAndUniversalCoordAppended.pvl \
	  format=pvl \
	  append=yes \
	  type=ground \
	  ringradius=102282693.8240891993045807 \
	  ringlongitude=234.9416974425 \
	  coordsys=universal > /dev/null;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.lev2.cub \
	  to=$(OUTPUT)/groundUserDefinedCoordAppended_tmp.txt \
	  format=flat \
	  append=yes \
	  type=ground \
	  ringradius=102282693.8240891993045807 \
	  ringlongitude=234.9416974425 \
	  coordsys=userdefined \
	  ringlondir=counterclockwise \
	  ringlondom=360 > /dev/null;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.lev2.detached.cw180.lbl \
	  to=$(OUTPUT)/groundUserDefinedCoordAppended_tmp.txt \
	  format=flat \
	  append=yes \
	  type=ground \
	  ringradius=102282693.8240891993045807 \
	  ringlongitude=125.0583025575 \
	  coordsys=userdefined \
	  ringlondir=clockwise \
	  ringlondom=180 > /dev/null;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.lev2.detached.ccw180.lbl \
	  to=$(OUTPUT)/groundUserDefinedCoordAppended_tmp.txt \
	  format=flat \
	  append=yes \
	  type=ground \
	  ringradius=102282693.8240891993045807 \
	  ringlongitude=-125.0583025575 \
	  coordsys=userdefined \
	  ringlondir=counterclockwise \
	  ringlondom=180 > /dev/null;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.lev2.detached.cw360.lbl \
	  to=$(OUTPUT)/groundUserDefinedCoordAppended_tmp.txt \
	  format=flat \
	  append=yes \
	  type=ground \
	  ringradius=102282693.8240891993045807 \
	  ringlongitude=125.0583025575 \
	  coordsys=userdefined \
	  ringlondir=clockwise \
	  ringlondom=360 > /dev/null;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.lev2.cub \
	  to=$(OUTPUT)/groundMapCoord_tmp.txt \
	  format=flat \
	  append=no \
	  type=ground \
	  ringradius=102282693.8240891993045807 \
	  ringlongitude=234.9416974425 \
	  coordsys=map \
	  map=$(INPUT)/W1591159850_1_cal.lev2.cub > /dev/null;
	  $(SED) 's+/.*/input/+input/+' $(OUTPUT)/groundUserDefinedCoordAppended_tmp.txt > $(OUTPUT)/groundUserDefinedCoordAppended.txt;
	  $(SED) 's+/.*/input/+input/+' $(OUTPUT)/groundMapCoord_tmp.txt > $(OUTPUT)/groundMapCoord.txt;
	  $(RM) $(OUTPUT)/groundUserDefinedCoordAppended_tmp.txt;
	  $(RM) $(OUTPUT)/groundMapCoord_tmp.txt;

