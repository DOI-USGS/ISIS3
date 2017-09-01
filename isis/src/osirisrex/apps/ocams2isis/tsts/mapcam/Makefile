# Application test for OSIRIS-REx mapping camera ingestion.
#
# @history 2016-09-09 Jeannie Backer - Original version.
#
APPNAME = ocams2isis
include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) $(TSTARGS)                      \
	           from=$(INPUT)/D19030320000.fits \
	           to=$(OUTPUT)/D19030320000.cub   \
	           >& /dev/null;
