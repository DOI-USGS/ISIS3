APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/tychogrid.cub $(OUTPUT)/tychogrid.cub;
	$(APPNAME) from=$(OUTPUT)/tychogrid.cub \
	  testxy=true > /dev/null;
	$$ISISROOT/bin/blobdump \
	  from=$(OUTPUT)/tychogrid.cub \
	  to=$(OUTPUT)/tychogrid.txt \
	  name=footprint \
	  type=Polygon > /dev/null;
	$(CAT) $(OUTPUT)/tychogrid.txt | \
	  sed 's/\([0-9]\.[0-9]\{5\}\)\([0-9]*\)/\1/g' | \
	  sed -e '$$G' > $(OUTPUT)/tychogrid.trimmed.txt
	rm $(OUTPUT)/tychogrid.cub $(OUTPUT)/tychogrid.txt
