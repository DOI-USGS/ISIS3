APPNAME = isis2gml

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) \
	  from=$(INPUT)/ab102401_EQ360.cub \
	  to=$(OUTPUT)/ab102401_EQ360.gml \
	  longitudedomain=360 \
	  label= "Projected lon domain 360" > /dev/null;
	cat $(OUTPUT)/ab102401_EQ360.gml | \
	  sed 's/\([0-9]\.[0-9]\{9\}\)\([0-9]*\)/\1/g' > \
	  $(OUTPUT)/ab102401_EQ360.gml.txt;
	$(MV) $(OUTPUT)/ab102401_EQ360.xsd $(OUTPUT)/ab102401_EQ360.xsd.txt > /dev/null;
	$(RM) $(OUTPUT)/ab102401_EQ360.gml > /dev/null;
	$(APPNAME) \
	  from=$(INPUT)/ab102401_EQ180.cub \
	  to=$(OUTPUT)/ab102401_EQ180.gml \
	  longitudedomain=180 \
	  label= "Projected lon domain 180" > /dev/null;
	cat $(OUTPUT)/ab102401_EQ180.gml | \
	  sed 's/\([0-9]\.[0-9]\{9\}\)\([0-9]*\)/\1/g' > \
	  $(OUTPUT)/ab102401_EQ180.gml.txt;
	$(MV) $(OUTPUT)/ab102401_EQ180.xsd $(OUTPUT)/ab102401_EQ180.xsd.txt > /dev/null;
	$(RM) $(OUTPUT)/ab102401_EQ180.gml > /dev/null;

