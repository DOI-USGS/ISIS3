APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/f843a02.cub $(OUTPUT)/f843a02.cub;
	$(APPNAME) from=$(OUTPUT)/f843a02.cub \
	           testxy=true \
	           map=$(INPUT)/map.pvl \
	           >& /dev/null;
	$$ISISROOT/bin/blobdump from=$(OUTPUT)/f843a02.cub \
	                        to=$(OUTPUT)/f843a02.txt \
	                        name=footprint \
	                        type=Polygon \
	                        >& /dev/null;
	$(CAT) $(OUTPUT)/f843a02.txt \
	       | $(SED) 's/\([0-9]\.[0-9]\{5\}\)\([0-9]*\)/\1/g' \
	       | $(SED) -e '$$G' \
	       >& $(OUTPUT)/f843a02.trimmed.txt;
	$(RM) $(OUTPUT)/f843a02.cub $(OUTPUT)/f843a02.txt;
