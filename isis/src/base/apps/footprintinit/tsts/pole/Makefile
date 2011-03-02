APPNAME = footprintinit

include $(ISISROOT)/make/isismake.tsts

# cp so I don't destroy the input cube
commands:
	$(CP) $(INPUT)/N1500056057_2.cub $(OUTPUT)/N1500056057_2.cub;
	$(APPNAME) from=$(OUTPUT)/N1500056057_2.cub sinc=10 linc=10 \
	  > /dev/null;
	$$ISISROOT/bin/blobdump \
	  from=$(OUTPUT)/N1500056057_2.cub \
	  to=$(OUTPUT)/N1500056057_2.txt \
	  name=footprint \
	  type=Polygon > /dev/null;
	$(CAT) $(OUTPUT)/N1500056057_2.txt | \
	  sed 's/\([0-9]\.[0-9]\{3\}\)\([0-9]*\)/\1/g' | \
	  sed -e '$$G' > $(OUTPUT)/N1500056057_2.trimmed.txt
	rm $(OUTPUT)/N1500056057_2.cub $(OUTPUT)/N1500056057_2.txt
