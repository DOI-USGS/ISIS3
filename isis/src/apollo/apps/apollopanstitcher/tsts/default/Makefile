APPNAME = apollopansticher
#This test excercises the stiching of 8 apollopanoramic subscans into a single image.
#  The detection of the fiducial marks is complex and involves a RANSAC algorithm.
#  For this reason the final stiched cubes are expected to vary slightly.  To address
#  this the median and number of valid pixels of the delta cube are used to test
#  for equivalence of the output.

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) TO=$(OUTPUT)/reduced8.cub \
		   FILE_BASE = $(INPUT)/AS15_P_0177R10 \
		   MICRONS = 50 > /dev/null;
	#difference the two cubes
	fx F1=$(OUTPUT)/reduced8.cub F2=$(INPUT)/reduced8.cub TO=$(OUTPUT)/fx.cub EQUATION="F1-F2" > /dev/null;
	#use hist to find the median difference of the cubes and total valid pixels
	hist FROM=$(OUTPUT)/fx.cub To=$(OUTPUT)/hist.txt NBINS=100000000 > /dev/null;
	#form the histogram output grep the median and remove digits beyond the hundreths
	cat $(OUTPUT)/hist.txt | grep "Median" \
	  | sed 's/\([0-9][0-9]*\.[0-9][0-9]\)\([0-9][0-9]*\)/\1/g' > $(OUTPUT)/CubeStats.txt;
	#form the histogram output grep the total valid Pixels
	cat $(OUTPUT)/hist.txt | grep "Valid Pixels:" >> $(OUTPUT)/CubeStats.txt;
	#remove the intermediate files
	$(RM) $(OUTPUT)/fx.cub $(OUTPUT)/hist.txt $(OUTPUT)/reduced8.cub
