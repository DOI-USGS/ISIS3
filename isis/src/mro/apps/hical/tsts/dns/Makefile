APPNAME = hical

output.txt.SKIPLINES = 7
output.txt.IGNORELINES = CONF

include $(ISISROOT)/make/isismake.tsts

# The sed command cuts off the last 2 numbers from the stddev
commands:
	$(APPNAME) FROM=$(INPUT)/PSP_001446_1790_RED2_0.cub \
	TO=$(OUTPUT)/output.cub OPATH=. UNITS=DN > /dev/null;
	$(CAT) PSP_001446_1790_RED2_0.hical.log | \
	  sed 's/\(.*StdDev\[[0-9][0-9]*\.[0-9]*\)\([0-9][0-9]\)/\1/' \
	  > $(OUTPUT)/output.txt;
	$(RM) PSP_001446_1790_RED2_0.hical.log
