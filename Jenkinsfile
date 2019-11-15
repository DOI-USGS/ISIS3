// vim: ft=groovy

node {
    parallel(
        'centos': {
            stage('CentOS') {
                build 'ISIS-Builds/CentOS'
            }
        },
        'fedora': {
            stage('Fedora') {
                build 'ISIS-Builds/Fedora'
            }
        },
        'ubuntu': {
            stage('Ubuntu') {
                build 'ISIS-Builds/Ubuntu'
            }
        },
        'mac' : {
            stage('Mac') {
                label 'mac'
                build 'ISIS-Builds/mac'
            }
        }
    )
}
