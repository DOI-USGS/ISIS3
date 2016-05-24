APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/V30027015RDR.even.cub $(OUTPUT)/V30027015RDR.even.cub;
	$(APPNAME) from=$(OUTPUT)/V30027015RDR.even.cub \
	           sinc=65 \
	           linc=65 \
	           >& /dev/null;
	$$ISISROOT/bin/blobdump from=$(OUTPUT)/V30027015RDR.even.cub \
	                        to=$(OUTPUT)/V30027015RDR.even.txt \
	                        name=footprint \
	                        type=Polygon \
	                        >& /dev/null;
	$(CAT) $(OUTPUT)/V30027015RDR.even.txt \
	       | $(SED) 's/\([0-9]\.[0-9]\{6\}\)\([0-9]*\)/\1/g' \
	       | $(SED) -e '$$G' \
		   >& $(OUTPUT)/V30027015RDR.trimmed.even.txt;
	$(RM) $(OUTPUT)/V30027015RDR.even.cub $(OUTPUT)/V30027015RDR.even.txt;
