# Tests how camtrim handles a projected image (it should throw an exception).
APPNAME = camtrim

include $(ISISROOT)/make/isismake.tsts

commands:
	if [ `$(APPNAME) from=$(INPUT)/ab102401_lev2.cub \
	  to=$(OUTPUT)/camtrimTruth1.cub \
	  minlat=-90 \
	  maxlat=90 \
	  minlon=224 \
	  maxlon=226 \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi; \
	$(SED) 's/\[.*\]/[]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error.txt; \
	$(RM) $(OUTPUT)/temp.txt
