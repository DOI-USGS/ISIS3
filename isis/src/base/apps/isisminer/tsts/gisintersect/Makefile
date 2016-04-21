APPNAME = isisminer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/MercuryQuadGeoms.pvl . > /dev/null;
	$(APPNAME) config=$(INPUT)/gisintersect_test.conf \
         parameters="inputdir:$(INPUT)@outputdir:$(OUTPUT)"  > /dev/null;
	$(RM) MercuryQuadGeoms.pvl > /dev/null; 
