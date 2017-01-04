# Application test for photcorri.
# This case tests the Rolo model. It was taken from tests developed by
# Dris Takir and Tammy Becker.
#
# @history 2017-01-04 Jeannie Backer - Original version.
#
APPNAME = photcorri

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/N1702359885_1_Fin11L2-tr_400m.cub \
	           to=$(OUTPUT)/N1702359885_1_Fin11L2-tr_400m.rolo.cub \
	           photmodel=$(INPUT)/RoloFitParameters.pvl \
			   > /dev/null;
