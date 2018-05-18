INGEST = tgocassis2isis
SPICE = spiceinit
RDRGEN = tgocassisrdrgen
CDIFF = cubediff

include $(ISISROOT)/make/isismake.tsts

commands:
	$(INGEST) $(TSTARGS) \
	    from=$(INPUT)/input.xml to=$(OUTPUT)/ingested.cub > /dev/null;

	$(SPICE) $(TSTARGS) \
	    from=$(OUTPUT)/ingested.cub ckp=t spkp=t > /dev/null;

	$(RDRGEN) $(TSTARGS) \
	    from=$(OUTPUT)/ingested.cub to=$(OUTPUT)/exported.img > /dev/null;

	$(INGEST) $(TSTARGS) \
	    from=$(OUTPUT)/exported.xml to=$(OUTPUT)/reingested.cub > /dev/null;

	$(CDIFF) $(TSTARGS) \
	    from=$(OUTPUT)/ingested.cub from2=$(OUTPUT)/reingested.cub > /dev/null;

	$(RM) $(OUTPUT)/exported.xml;
	$(RM) $(OUTPUT)/exported.img;

	
