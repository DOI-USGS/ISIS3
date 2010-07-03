APPNAME = noproj
#This file exercises the user sum option

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/PSP_001414_1780_RED5.balance.cropped.cub \
	to=$(OUTPUT)/PSP_001414_1780_RED5.balance.cropped.noproj.sum2.cub  \
	source=fromuser sum=2 > /dev/null;
	$(RM) print.prt > /dev/null;
