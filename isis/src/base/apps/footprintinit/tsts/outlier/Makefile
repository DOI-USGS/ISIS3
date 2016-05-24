APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# Test the outlier check, avoids choosing a first point without any valid
# neighbors.  Without this check, the program should be unable to find a valid
# polygon because it will choose a bad first point every time.
# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/EW0215651170I.lev1.cub $(OUTPUT)/EW0215651170I.lev1.cub;
	$(APPNAME) from=$(OUTPUT)/EW0215651170I.lev1.cub \
	           sinc=10 \
	           linc=10 \
	           maxemission=89 \
	           maxincidence=89 \
	           >& /dev/null;
	$$ISISROOT/bin/blobdump from=$(OUTPUT)/EW0215651170I.lev1.cub \
	                        to=$(OUTPUT)/EW0215651170I.lev1.txt \
	                        name=footprint \
	                        type=Polygon \
	                        >& /dev/null;
	$(CAT) $(OUTPUT)/EW0215651170I.lev1.txt \
           | $(SED) 's/\([0-9]\.[0-9]\{5\}\)\([0-9]*\)/\1/g' \
           | $(SED) -e '$$G' \
	       >& $(OUTPUT)/EW0215651170I.lev1.trimmed.txt;
	$(RM) $(OUTPUT)/EW0215651170I.lev1.txt;
	$(RM) $(OUTPUT)/EW0215651170I.lev1.cub;
