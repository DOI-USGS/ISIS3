APPNAME = vikclean

vikcleanTruth.cub.TOLERANCE = 1.0

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/f348b26.cub to=$(OUTPUT)/vikcleanTruth.cub \
          remove=yes > /dev/null;
	# There are 9x9 groups of pixels that result in slightly different DN's on the MAC
        # The FX command converts some row to HRS in oder to avoid OS specific truth data
	fx F1=$(OUTPUT)/vikcleanTruth.cub to=$(OUTPUT)/temp.cub EQUATION="F1/( (1-(line>130)*(line<154)) * (1-(line>260)*(line<282)) * (1-(line>390)*(line<415)) * (1-(line>520)*(line<540)) * (1-(line>649)*(line<670)) * (1-(line>779)*(line<798)) * (1-(line>908)*(line<926)) )" > /dev/null ;
	$(RM) $(OUTPUT)/vikcleanTruth.cub > /dev/null;
	$(MV) $(OUTPUT)/temp.cub $(OUTPUT)/vikcleanTruth.cub > /dev/null;

	  
