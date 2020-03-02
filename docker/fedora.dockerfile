# vim: set ft=dockerfile:

FROM usgsastro/fedora:latest

SHELL ["/bin/bash", "-lc"]

RUN dnf makecache &&        \
    dnf install -y          \
        gcc                 \
        gcc-c++             \
        git                 \
        libnsl              \
        mesa-libGL-devel    \
    &&                      \
    dnf clean all

RUN curl -sLo /tmp/miniconda.sh https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh &&    \
    chmod +x /tmp/miniconda.sh &&                                                                           \
    /tmp/miniconda.sh -b -p /opt/conda &&                                                                   \
    rm /tmp/miniconda.sh &&                                                                                 \
    echo '. /opt/conda/etc/profile.d/conda.sh' >> /etc/bashrc

RUN git config --system http.sslCAInfo /etc/ssl/certs/ca-bundle.crt &&              \ 
    echo 'ssl_verify: /etc/ssl/certs/ca-bundle.crt' > /opt/conda/.condarc &&        \
    echo 'always_yes: true' >> /opt/conda/.condarc &&                               \
    conda update conda

RUN useradd -m conda
USER conda
WORKDIR /home/conda

CMD ["/bin/bash", "-l"]
