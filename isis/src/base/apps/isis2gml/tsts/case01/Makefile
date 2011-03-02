APPNAME = isis2gml

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/ab102401.cub $(OUTPUT)/ab102401.cub;
	$(APPNAME) from=$(OUTPUT)/ab102401.cub \
	to=$(OUTPUT)/ab102401gml.txt \
	label= "appTest" > /dev/null;
	$(RM) $(OUTPUT)/ab102401.cub
	cat $(OUTPUT)/ab102401gml.txt | \
	  sed 's/\([0-9]\.[0-9]\{9\}\)\([0-9]*\)/\1/g' \
	  > $(OUTPUT)/ab102401gml2.txt;
	$(MV) $(OUTPUT)/ab102401gml2.txt $(OUTPUT)/ab102401gml.txt \
	  > /dev/null;
