// vim: ft=groovy

// Helpers for setting commit status
def getRepoUrl() {
    return sh(script: "git config --get remote.origin.url", returnStdout: true).trim()
}

def getCommitSha() {
    return sh(script: "git rev-parse HEAD", returnStdout: true).trim()
}


def setGitHubBuildStatus(status) {
    def repoUrl = getRepoUrl()
    def commitSha = getCommitSha()

    step([
        $class: 'GitHubCommitStatusSetter',
        reposSource: [$class: "ManuallyEnteredRepositorySource", url: repoUrl],
        commitShaSource: [$class: "ManuallyEnteredShaSource", sha: commitSha],
        errorHandlers: [[$class: 'ShallowAnyErrorHandler']],
        statusResultSource: [
          $class: 'ConditionalStatusResultSource',
          results: [
            [$class: 'BetterThanOrEqualBuildResult', result: 'SUCCESS', state: 'SUCCESS', message: status],
            [$class: 'BetterThanOrEqualBuildResult', result: 'FAILURE', state: 'FAILURE', message: status],
            [$class: 'AnyBuildResult', state: 'FAILURE', message: 'Loophole']
          ]
        ]
    ])
}

pipeline {
  agent any
  stages {
    stage("CI") {
      steps { 
        script {
          def groovy_utilities = load "${pwd()}/groovy_utilities.groovy"  

          def errors = []
          def labels = ['CentOS', 'Fedora', 'Ubuntu', 'Mac'] // labels for Jenkins node types we will build on
          def builders = [:] 
          
          println("Using Labels: " + labels)

          for (x in labels) {
            def label = x
            def lower_label = label.toLowerCase()
        
            builders[lower_label] = {
              node(lower_label) {
              
              def build_ok = true
              def condaPath = ""
              def isisEnv = [
                "ISIS3DATA=/isisData/data",
                "ISIS3TESTDATA=/isisData/testData",
                "ISIS3MGRSCRIPTS=/isisData/data/isis3mgr_scripts",
                "MALLOC_CHECK_=1"
              ]

              def cmakeFlags = [
                  "-DJP2KFLAG=ON",
                  "-DKAKADU_INCLUDE_DIR=/isisData/kakadu",
                  "-Dpybindings=OFF",
                  "-DCMAKE_BUILD_TYPE=RELEASE"
              ]

              stage (label) {
                  sh 'git config --global http.sslVerify false'
                  checkout scm
                  isisEnv.add("ISISROOT=${pwd()}/build")
                  cmakeFlags.add("-DCMAKE_INSTALL_PREFIX=${pwd()}/install")
                      
                  env.STAGE_STATUS = "Creating conda environment"
                  
                  if (lower_label == "mac") {
                    condaPath = "/tmp/macbuilds/" + sh(script: '{ date "+%m/%d/%y|%H:%M:%S:%m"; echo $WORKSPACE; } | md5 | tr -d "\n";', returnStdout: true) 
                    
                    sh """
                      mkdir -p /tmp/macbuilds/
                      curl -o miniconda.sh  https://repo.continuum.io/miniconda/Miniconda3-latest-MacOSX-x86_64.sh
                      bash miniconda.sh -b -p ${condaPath}
                      """
                  } else {
                    condaPath = "/home/jenkins/.conda/"
                  } 
           
                  isisEnv.add("PATH=${pwd()}/install/bin:$condaPath/envs/isis/bin:$condaPath/bin:${env.PATH}")

                  withEnv(isisEnv) {
                    sh 'printenv'
                    println("Anaconda Path: " + condaPath)
                    
                    sh """
                        # Use the conda cache running on the Jenkins host
                        conda config --set always_yes True
                        conda config --set ssl_verify false
                        conda create -c conda-forge --prefix ${condaPath}/envs/isis python=3
                    """
                    
                    if (lower_label == "centos") {
                        sh "conda env update -p ${condaPath}/envs/isis -f environment_gcc4.yml --prune"
                    } else {
                      sh """
                        conda config --show channels
                        conda env update -p ${condaPath}/envs/isis -f environment.yml --prune
                      """
                    }
                    dir("${env.ISISROOT}") {
                        try {
                              env.STAGE_STATUS = "Building ISIS on ${label}"
                              sh """
                                  source activate ${condaPath}/envs/isis
                                  echo `ls ../`
                                  echo `pwd`
                                  conda list
                                  cmake -GNinja ${cmakeFlags.join(' ')} ../isis
                                  ninja -j4 install
                              """
                        }
                        catch(e) {
                            build_ok = false
                            errors.add(env.STAGE_STATUS)
                            println e.toString()
                        }

                        if (build_ok) {
                            try{
                                dir("${env.ISISROOT}") {
                                    env.STAGE_STATUS = "Running unit tests on ${label}"
                                    sh """
                                        source activate ${condaPath}/envs/isis
                                        ctest -R _unit_ -j4 -VV
                                    """
                                }
                            }
                            catch(e) {
                                build_ok = false
                                errors.add(env.STAGE_STATUS)
                                echo e.toString()
                            }
                            sh 'source deactivate'

                            try{
                                env.STAGE_STATUS = "Running app tests on ${label}"
                                sh """
                                    source activate ${condaPath}/envs/isis
                                    export PATH="${condaPath}bin/:$PATH"
                                    echo $PATH
                                    ctest -R _app_ -j4 -VV
                                """
                            }
                            catch(e) {
                                build_ok = false
                                errors.add(env.STAGE_STATUS)
                                println e.toString()
                            }
                            sh 'source deactivate'

                            try{
                                sh """
                                    source activate ${condaPath}/envs/isis 
                                    ctest -R _module_ -j4 -VV
                                """
                            }
                            catch(e) {
                                build_ok = false
                                errors.add(env.STAGE_STATUS)
                                println e.toString()
                            }
                            sh 'source deactivate'

                            try{
                                env.STAGE_STATUS = "Running gtests on ${label}"
                                sh """
                                    source activate ${condaPath}/envs/isis
                                    ctest -R "." -E "(_app_|_unit_|_module_)" -j4 -VV
                                """
                            }
                            catch(e) {
                                build_ok = false
                                errors.add(env.STAGE_STATUS)
                                println e.toString()
                            }
                            sh 'source deactivate'
                        }
                      }
                      
                      if (lower_label == "mac") {
                        sh "rm -rf ${condaPath}"
                      }

                      if(build_ok) {
                          currentBuild.result = "SUCCESS"
                      }
                      else {
                          currentBuild.result = "FAILURE"
                          def comment = "Failed during:\n"
                          errors.each {
                              comment += "- ${it}\n"
                          }
                          setGitHubBuildStatus(comment)
                      }
                  }
                }
              }
            }
          }
          parallel builders
        }
      }
    }
  }
}
