// vim: ft=groovy

def NUM_CORES = 8
def errors = []

pipeline {
    agent {
        docker { 
                image '950438895271.dkr.ecr.us-west-2.amazonaws.com/asc-jenkins' 
                registryCredentialsId 'ecr:us-west-2:Jenkins-Manager-Role'
                registryUrl 'https://950438895271.dkr.ecr.us-west-2.amazonaws.com'
                args '--entrypoint= -v /astro_efs:/astro_efs'
               }
    }
    environment {
        ISISDATA        =   '/astro_efs/isis_data'
        ISISTESTDATA    =   '/astro_efs/isis_testData'
        MALLOC_CHECK_   =   1
        ISISROOT        =   "${env.WORKSPACE}/build"
        KAKADU_HEADERS  =   '/astro_efs/kakadu_7_9'
    }
    
    stages {
        stage('Environment Setup') {
            steps {
                sh '''
                . /home/conda/mambaforge3/etc/profile.d/conda.sh > /dev/null
                echo "ISISROOT: ${ISISROOT}"
                
                conda create -y -n isis
                conda activate isis > /dev/null
                conda install -c conda-forge python=3 findutils
                mamba env update -f environment.yml --prune
                conda activate isis
                mamba install -c conda-forge git
                git submodule update --init --recursive
                conda list
                '''
            }
        }
        stage('Build') {
            steps {
                echo "Building....."
                // sh '''
                // . /home/conda/mambaforge3/etc/profile.d/conda.sh > /dev/null
                // conda activate isis > /dev/null
                // mkdir -p build install
                // cd build
                // cmake -GNinja -DJP2KFLAG=ON  \
                //       -DKAKADU_INCLUDE_DIR=${KAKADU_HEADERS} \
                //       -Dpybindings=OFF \
                //       -DCMAKE_BUILD_TYPE=RELEASE \
                //       -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} \
                //       ../isis
                // ninja -j 8 install
                // '''
            }
        }
        stage("Tests") {
            parallel {
                stage('GTests') {
                    steps {
                        echo "Testing G's"
                        // catchError(buildResult: 'FAILURE', stageResult: 'FAILURE') {
                        //    sh '''
                        //    . /home/conda/mambaforge3/etc/profile.d/conda.sh > /dev/null
                        //    conda activate isis > /dev/null
                        //    cd build
                        //    ctest -R '.' -E '(_app_|_unit_|_module_)' -j 8 --output-on-failure --timeout 10000
                        //    '''
                        // }
                    }
                }
                stage('Unit Tests') {
                    steps {
                        echo "Testing Units"
                        // catchError(buildResult: 'FAILURE', stageResult: 'FAILURE') {
                        //     sh '''
                        //     . /home/conda/mambaforge3/etc/profile.d/conda.sh > /dev/null
                        //     conda activate isis > /dev/null
                        //     cd build
                        //     ctest -R _unit_ -j 8 --output-on-failure
                        //     '''
                        // }
                    }
                }
                stage('App Tests') {
                    steps {
                        echo "Testing appetizers"
                        // catchError(buildResult: 'FAILURE', stageResult: 'FAILURE') {
                        //     sh '''
                        //     . /home/conda/mambaforge3/etc/profile.d/conda.sh > /dev/null
                        //     conda activate isis > /dev/null
                        //     cd build
                        //     ctest -R _app_ -j 8 --output-on-failure --timeout 10000
                        //     '''
                        // }
                    }
                }
                stage('Module Tests') {
                    steps {
                        echo "Testing modules"
                        // catchError(buildResult: 'FAILURE', stageResult: 'FAILURE') {
                        //     sh '''
                        //     . /home/conda/mambaforge3/etc/profile.d/conda.sh > /dev/null
                        //     conda activate isis > /dev/null
                        //     cd build
                        //     ctest -R _module_ -j 8 --output-on-failure --timeout 10000
                        //     '''
                        // }
                    }
                }
                stage('Py Tests') {
                    environment {
                        PATH            =   "${env.WORKSPACE}/install/bin:${env.PATH}"
                    }
                    steps {
                        echo "Testing pys"
                        // catchError(buildResult: 'FAILURE', stageResult: 'FAILURE') {
                        //     sh '''
                        //     . /home/conda/mambaforge3/etc/profile.d/conda.sh > /dev/null
                        //     conda activate isis > /dev/null
                        //     cd build
                        //     cd $WORKSPACE/isis/pytests 
                        //     pytest .

                        //     '''
                        // }
                    }
                }
            }
        }
        stage('Deploy') {
            // when { 
            //   allOf { 
            //     expression { env.GITHUB_PR_STATE == "CLOSE" }
            //     expression { env.GITHUB_PR_TARGET_BRANCH == "test-dev" }
            //     expression { env.GITHUB_PR_SOURCE_BRANCH == "PR-*" }
            //   } 
            // }
            steps {
                script{
                    echo "Deploy! Deploy!"
                    echo "${env.GITHUB_REPO_GIT_URL}"
                    echo "${env.GITHUB_PR_STATE}"
                    pullRequest.labels.each{
                        echo "label: $it"
                    }
                }
            }
        }
    }
}
