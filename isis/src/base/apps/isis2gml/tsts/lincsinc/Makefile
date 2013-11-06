APPNAME = isis2gml

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) \
	  from=$(INPUT)/ab102401.cub \
	  to=$(OUTPUT)/ab102401_360_1.gml \
	  longitudedomain=360 \
	  label= "7" \
	  linc=2 \
	  sinc=2 > /dev/null;
	cat $(OUTPUT)/ab102401_360_1.gml | \
	  sed 's/\([0-9]\.[0-9]\{9\}\)\([0-9]*\)/\1/g' > \
	  $(OUTPUT)/ab102401_360_1.gml.txt;
	$(MV) $(OUTPUT)/ab102401_360_1.xsd $(OUTPUT)/ab102401_360_1.xsd.txt > /dev/null;
	$(RM) $(OUTPUT)/ab102401_360_1.gml > /dev/null;
	$(CP) $(INPUT)/ab102401.cub $(OUTPUT)/ab102401.cub > /dev/null;
	$(APPNAME) \
	  from=$(OUTPUT)/ab102401.cub \
	  longitudedomain=360 \
	  label= "9" \
	  linc=3 \
	  sinc=4 > /dev/null;
	cat $(OUTPUT)/ab102401.gml | \
	  sed 's/\([0-9]\.[0-9]\{9\}\)\([0-9]*\)/\1/g' > \
	  $(OUTPUT)/ab102401_360_2.gml.txt;
	$(MV) $(OUTPUT)/ab102401.xsd $(OUTPUT)/ab102401_360_2.xsd.txt > /dev/null;
	$(RM) $(OUTPUT)/ab102401.cub > /dev/null;
	$(RM) $(OUTPUT)/ab102401.gml > /dev/null;
	$(APPNAME) \
	  from=$(INPUT)/ab102401.cub \
	  to=$(OUTPUT)/ab102401_180_3.gml \
	  longitudedomain=180 \
	  label= "11" \
	  linc=4 \
          sinc=3 > /dev/null;
	cat $(OUTPUT)/ab102401_180_3.gml | \
	  sed 's/\([0-9]\.[0-9]\{9\}\)\([0-9]*\)/\1/g' > \
	  $(OUTPUT)/ab102401_180_3.gml.txt;
	$(MV) $(OUTPUT)/ab102401_180_3.xsd $(OUTPUT)/ab102401_180_3.xsd.txt > /dev/null;
	$(RM) $(OUTPUT)/ab102401_180_3.gml > /dev/null;

