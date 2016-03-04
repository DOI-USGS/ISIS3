APPNAME = isisminer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) config=$(INPUT)/stereopair_intersect_centroid.conf \
	  parameters="inputdir:$(INPUT)@outputdir:$(OUTPUT)" > /dev/null;
	# Trim precision to 10 places past decimal
	for csv in `ls $(OUTPUT)`; do \
	  $(CAT) $(OUTPUT)/$$csv \
 	  | $(SED) 's/\([0-9]*\.[0-9]\{10\}\)[0-9]*/\1/g' \
 	  > $(OUTPUT)/$${csv}.temp; \
 	  $(MV) $(OUTPUT)/$${csv}.temp $(OUTPUT)/$${csv} > /dev/null; \
 	  $(RM) $(OUTPUT)/$${csv}.temp > /dev/null; \
	done;
