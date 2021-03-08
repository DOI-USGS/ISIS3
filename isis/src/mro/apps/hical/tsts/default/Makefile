APPNAME = hical

output.txt.SKIPLINES = 7
output.txt.IGNORELINES = CONF

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/PSP_001446_1790_RED2_0.cub \
	TO=$(OUTPUT)/output.cub OPATH=. > /dev/null;
	cat  PSP_001446_1790_RED2_0.hical.log \
          | sed 's/\([0-9][0-9]*\.[0-9]\{12\}\)\([0-9][0-9]*\)/\1/g' \
          > $(OUTPUT)/output.txt;
	$(RM) PSP_001446_1790_RED2_0.hical.log
	$(APPNAME) FROM=$(INPUT)/PSP_001446_1790_RED2_0_spiceinit.cub \
	TO=$(OUTPUT)/output_spiceinit.cub OPATH=. > /dev/null;
	$(RM) PSP_001446_1790_RED2_0.hical.log

