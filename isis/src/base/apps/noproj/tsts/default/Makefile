APPNAME = noproj
#This file exercises the default options
# Also tests that the AlphaCube group is removed when noproj is run.
#
# NOTE: editlab is being used to get rid of the "MatchedCube" keyword.
# pvldiff does not seem to be ignoring file paths for relative filepaths.

# Maintains accuracy across systems to 3 sig figs
PSP_001414_1780_RED5.balance.cropped.noproj.cub.TOLERANCE = 1.1

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/PSP_001414_1780_RED5.balance.cropped.cub \
	to=$(OUTPUT)/PSP_001414_1780_RED5.balance.cropped.noproj.cub > /dev/null;
	catlab from=$(OUTPUT)/PSP_001414_1780_RED5.balance.cropped.noproj.cub \
	  > $(OUTPUT)/PSP_001414_1780_RED5.balance.cropped.noproj.pvl;
	editlab from=$(OUTPUT)/PSP_001414_1780_RED5.balance.cropped.noproj.pvl \
	  options=delkey \
	  grpname=Instrument \
	  keyword=MatchedCube;
	$(RM) print.prt > /dev/null;
