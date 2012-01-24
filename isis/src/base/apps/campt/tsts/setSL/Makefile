APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	to=$(OUTPUT)/output.pvl \
	sample=25 line=25 \
	append=false \
	> /dev/null;
	cat $(OUTPUT)/output.pvl | \
	  sed 's/\([0-9]*\.[0-9]\{9\}\)\([0-9]*\)/\1/g' \
	  > $(OUTPUT)/temp.txt;
	$(MV) $(OUTPUT)/temp.txt $(OUTPUT)/output.pvl;
