APPNAME = noproj
#This file exercises the default options

# Maintains accuracy across systems to 3 sig figs
PSP_001414_1780_RED5.balance.cropped.noproj.cub.TOLERANCE = 1.1

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/PSP_001414_1780_RED5.balance.cropped.cub \
	to=$(OUTPUT)/PSP_001414_1780_RED5.balance.cropped.noproj.cub > /dev/null;
	$(RM) print.prt > /dev/null;
