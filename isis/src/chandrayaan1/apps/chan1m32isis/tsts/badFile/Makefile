APPNAME = chan1m32isis

include $(ISISROOT)/make/isismake.tsts

commands:
	if [ `$(APPNAME) from=$(INPUT)/lua1855j.310 \
	  loc=$(OUTPUT)/loc.cub \
	  obs=$(OUTPUT)/obs.cub \
          to=$(OUTPUT)/junk.cub >& $(OUTPUT)/temporary.txt` ]; \
        then \
	  true; \
        fi;
	$(SED) 's+\(\[.*/\)\(.*\]\)+\[\2+' $(OUTPUT)/temporary.txt >& $(OUTPUT)/error.txt; \
	rm $(OUTPUT)/temporary.txt; \

