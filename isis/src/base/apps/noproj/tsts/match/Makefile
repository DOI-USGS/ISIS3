APPNAME = noproj
# This file exercises the match file option

PSP_001414_1780_RED4.balance.cropped.noproj.cub.TOLERANCE=0.8

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/PSP_001414_1780_RED4.balance.cropped.cub \
	match=$(INPUT)/PSP_001414_1780_RED5.balance.cropped.cub \
	to=$(OUTPUT)/PSP_001414_1780_RED4.balance.cropped.noproj.cub > /dev/null;
	$(RM) print.prt > /dev/null;
