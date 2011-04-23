APPNAME = caminfo

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/input.cub $(OUTPUT)/input.cub;
	$(APPNAME) FROM=$(OUTPUT)/input.cub \
	TO=$(OUTPUT)/temp.txt FORMAT=flat APPEND=false \
	STATISTICS=true CAMSTATS=true GEOMETRY=true spice=true > /dev/null;
	cat $(OUTPUT)/temp.txt | \
	  sed 's/\([0-9]*\.[0-9]\{4\}\)\([0-9]*\)/\1/g' | \
	  sed 's/\([a-z]*\,\)\([0-9]\.[0-9]\.[0-9]\.[0-9] | [0-9]\{4\}\-[0-9]\{2\}\-[0-9]\{2\}\)/\1/g' \
	  > $(OUTPUT)/truth.txt;
	$(RM) $(OUTPUT)/temp.txt;
	$(RM) $(OUTPUT)/input.cub;
