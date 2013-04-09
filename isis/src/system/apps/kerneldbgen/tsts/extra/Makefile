APPNAME = kerneldbgen

include $(ISISROOT)/make/isismake.tsts

# First, test a successful run. Then test errors with bad db files.
commands:
	export INPUT="$(INPUT)"; \
	$(APPNAME) TO=$(OUTPUT)/crism_kernels.db.pvl recondir='$$mro/kernels/ck' \
	  reconfilter=mro_crm_psp_??????_??????.bc \
	  sclk='$$mro/kernels/sclk/MRO_SCLKSCET.?????.65536.tsc' \
	  lsk='$$base/kernels/lsk/naif????.tls' \
	  extra='($$mro/kernels/fk/mro_v15.tf,$$mro/kernels/fk/kernels.????.db)' > /dev/null; \
	\
	$(APPNAME) TO=$(OUTPUT)/crism_kernels.????.db.pvl recondir='$$mro/kernels/ck' \
	  reconfilter=mro_crm_psp_??????_??????.bc \
	  sclk='$$mro/kernels/sclk/MRO_SCLKSCET.?????.65536.tsc' \
	  lsk='$$base/kernels/lsk/naif????.tls' \
	  extra='$$INPUT/kernels_badkeywordvalue.db' >& $(OUTPUT)/kernels_badkeywordvalue.txt \
	  || true; \
	\
	$(APPNAME) TO=$(OUTPUT)/crism_kernels.????.db.pvl recondir='$$mro/kernels/ck' \
	  reconfilter=mro_crm_psp_??????_??????.bc \
	  sclk='$$mro/kernels/sclk/MRO_SCLKSCET.?????.65536.tsc' \
	  lsk='$$base/kernels/lsk/naif????.tls' \
	  extra='$$INPUT/kernels_blank.db' >& $(OUTPUT)/kernels_blank.txt || true; \
	\
	$(APPNAME) TO=$(OUTPUT)/crism_kernels.????.db.pvl recondir='$$mro/kernels/ck' \
	  reconfilter=mro_crm_psp_??????_??????.bc \
	  sclk='$$mro/kernels/sclk/MRO_SCLKSCET.?????.65536.tsc' \
	  lsk='$$base/kernels/lsk/naif????.tls' \
	  extra='$$INPUT/kernels_misnamedgroup.db' >& $(OUTPUT)/kernels_misnamedgroup.txt \
	  || true; \
	\
	$(APPNAME) TO=$(OUTPUT)/crism_kernels.????.db.pvl recondir='$$mro/kernels/ck' \
	  reconfilter=mro_crm_psp_??????_??????.bc \
	  sclk='$$mro/kernels/sclk/MRO_SCLKSCET.?????.65536.tsc' \
	  lsk='$$base/kernels/lsk/naif????.tls' \
	  extra='$$INPUT/kernels_misnamedkeyword.db' >& $(OUTPUT)/kernels_misnamedkeyword.txt \
	  || true; \
	\
	$(APPNAME) TO=$(OUTPUT)/crism_kernels.????.db.pvl recondir='$$mro/kernels/ck' \
	  reconfilter=mro_crm_psp_??????_??????.bc \
	  sclk='$$mro/kernels/sclk/MRO_SCLKSCET.?????.65536.tsc' \
	  lsk='$$base/kernels/lsk/naif????.tls' \
	  extra='$$INPUT/kernels_nogroup.db' >& $(OUTPUT)/kernels_nogroup.txt || true; \
	\
	$(APPNAME) TO=$(OUTPUT)/crism_kernels.????.db.pvl recondir='$$mro/kernels/ck' \
	  reconfilter=mro_crm_psp_??????_??????.bc \
	  sclk='$$mro/kernels/sclk/MRO_SCLKSCET.?????.65536.tsc' \
	  lsk='$$base/kernels/lsk/naif????.tls' \
	  extra='$$INPUT/kernels_nokeyword.db' >& $(OUTPUT)/kernels_nokeyword.txt || true;
      
	$(SED) 's+mro/kernels/sclk/MRO_SCLKSCET.*tsc+mro/kernels/sclk/MRO_SCLKSCET.tsc+g' $(OUTPUT)/crism_kernels.db.pvl > $(OUTPUT)/crism_kernels.0001.db.pvl;
	rm $(OUTPUT)/crism_kernels.db.pvl;