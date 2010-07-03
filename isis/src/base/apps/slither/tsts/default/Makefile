APPNAME = slither

include $(ISISROOT)/make/isismake.tsts

PSP_001392_2490_IR11_BG13.slither.txt.IGNORELINES = RunDate: FROM CNETFILE

commands:
	$(APPNAME) from=$(INPUT)/PSP_001392_2490_IR11.balance.cub \
	           to=$(OUTPUT)/PSP_001392_2490_IR11.slither.cub \
	           control=$(INPUT)/PSP_001392_2490_IR11_BG13.net \
	           results=$(OUTPUT)/PSP_001392_2490_IR11_BG13.slither.txt \
	           direction=reverse spline=cubic \
	           lineoffset=0 sampleoffset = 0 \
	           interp=cubicconvolution > /dev/null
