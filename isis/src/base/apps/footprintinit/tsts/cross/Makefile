APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/f174s47.cub $(OUTPUT)/f174s47.cub;
	$(APPNAME) from=$(OUTPUT)/f174s47.cub \
	           sinc=100 \
	           linc=100 \
	           >& /dev/null;
	$$ISISROOT/bin/blobdump from=$(OUTPUT)/f174s47.cub \
	                        to=$(OUTPUT)/f174s47.txt \
	                        name=footprint \
	                        type=Polygon \
	                        >& /dev/null;
	$(CAT) $(OUTPUT)/f174s47.txt \
           | $(SED) 's/\([0-9]\.[0-9]\{4\}\)\([0-9]*\)/\1/g' \
           | $(SED) -e '$$G' \
		   >& $(OUTPUT)/f174s47.trimmed.txt;
	$(RM) $(OUTPUT)/f174s47.cub $(OUTPUT)/f174s47.txt;
