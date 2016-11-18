# This tests that the band selection work properly for pixel2map.
# In this test, we will select band 1.
#
# @history 2016-11-18 Ian Humphrey - Original version. Adapated from #4520 user test plan.
#                         References #4520.
APPNAME = pixel2map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/CM_1540484927_1_001.ir.cub+1 \
	  to=$(OUTPUT)/band1.cub \
  	  > /dev/null;
