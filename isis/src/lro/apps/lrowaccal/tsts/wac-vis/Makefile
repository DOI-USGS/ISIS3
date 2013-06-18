APPNAME = lrowaccal
FILE=wac00002cf4

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/${FILE}.vis.odd.cub \
	  to=$(OUTPUT)/${FILE}.vis.odd.cal.cub > /dev/null;
	$(APPNAME) from=$(INPUT)/${FILE}.vis.even.cub \
          to=$(OUTPUT)/${FILE}.vis.even.cal.cub > /dev/null;
	$(APPNAME) from=$(INPUT)/${FILE}.vis.even.cub+4 \
          to=$(OUTPUT)/${FILE}.vis.even.cal.band4.cub > /dev/null;
