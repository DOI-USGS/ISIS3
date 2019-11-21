// vim: ft=groovy

pipeline
{
    agent any
    stages {
        stage ("CI") {
            steps {
                script {
                    def labels = ['centos', 'fedora'] // labels for Jenkins node types we will build on
                    def builders = [:]
                    for (x in labels) {
                        def label = x // Need to bind the label variable before the closure - can't do 'for (label in labels)'

                        // Create a map to pass in to the 'parallel' step so we can fire all the builds at once
                        builders[label] = {
                            node(label) {
                                stage("Create environment") {
                                    env.STAGE_STATUS = "Creating conda environment"
                                    echo "${pwd()}"
                                    sh '''
                                      # Use the conda cache running on the Jenkins host
                                      conda config --set channel_alias http://dmz-jenkins.wr.usgs.gov
                                      conda config --set always_yes True
                                      conda create -n isis python=3
                                    '''

                                    if (label == "centos") {
                                      sh 'conda env update -n isis -f environment_gcc4.yml --prune'
                                    } else {
                                      sh 'conda env update -n isis -f environment.yml --prune'
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
