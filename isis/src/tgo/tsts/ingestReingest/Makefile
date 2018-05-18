APPNAME =

include $(ISISROOT)/make/isismake.tsts

commands:
	tgocassis2isis $(TSTARGS) \
	    from=$(INPUT)/input.xml to=$(OUTPUT)/ingested.cub > /dev/null;

	spiceinit $(TSTARGS) \
	    from=$(OUTPUT)/ingested.cub ckp=t spkp=t > /dev/null;

	tgocassisrdrgen $(TSTARGS) \
	    from=$(OUTPUT)/ingested.cub to=$(OUTPUT)/exported.img > /dev/null;

	tgocassis2isis $(TSTARGS) \
	    from=$(OUTPUT)/exported.xml to=$(OUTPUT)/reingested.cub > /dev/null;

	cubediff $(TSTARGS) \
	    from=$(OUTPUT)/ingested.cub from2=$(OUTPUT)/reingested.cub > /dev/null;

	$(RM) $(OUTPUT)/exported.xml;
	$(RM) $(OUTPUT)/exported.img;

	
