APPNAME = chan1m32isis

include $(ISISROOT)/make/isismake.tsts

commands:
	if [ `$(APPNAME) from=$(INPUT)/lua1855j.310 \
	   loc=$(OUTPUT)/loc.cub \
	   obs=$(OUTPUT)/obs.cub \
           to=$(OUTPUT)/junk.cub >& $(OUTPUT)/error.txt` ]; \
        then \
	  true; \
        fi;
