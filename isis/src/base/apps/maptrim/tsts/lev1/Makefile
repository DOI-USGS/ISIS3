# Tests how maptrim handles a level 1 (i.e. non-projected) cube. It should throw
# an exception.
APPNAME = maptrim

include $(ISISROOT)/make/isismake.tsts

commands:
	if [ `$(APPNAME) from=$(INPUT)/ab102401.cub \
	  to=$(OUTPUT)/maptrimTruth.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi; \
	$(SED) 's/\[.*\]/[]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error.txt; \
	$(RM) $(OUTPUT)/temp.txt
