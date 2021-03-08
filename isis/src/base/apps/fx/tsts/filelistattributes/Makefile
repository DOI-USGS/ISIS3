APPNAME = fx

include $(ISISROOT)/make/isismake.tsts

commands:
	$(ECHO) $(INPUT)/vims1+1 > $(OUTPUT)/vims.lis;
	$(ECHO) $(INPUT)/vims2+100 >> $(OUTPUT)/vims.lis;
	$(APPNAME) fromlist=$(OUTPUT)/vims.lis \
	mode=list \
	equation=f1*f2 \
	to=$(OUTPUT)/f1Multf2.cub > /dev/null;
	$(RM) $(OUTPUT)/vims.lis
