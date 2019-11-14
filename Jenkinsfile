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
        'timmy': {
            stage('Timmy') {
                agent { 
                    label "dmz-progmac" 
                }
                steps {echo "Foo"}
            }
        }
    )
}
