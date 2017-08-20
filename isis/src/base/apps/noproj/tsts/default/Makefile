APPNAME = noproj
#This file exercises the default options
# Also tests that the AlphaCube group is removed when noproj is run.

# Maintains accuracy across systems to 3 sig figs
PSP_001414_1780_RED5.balance.cropped.noproj.cub.TOLERANCE = 1.1

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/PSP_001414_1780_RED5.balance.cropped.cub \
	to=$(OUTPUT)/PSP_001414_1780_RED5.balance.cropped.noproj.cub > /dev/null;
	catlab from=$(OUTPUT)/PSP_001414_1780_RED5.balance.cropped.noproj.cub \
	  > $(OUTPUT)/PSP_001414_1780_RED5.balance.cropped.noproj.pvl.tmp;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/PSP_001414_1780_RED5.balance.cropped.noproj.pvl.tmp \
	  > $(OUTPUT)/PSP_001414_1780_RED5.balance.cropped.noproj.pvl;
	$(RM) $(OUTPUT)/PSP_001414_1780_RED5.balance.cropped.noproj.pvl.tmp;
	$(RM) print.prt > /dev/null;
