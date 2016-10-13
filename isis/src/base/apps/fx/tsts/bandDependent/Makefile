# This tests fx with a band-dependent cube when making calculations on camera
# properties. In this test, we use a band-dependent THEMIS-IR cube and use the
# various camera functions.
#
# 2016-10-13 Ian Humphrey - Original version. References #1301.
APPNAME = fx

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) \
	  f1=$(INPUT)/themisIR.crop20x20.cub \
	  to=$(OUTPUT)/radiusTruth.cub \
	  equation=radius\(f1\) \
	  > /dev/null;
	$(APPNAME) \
	  f1=$(INPUT)/themisIR.crop20x20.cub \
	  to=$(OUTPUT)/phacTruth.cub \
          equation=phac\(f1\) \
	  > /dev/null;
	$(APPNAME) \
	  f1=$(INPUT)/themisIR.crop20x20.cub \
	  to=$(OUTPUT)/inacTruth.cub \
          equation=inac\(f1\) \
	  > /dev/null;
	$(APPNAME) \
	  f1=$(INPUT)/themisIR.crop20x20.cub \
	  to=$(OUTPUT)/emacTruth.cub \
          equation=emac\(f1\) \
	  > /dev/null;
	$(APPNAME) \
	  f1=$(INPUT)/themisIR.crop20x20.cub \
	  to=$(OUTPUT)/phaTruth.cub \
          equation=pha\(f1\) \
	  > /dev/null;
	$(APPNAME) \
	  f1=$(INPUT)/themisIR.crop20x20.cub \
	  to=$(OUTPUT)/inaTruth.cub \
          equation=ina\(f1\) \
	  > /dev/null;
	$(APPNAME) \
	  f1=$(INPUT)/themisIR.crop20x20.cub \
	  to=$(OUTPUT)/emaTruth.cub \
          equation=ema\(f1\) \
	  > /dev/null;
	$(APPNAME) \
	  f1=$(INPUT)/themisIR.crop20x20.cub \
	  to=$(OUTPUT)/phalTruth.cub \
          equation=phal\(f1\) \
	  > /dev/null;
	$(APPNAME) \
	  f1=$(INPUT)/themisIR.crop20x20.cub \
	  to=$(OUTPUT)/inalTruth.cub \
          equation=inal\(f1\) \
	  > /dev/null;
	$(APPNAME) \
	  f1=$(INPUT)/themisIR.crop20x20.cub \
	  to=$(OUTPUT)/emalTruth.cub \
          equation=emal\(f1\) \
	  > /dev/null;
	$(APPNAME) \
	  f1=$(INPUT)/themisIR.crop20x20.cub \
	  to=$(OUTPUT)/latTruth.cub \
          equation=lat\(f1\) \
	  > /dev/null;
	$(APPNAME) \
	  f1=$(INPUT)/themisIR.crop20x20.cub \
	  to=$(OUTPUT)/lonTruth.cub \
          equation=lon\(f1\) \
	  > /dev/null;
	$(APPNAME) \
	  f1=$(INPUT)/themisIR.crop20x20.cub \
	  to=$(OUTPUT)/resTruth.cub \
          equation=res\(f1\) \
	  > /dev/null;
	#$(APPNAME) \
	#  -batchlist=$(INPUT)/batchList.txt \
	#  f1=$(INPUT)/\$$\1.cub+\$$\2 \
	#  to=$(OUTPUT)/\$$\1_band\$$\2_temp.cub \
	#  equation=radius\(f1\) \
	#  > /dev/null;
	#$(LS) $(OUTPUT)/*temp.cub > $(OUTPUT)/cubeit_temp.txt;
	#cubeit \
	#  fromlist=$(INPUT)/cubeitList.txt \
	#  to=$(OUTPUT)/radiusTruth2.cub \
	#  > /dev/null;
	$(RM) $(OUTPUT)/*temp*;
