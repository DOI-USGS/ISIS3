APPNAME = crism2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/t1865_mrrde_70n185_0256_1.lbl \
	to=$(OUTPUT)/mrdrtruth.cub \
	  | $(SED) 's+.*% Processed.*#+#+' \
	  | grep -v "Processed" \
	  | grep -v "Working" \
	  | grep -v "Importing" \
	  >& $(OUTPUT)/resultsLog.pvl;
	catlab from=$(OUTPUT)/mrdrtruth.cub \
	to=$(OUTPUT)/mrdrtruth.pvl > /dev/null;
