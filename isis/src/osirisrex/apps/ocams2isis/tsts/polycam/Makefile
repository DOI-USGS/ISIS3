# Application test for OSIRIS-REx poly camera ingestion.
#
# @history 2016-09-09 Jeannie Backer - Original version. 
#
APPNAME = ocams2isis
include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) $(TSTARGS)                                               \
	           from=$(INPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.fits \
	           to=$(OUTPUT)/2019-01-13T23_35_59.000_PCAM_L2b_V001.cub   \
	           >& /dev/null;
