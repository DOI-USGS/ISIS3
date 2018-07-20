pipeline { 
    agent {
        docker {
            label 'cmake'
            image 'chrisryancombs/docker_isis'
            args  '''\
                    -v /usgs/cpkgs/isis3/data:/usgs/cpkgs/isis3/data \
                    -v /usgs/cpkgs/isis3/testData:/usgs/cpkgs/isis3/testData\
                  '''  
        }
    }
    environment {
        ISISROOT="${workspace}" + "/build/"
        ISIS3TESTDATA="/usgs/cpkgs/isis3/testData/"
        ISIS3DATA="/usgs/cpkgs/isis3/data/"
    }
    stages {
        stage('Config') { 
            steps { 
                sh """
                    conda env create -n isis3 -f environment.yml
                    source activate isis3
                    mkdir -p ./install ./build && cd build
                    cmake -GNinja -DJP2KFLAG=OFF -DCMAKE_INSTALL_PREFIX=../install -Disis3Data=/usgs/cpkgs/isis3/data -Disis3TestData=/usgs/cpkgs/isis3/testData ../isis \
                   """
            }
        }
        stage('Build') { 
            steps {
                sh """
                    export PATH="${PATH}:/opt/conda/envs/isis3/bin"
                    set +e
                    cd build
                    ninja -j8 && ninja install
                   """
            }
        }
        stage('Test'){
            steps {
                sh """
                    export PATH="${PATH}:/opt/conda/envs/isis3/bin/:${workspace}/install"
                    pwd
                    set +e
                    cd build
                    ctest -j8 -V -R _unit_ 
                    ctest -j8 -V -R _app_ 
                    ctest -j8 -V -R _module_ 
                   """
            }
        }
    }
//    post {
//        success {
//            sh 'pwd && ls'
//            archiveArtifacts artifacts: "build/objects/*.o"
//        }
//        always {
//            mail to: 'ccombs@usgs.gov',
//                    subject: "Build Finished: ${currentBuild.fullDisplayName}",
//                    body: "Link: ${env.BUILD_URL}"
//            sh "rm -rf build/* && rm -rf install/*"
//            cleanWs()
//        }
//    }
}
