# Application test for OSIRIS-REx mapping camera ingestion.
#
# @history 2016-09-09 Jeannie Backer - Original version.
#
include $(ISISROOT)/make/isismake.tsts

commands:
	ocams2isis $(TSTARGS)                      \
	           from=$(INPUT)/D19030320000.fits \
	           to=$(OUTPUT)/D19030320000.cub   \
	           >& /dev/null;
