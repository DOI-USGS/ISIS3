APPNAME = lrowacpho
FILE=wac0000983c

wac0000983c.vis.even.pho.cub.TOLERANCE = 0.0000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) phopar=${INPUT}/test.pvl from=$(INPUT)/${FILE}.uv.odd.cal.cub \
	  to=$(OUTPUT)/${FILE}.uv.odd.pho.cub > /dev/null;
	$(APPNAME) phopar=${INPUT}/test.pvl from=$(INPUT)/${FILE}.uv.even.cal.cub \
          to=$(OUTPUT)/${FILE}.uv.even.pho.cub > /dev/null;
	$(APPNAME) phopar=${INPUT}/test.pvl from=$(INPUT)/${FILE}.vis.odd.cal.cub \
          to=$(OUTPUT)/${FILE}.vis.odd.pho.cub > /dev/null;
	$(APPNAME) phopar=${INPUT}/test.pvl from=$(INPUT)/${FILE}.vis.even.cal.cub \
          to=$(OUTPUT)/${FILE}.vis.even.pho.cub > /dev/null;

