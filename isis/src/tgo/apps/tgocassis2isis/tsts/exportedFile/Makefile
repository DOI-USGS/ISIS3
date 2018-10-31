# This test checks to see if images exported from tgocassisrdrgen can be 
# re-imported using tgocassis2isis. 
#
# history 2018-05-16 Kristin Berry - Original version. References # 5418

APPNAME = tgocassis2isis 

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/CAS-MCO-2016-11-26T22.50.30.181-RED-01012-B1_rdrgen.xml \
	   to=$(OUTPUT)/reingested_unproj.cub > /dev/null;
	catlab from=$(OUTPUT)/reingested_unproj.cub \
	  to=$(OUTPUT)/reingested_unproj.pvl > /dev/null;


# projected
	$(APPNAME) from=$(INPUT)/CAS-MCO-2016-11-26T22.50.30.181-RED-01012-B1_proj_rdrgen.xml \
	   to=$(OUTPUT)/reingested_proj.cub > /dev/null;
	catlab from=$(OUTPUT)/reingested_proj.cub \
	  to=$(OUTPUT)/reingested_proj.pvl > /dev/null;

# add mosaic test later. 
