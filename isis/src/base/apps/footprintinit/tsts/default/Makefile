APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/ab102401.cub $(OUTPUT)/ab102401.cub;
	$(APPNAME) from=$(OUTPUT)/ab102401.cub \
	           sinc=1 \
	           linc=1 \
	           >& /dev/null;
	$$ISISROOT/bin/blobdump from=$(OUTPUT)/ab102401.cub \
	                        to=$(OUTPUT)/ab102401.txt \
	                        name=footprint \
	                        type=Polygon \
	                        >& /dev/null;
	$(CAT) $(OUTPUT)/ab102401.txt \
           | $(SED) 's/\([0-9]\.[0-9]\{5\}\)\([0-9]*\)/\1/g' \
           | $(SED) -e '$$G' \
	       >& $(OUTPUT)/ab102401.trimmed.txt;
	$(RM) $(OUTPUT)/ab102401.txt;
	$(APPNAME) from=$(OUTPUT)/ab102401.cub \
	           inctype=vertices \
	           numvertices=8 \
	           >& /dev/null;
	$$ISISROOT/bin/blobdump from=$(OUTPUT)/ab102401.cub \
	                        to=$(OUTPUT)/ab102401.txt \
	                        name=footprint \
	                        type=Polygon \
	                        >& /dev/null;
	$(CAT) $(OUTPUT)/ab102401.txt \
           | $(SED) 's/\([0-9]\.[0-9]\{5\}\)\([0-9]*\)/\1/g' \
           | $(SED) -e '$$G' \
	       >& $(OUTPUT)/ab102401.vertices.trimmed.txt;
	$(RM) $(OUTPUT)/ab102401.cub $(OUTPUT)/ab102401.txt;
