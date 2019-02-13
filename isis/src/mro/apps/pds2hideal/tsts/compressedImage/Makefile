# This tests the following conditions:
#
# -- We can export a compressed image.
# -- The tables in the compressed image do not have the keyword RECORD_BYTES.
# -- When exported, the compressed and uncompressed input data produce the same output.
APPNAME = pds2hideal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/PSP_009492_1280_RED.NOPROJ.lbl \
	  to=$(OUTPUT)/uncompressed.cub > /dev/null;
	$(APPNAME) FROM=$(INPUT)/PSP_009492_1280_RED_COMPRESSED.NOPROJ.IMG \
	  to=$(OUTPUT)/compressed.cub > /dev/null;
	cubediff from=$(OUTPUT)/uncompressed.cub from2=$(OUTPUT)/compressed.cub \
	  to=$(OUTPUT)/comparisonTruth.txt > /dev/null;

