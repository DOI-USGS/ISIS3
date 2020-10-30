# vim: ft=dockerfile:

FROM usgsastro/miniconda3

RUN conda update conda && \
    conda install python=3.6 && \
    conda config --add channels conda-forge && \
    conda config --add channels usgs-astrogeology

RUN conda install -c usgs-astrogeology isis

RUN apt-get update && \
    apt-get install libgl1 -y && \
    rm -rf /var/lib/apt/lists/*

RUN bash -lc 'python "$CONDA_PREFIX/scripts/isisVarInit.py"'

