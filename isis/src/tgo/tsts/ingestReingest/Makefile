INGEST = tgocassis2isis
SPICE = spiceinit
RDRGEN = tgocassisrdrgen
CDIFF = cubediff

include $(ISISROOT)/make/isismake.tsts

commands:
	$(INGEST) $(TSTARGS) \
	    from=$(INPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.xml \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.cub > /dev/null;

	$(RDRGEN) $(TSTARGS) \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.cub \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.img > /dev/null;

	$(INGEST) $(TSTARGS) \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.xml \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.reingested.cub > /dev/null;

	$(CDIFF) $(TSTARGS) \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.cub \
	    from2=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.reingested.cub > /dev/null;

	catlab $(TSTARGS) \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.cub \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.pvl > /dev/null;

	catlab $(TSTARGS) \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.reingested.cub \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.reingested.pvl > /dev/null;

	$(MV) $(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.xml \
	      $(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.xmlLabel.txt;
