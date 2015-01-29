# This test will print errors thrown by the application.
.IGNORE:
APPNAME = thmnoseam

include $(ISISROOT)/make/isismake.tsts

commands:
	echo -e "Error Test: INEVEN cube is not THEMIS VIS " > $(OUTPUT)/error_temp.txt;
	$(CP) $(INPUT)/*.cub $(OUTPUT)
	$(APPNAME) INEVEN=$(INPUT)/I00831002RDR.cropped.cub \
	INODD=$(INPUT)/I00831002RDR.cropped.cub \
	OUTEVEN=$(OUTPUT)/out.even.cub \
	OUTODD=$(OUTPUT)/out.odd.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;
	echo -e "\nError Test: INODD cube is not THEMIS VIS " >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) INEVEN=$(INPUT)/V06944003RDR.even.cub \
	INODD=$(INPUT)/I00831002RDR.cropped.cub \
	OUTEVEN=$(OUTPUT)/out.even.cub \
	OUTODD=$(OUTPUT)/out.odd.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;
	echo -e "\nError Test: INEVEN cube is not even " >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) INEVEN=$(INPUT)/V06944003RDR.odd.cub \
	INODD=$(INPUT)/V06944003RDR.odd.cub \
	OUTEVEN=$(OUTPUT)/out.even.cub \
	OUTODD=$(OUTPUT)/out.odd.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;
	echo -e "\nError Test: INODD cube is not odd " >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) INEVEN=$(INPUT)/V06944003RDR.even.cub \
	INODD=$(INPUT)/V06944003RDR.even.cub \
	OUTEVEN=$(OUTPUT)/out.even.cub \
	OUTODD=$(OUTPUT)/out.odd.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;

	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	$(RM) $(OUTPUT)/error_temp.txt;
	$(RM) $(OUTPUT)/*.cub;
