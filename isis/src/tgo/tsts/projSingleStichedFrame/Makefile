APPNAME =

include $(ISISROOT)/make/isismake.tsts

commands:
	tgocassis2isis $(TSTARGS) \
            from=$(INPUT)/CAS-MCO-2016-11-26T22.50.27.381-PAN-00005-B1.xml \
            to=$(OUTPUT)/panframelet.cub > /dev/null;

	spiceinit $(TSTARGS) \
            from=$(OUTPUT)/panframelet.cub \
            ckp=t spkp=t > /dev/null;

	tgocassis2isis $(TSTARGS) \
            from=$(INPUT)/CAS-MCO-2016-11-26T22.50.27.381-RED-01005-B1.xml \
            to=$(OUTPUT)/redframelet.cub > /dev/null;

	spiceinit $(TSTARGS) \
            from=$(OUTPUT)/redframelet.cub \
            ckp=t spkp=t > /dev/null;

	tgocassis2isis $(TSTARGS) \
            from=$(INPUT)/CAS-MCO-2016-11-26T22.50.27.381-BLU-03005-B1.xml \
            to=$(OUTPUT)/bluframelet.cub > /dev/null;

	spiceinit $(TSTARGS) \
            from=$(OUTPUT)/bluframelet.cub \
            ckp=t spkp=t > /dev/null;

	tgocassis2isis $(TSTARGS) \
            from=$(INPUT)/CAS-MCO-2016-11-26T22.50.27.381-NIR-02005-B1.xml \
            to=$(OUTPUT)/nirframelet.cub > /dev/null;

	spiceinit $(TSTARGS) \
            from=$(OUTPUT)/nirframelet.cub \
            ckp=t spkp=t > /dev/null;

	tgocassisstitch $(TSTARGS) \
            fromlist=$(INPUT)/cubelist.lis \
            outputprefix=$(OUTPUT)/"stitched" > /dev/null;

	$(LS) $(OUTPUT)/stitched*.cub > $(OUTPUT)/stitched.lis

	mosrange $(TSTARGS) \
            fromlist=$(OUTPUT)/stitched.lis \
            to=$(OUTPUT)/stitched.map > /dev/null;

	cam2map $(TSTARGS) \
            from=$(OUTPUT)/stitched-2016-11-26T22:50:27.381.cub \
            to=$(OUTPUT)/projected.cub \
            map=$(OUTPUT)/stitched.map > /dev/null;

	tgocassisrdrgen $(TSTARGS) \
            from=$(OUTPUT)/projected.cub \
            to=$(OUTPUT)/exported.img > /dev/null;

	$(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
               $(OUTPUT)/exported.xml \
	       > $(OUTPUT)/tempLabel1.txt;
	$(SED) 's+\FSW_HEADER.*>+\FSW_HEADER>+' \
	       $(OUTPUT)/tempLabel1.txt \
	       > $(OUTPUT)/tempLabel2.txt;
	$(SED) 's+\PEHK_HEADER.*>+\PEHK_HEADER>+' \
	       $(OUTPUT)/tempLabel2.txt \
	       > $(OUTPUT)/tempLabel3.txt;
	$(SED) 's+\modification_date.*>+\modification_date>+' \
	       $(OUTPUT)/tempLabel3.txt \
	       > $(OUTPUT)/tempLabel4.txt
	$(SED) 's+\ISIS version.*<+\ISIS version.<+' \
	       $(OUTPUT)/tempLabel4.txt \
               > $(OUTPUT)/exported.xmlLabel.txt;

	$(RM) $(OUTPUT)/stitched.lis;

	$(MV) $(OUTPUT)/stitched.map $(OUTPUT)/stitched.pvl;

		# Cleanup
	$(RM) $(OUTPUT)/exported.xml;
	$(RM) $(OUTPUT)/tempLabel1.txt;
	$(RM) $(OUTPUT)/tempLabel2.txt;
	$(RM) $(OUTPUT)/tempLabel3.txt;
	$(RM) $(OUTPUT)/tempLabel4.txt;

