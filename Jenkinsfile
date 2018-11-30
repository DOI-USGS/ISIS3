properties([pipelineTriggers([githubPush()])])

def nodes = [:]

nodes["isis-fedora-25"] = {
    node("isis-fedora-25") {
        withEnv(["ISISROOT=" + "${workspace}" + "/build/", "ISIS3TESTDATA=/usgs/cpkgs/isis3/testData/", "ISIS3DATA=/usgs/cpkgs/isis3/data/"]) {
            stage ("Fedora 25") {
                git branch: 'dev', url: 'https://github.com/USGS-Astrogeology/ISIS3.git'
                sh """
                    git clone https://github.com/abseil/googletest.git gtest
                    sed -i "s|usgs-astrogeology|http://astro-bin.wr.usgs.gov/conda-usgs-astrogeology|" environment.yml
                    sed -i "s|conda-forge|http://astro-bin.wr.usgs.gov/conda-forge|" environment.yml
                    sed -i "s|defaults|http://astro-bin.wr.usgs.gov/conda|" environment.yml
                    conda env create -n isis3 -f environment.yml
                    source activate isis3
                    mkdir -p ./install ./build && cd build
                    cmake -GNinja -DJP2KFLAG=OFF -Dpybindings=OFF -DCMAKE_INSTALL_PREFIX=../install -Disis3Data=/usgs/cpkgs/isis3/data -Disis3TestData=/usgs/cpkgs/isis3/testData ../isis
                    set +e
                    ninja -j8 && ninja install
                    source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
                    ctest -V -R _unit_ --timeout 500
                    ctest -V -R _app_ --timeout 500
                    ctest -V -R _module_ --timeout 500
                """
            }
        }
    }
}

nodes["isis-centos-7"] = {
    node("isis-centos-7") {
        withEnv(["ISISROOT=" + "${workspace}" + "/build/", "ISIS3TESTDATA=/usgs/cpkgs/isis3/testData/", "ISIS3DATA=/usgs/cpkgs/isis3/data/"]) {
            stage ("CentOS 7") {
                git branch: 'dev', url: 'https://github.com/USGS-Astrogeology/ISIS3.git'
                sh """
                    git clone https://github.com/abseil/googletest.git gtest
                    sed -i "s|usgs-astrogeology|http://astro-bin.wr.usgs.gov/conda-usgs-astrogeology|" environment.yml
                    sed -i "s|conda-forge|http://astro-bin.wr.usgs.gov/conda-forge|" environment.yml
                    sed -i "s|defaults|http://astro-bin.wr.usgs.gov/conda|" environment.yml
                    conda env create -n isis3 -f environment.yml
                    source activate isis3
                    mkdir -p ./install ./build && cd build
                    cmake -GNinja -DJP2KFLAG=OFF -Dpybindings=OFF -DCMAKE_INSTALL_PREFIX=../install -Disis3Data=/usgs/cpkgs/isis3/data -Disis3TestData=/usgs/cpkgs/isis3/testData ../isis
                    set +e
                    ninja -j8 && ninja install
                    source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
                    ctest -V -R _unit_ --timeout 500
                    ctest -V -R _app_ --timeout 500
                    ctest -V -R _module_ --timeout 500
                """
            }
        }
    }
}

nodes["isis-debian-9"] = {
    node("isis-debian-9") {
        withEnv(["ISISROOT=" + "${workspace}" + "/build/", "ISIS3TESTDATA=/usgs/cpkgs/isis3/testData/", "ISIS3DATA=/usgs/cpkgs/isis3/data/"]) {
            stage ("Debian 9") {
                git branch: 'dev', url: 'https://github.com/USGS-Astrogeology/ISIS3.git'
                sh """
                    git clone https://github.com/abseil/googletest.git gtest
                    sed -i "s|usgs-astrogeology|http://astro-bin.wr.usgs.gov/conda-usgs-astrogeology|" environment.yml
                    sed -i "s|conda-forge|http://astro-bin.wr.usgs.gov/conda-forge|" environment.yml
                    sed -i "s|defaults|http://astro-bin.wr.usgs.gov/conda|" environment.yml
                    conda env create -n isis3 -f environment.yml
                    source activate isis3
                    mkdir -p ./install ./build && cd build
                    cmake -GNinja -DJP2KFLAG=OFF -Dpybindings=OFF -DCMAKE_INSTALL_PREFIX=../install -Disis3Data=/usgs/cpkgs/isis3/data -Disis3TestData=/usgs/cpkgs/isis3/testData ../isis
                    set +e
                    ninja -j8 && ninja install
                    source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
                    ctest -V -R _unit_ --timeout 500
                    ctest -V -R _app_ --timeout 500
                    ctest -V -R _module_ --timeout 500
                """
            }
        }
    }
}

nodes["isis-ubuntu-1804"] = {
    node("isis-ubuntu-1804") {
        withEnv(["ISISROOT=" + "${workspace}" + "/build/", "ISIS3TESTDATA=/usgs/cpkgs/isis3/testData/", "ISIS3DATA=/usgs/cpkgs/isis3/data/"]) {
            stage ("Ubuntu 18.04") {
                git branch: 'dev', url: 'https://github.com/USGS-Astrogeology/ISIS3.git'
                sh """
                    git clone https://github.com/abseil/googletest.git gtest
                    sed -i "s|usgs-astrogeology|http://astro-bin.wr.usgs.gov/conda-usgs-astrogeology|" environment.yml
                    sed -i "s|conda-forge|http://astro-bin.wr.usgs.gov/conda-forge|" environment.yml
                    sed -i "s|defaults|http://astro-bin.wr.usgs.gov/conda|" environment.yml
                    conda env create -n isis3 -f environment.yml
                    source activate isis3
                    mkdir -p ./install ./build && cd build
                    cmake -GNinja -DJP2KFLAG=OFF -Dpybindings=OFF -DCMAKE_INSTALL_PREFIX=../install -Disis3Data=/usgs/cpkgs/isis3/data -Disis3TestData=/usgs/cpkgs/isis3/testData ../isis
                    set +e
                    ninja -j8 && ninja install
                    source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
                    ctest -V -R _unit_ --timeout 500
                    ctest -V -R _app_ --timeout 500
                    ctest -V -R _module_ --timeout 500
                """
            }
        }
    }
}

parallel nodes
