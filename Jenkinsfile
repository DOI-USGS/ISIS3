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
        'mac': {
            agent { label 'dmz-progmac' }
            stage('Mac') {
                agent { label 'dmz-progmac' }
                sh """echo Foo"""
            }
        }
    )
}
