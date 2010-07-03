APPNAME = kerneldbgen

# grep -v RunTime testTmp/temp01.db &gt; <temp>temp1.db</temp> 2&gt;&amp;1;
# head -n 19 testTmp/temp1.db &gt; <temp>temp.1.db</temp> 2&gt;&amp;1;

temp.db.txt.IGNORELINES = RunTime

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) to=$(OUTPUT)/temp.db.txt \
	  type=CK \
	  recondir=\$$mro/kernels/ck \
	  reconfilter=mro_sc_2005-12-*.bc \
	  sclk=\$$mro/kernels/sclk/MRO_SCLKSCET.00006.tsc \
	  lsk=\$$base/kernels/lsk/naif0008.tls > /dev/null;
