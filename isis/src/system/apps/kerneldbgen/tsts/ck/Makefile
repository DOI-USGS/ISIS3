# CK Test for kerneldbgen
# This test creates an output database file from the kernels in the input file
# that follow the given filter for reconstructed ck file name patterns.
# Note that the array style for this input is \("pattern1","pattern2"\)
.IGNORE:
APPNAME = kerneldbgen

# grep -v RunTime testTmp/temp01.db &gt; <temp>temp1.db</temp> 2&gt;&amp;1;
# head -n 19 testTmp/temp1.db &gt; <temp>temp.1.db</temp> 2&gt;&amp;1;

kernel.db.noTestDataPath.txt.IGNORELINES = RunTime

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) to=$(OUTPUT)/kernel.db.txt \
	  type=CK \
	  recondir=$(INPUT) \
	  reconfilter=\("mro_sc_2005-10*.bc","mro_sc_2005-12-*.bc"\) \
	  sclk=\$$mro/kernels/sclk/MRO_SCLKSCET.00006.tsc \
	  lsk=\$$base/kernels/lsk/naif0008.tls > /dev/null;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/kernel.db.txt > $(OUTPUT)/kernel.db.noTestDataPath.txt;
	$(RM) $(OUTPUT)/kernel.db.txt;
