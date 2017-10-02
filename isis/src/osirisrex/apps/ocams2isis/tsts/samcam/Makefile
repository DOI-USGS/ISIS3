# Application test for OSIRIS-REx sample camera ingestion.
#
# NOTE: this test should be updated when a better samcam image is acquired.
#
# @history 2016-09-09 Jeannie Backer - Original version.
#
APPNAME = ocams2isis
include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) $(TSTARGS)                                           \
	           from=$(INPUT)/20141111T202650_SCAM_L2_V001_SCAM.fits \
	           to=$(OUTPUT)/20141111T202650_SCAM_L2_V001_SCAM.cub   \
	           >& /dev/null;
