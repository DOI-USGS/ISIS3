// vim: ft=groovy

def myFunc(machine) {
    env.STAGE_STATUS = "Creating conda environment"
    sh '''
        # Use the conda cache running on the Jenkins host
        conda config --set channel_alias http://dmz-jenkins.wr.usgs.gov
        conda config --set always_yes True
        conda create -n isis python=3
    '''

    if (machine.toLowerCase() == "centos") {
        sh 'conda env update -n isis_${machine} -f environment_gcc4.yml --prune'
    } else {
        sh 'conda env update -n isis_${machine} -f environment.yml --prune'
    }
}
