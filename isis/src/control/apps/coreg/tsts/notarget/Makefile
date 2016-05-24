APPNAME = coreg

# tail -1330 testTmp/tempcnet1.txt &gt; <temp> tempcnet.txt</temp> 2&gt;&amp;1 > /dev/null;

# This test has been set up so that the all but one points are ignored due to goodness of fit.
# Ignored points are not written to the flat file.

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/nullTarget.cub \
	           match=$(INPUT)/CRU_000004_1200_RED4_0.equi.cub \
	           deffile=$(INPUT)/coregtemp.def \
	           to=$(OUTPUT)/coregTruth.cub \
	           onet=$(OUTPUT)/coregTruthCnet.net \
	           flatfile=$(OUTPUT)/coregTruthFlatFile.txt \
	           interp=n \
			   >& /dev/null;
