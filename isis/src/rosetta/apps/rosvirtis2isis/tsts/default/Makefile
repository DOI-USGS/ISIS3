# Test for rosvirtis2isis with normal (non-PDS-draft) input qub

APPNAME = rosvirtis2isis 
APPNAME2 = catlab

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/i1_00237330013.qub to=$(OUTPUT)/i1_00237330013.cub > /dev/null;
	$(APPNAME2) from=$(OUTPUT)/i1_00237330013.cub to=$(OUTPUT)/i1_00237330013.pvl > /dev/null;
