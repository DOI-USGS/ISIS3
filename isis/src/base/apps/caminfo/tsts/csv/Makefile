APPNAME = caminfo

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/input.cub $(OUTPUT)/input.cub;
	$(APPNAME) FROM=$(OUTPUT)/input.cub \
	TO=$(OUTPUT)/temp.txt FORMAT=flat APPEND=false \
	STATISTICS=true CAMSTATS=true GEOMETRY=true spice=true > /dev/null;
	# Remove the trailing decimal place on numbers
	# Replace the caminfo,version number | date with "caminfo,version | date"
	cat $(OUTPUT)/temp.txt | \
	  sed 's/\([0-9]*\.[0-9]\{4\}\)\([0-9]*\)/\1/g' | \
	  sed 's/\([a-z]*\,\)\([0-9]\.[0-9]*\.[0-9]*.*| [0-9]\{4\}\-[0-9]\{2\}\-[0-9]\{2\}\)/\1version | date/g' \
	  > $(OUTPUT)/truth.txt;
	$(RM) $(OUTPUT)/temp.txt;
	$(RM) $(OUTPUT)/input.cub;
