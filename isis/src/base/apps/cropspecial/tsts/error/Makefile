APPNAME = cropspecial

include $(ISISROOT)/make/isismake.tsts

commands:
	if [ `$(APPNAME) from=$(INPUT)/allnull.cub to=$(OUTPUT)/fail.cub \
	   >& $(OUTPUT)/error.txt` ]; \
	then \
	  true; \
	fi;
