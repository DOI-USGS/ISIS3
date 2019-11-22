APPNAME = cisscal

include $(ISISROOT)/make/isismake.tsts


commands:
	-$(APPNAME) from=$(INPUT)/N1605851822_1.cub \
	to=$(OUTPUT)/N1605851822_1.truth.cub >& $(OUTPUT)/error.txt;
	$(RM) print.prt > /dev/null;
