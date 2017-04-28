APPNAME = lronacpho

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) usedem=false \
	  phopar=${INPUT}/LROC_Empirical_2014.pvl \
	  from=$(INPUT)/M143947267L.cal.echo.crop.cub \
	  to=$(OUTPUT)/M143947267L.cal.echo.crop.nodtmpho.test.cub > /dev/null;
