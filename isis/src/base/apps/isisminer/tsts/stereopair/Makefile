APPNAME = isisminer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) config=$(INPUT)/stereopair_test.conf \
	  parameters="inputdir:$(INPUT)@outputdir:$(OUTPUT)" > /dev/null;
	# Trim precision to 8 places past decimal
	for csv in `ls $(OUTPUT)`; do \
	  $(CAT) $(OUTPUT)/$$csv \
	  | $(SED) 's/\([0-9]*\.[0-9]\{8\}\)[0-9]*/\1/g' \
	  > $(OUTPUT)/$${csv}.temp; \
	  $(MV) $(OUTPUT)/$${csv}.temp $(OUTPUT)/$${csv} > /dev/null; \
	  $(RM) $(OUTPUT)/$${csv}.temp > /dev/null; \
	done;