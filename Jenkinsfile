pipeline {
    agent none
    environment {
        ISISROOT="${workspace}" + "/build/"
        ISIS3TESTDATA="/usgs/cpkgs/isis3/testData/"
        ISIS3DATA="/usgs/cpkgs/isis3/data/"
    }
    stages {
        stage('Fedora25') {
            agent {
                label 'isis-fedora-25'
            }
            steps {
              sh"""
                  ls -lah
                  echo $PWD
              """
                // sh """
                //     conda env create -n isis3 -f environment.yml
                //     source activate isis3
                //     mkdir -p ./install ./build && cd build
                //     source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
                //     cmake -GNinja -DJP2KFLAG=OFF -Dpybindings=OFF -DCMAKE_INSTALL_PREFIX=../install -Disis3Data=/usgs/cpkgs/isis3/data -Disis3TestData=/usgs/cpkgs/isis3/testData ../isis
                //     set +e
                //     ninja -j8 && ninja install
                //     source /usgs/cpkgs/isis3/isis3mgr_scripts/initIsisCmake.sh .
                //     ctest -V -R _unit_ --timeout 500
                //     ctest -V -R _app_ --timeout 500
                //     ctest -V -R _module_ --timeout 500
                //     """
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
