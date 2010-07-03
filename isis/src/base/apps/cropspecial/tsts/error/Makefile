APPNAME = cropspecial

include $(ISISROOT)/make/isismake.tsts

commands:
	if [ `$(APPNAME) from=$(INPUT)/allnull.cub to=$(OUTPUT)/fail.cub \
	  -preference=$(ISISROOT)/src/base/objs/Preference/TestPreferences \
	   >& $(OUTPUT)/error.txt` ]; \
	then \
	  true; \
	fi;
