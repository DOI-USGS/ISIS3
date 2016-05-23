APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/EN0108821505M.cal.cub $(OUTPUT)/EN0108821505M.cal.cub;
	$(APPNAME) from=$(OUTPUT)/EN0108821505M.cal.cub \
	           sinc=3 \
	           linc=3 \
	           >& /dev/null;
	$$ISISROOT/bin/blobdump from=$(OUTPUT)/EN0108821505M.cal.cub \
	                        to=$(OUTPUT)/EN0108821505M.txt \
	                        name=footprint \
	                        type=Polygon \
	                        >& /dev/null;
	$(CAT) $(OUTPUT)/EN0108821505M.txt \
	       | $(SED) 's/\([0-9]\.[0-9]\{4\}\)\([0-9]*\)/\1/g' \
	       | $(SED) -e '$$G' \
	       >& $(OUTPUT)/EN0108821505M.trimmed.txt;
	$(RM) $(OUTPUT)/EN0108821505M.cal.cub $(OUTPUT)/EN0108821505M.txt;
