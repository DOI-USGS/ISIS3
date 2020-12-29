// vim: ft=groovy

def labels = ['centos', 'fedora', 'ubuntu', 'mac'] // labels for Jenkins node types we will build on
def nodes = [:] 
def ISIS_VERSION="4.2.0"

for (lbl in labels) {
    def label = lbl 

    nodes[label] = {
        stage(label) {
            isisNode(label) {
                script {
                  def condaPath = "/tmp/builds/" + sh(script: '{ date "+%m/%d/%y|%H:%M:%S:%m"; echo $WORKSPACE; } | md5 | tr -d "\n";', returnStdout: true)

                  if ("${label}" == "mac") {
                  sh """
                    curl -o miniconda.sh  https://repo.anaconda.com/miniconda/Miniconda3-latest-MacOSX-x86_64.sh
                    bash miniconda.sh -b -p ${condaPath}
                    """
                  }
                  else {
                    sh """
                      curl -o miniconda.sh  https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
                      bash miniconda.sh -b -p ${condaPath}
                    """
                  }
                  
                  sh """
                     ${condaPath}/bin/conda config --set always_yes True
                     ${condaPath}/bin/conda config --set ssl_verify false
                     ${condaPath}/bin/conda config --env --add channels conda-forge
                     ${condaPath}/bin/conda config --env --add channels usgs-astrogeology
                     
                     ${condaPath}/bin/conda create -n isis -c acpaquette/label/reconfig isis=${ISIS_VERSION}

                     export ISISROOT=${condaPath}/envs/isis/
                     ${condaPath}/bin/conda run -n isis campt -HELP
                  """

                  // skip build for centos
                  if ("${label}" != "centos") {
                    checkout scm

                    sh """
                      git checkout dev 
                      cd recipe 
                      ${condaPath}/bin/conda install conda-build
                      ${condaPath}/bin/conda build . --no-anaconda-upload  
                    """
                  }
                }
            }
        }
    }
}

// Run the builds in parallel
node {
    try {
        parallel nodes

    } catch(e) {
        // Report result to GitHub
        currentBuild.result = "FAILURE"
        
        def comment = "Failed during:\n"
        errors.each {
            comment += "- ${it}\n"
        }

        setGithubStatus(comment)
    }
}
