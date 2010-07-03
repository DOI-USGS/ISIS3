APPNAME = mar10restore

include $(ISISROOT)/make/isismake.tsts

commands:
	if [ `$(APPNAME) from=$(INPUT)/isisTruth.cub to=$(OUTPUT)/fail.cub \
	  >& $(OUTPUT)/output.txt` ]; \
	then \
	  true; \
	fi;
	$(CAT) $(OUTPUT)/output.txt | $(SED) s_\\[.\*\]_\[input/isisTruth.cub\]_ \
	 | $(SED) 's_ in .*__' > $(OUTPUT)/error.txt;
	rm -f $(OUTPUT)/output.txt > /dev/null;
