APPNAME = lrowaccal
FILE=wac0001832b

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/${FILE}.uv.odd.cub \
	  to=$(OUTPUT)/${FILE}.uv.odd.cal.cub > /dev/null;
	$(APPNAME) from=$(INPUT)/${FILE}.uv.even.cub \
          to=$(OUTPUT)/${FILE}.uv.even.cal.cub > /dev/null;
