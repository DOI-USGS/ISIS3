pipeline {
 stages{
  stage ('Work'){
    parallel {
      stage('CentOS') {
            build 'ISIS-Builds/CentOS'
      }
      stage('Fedora') {
                build 'ISIS-Builds/Fedora'
      }
      stage('Ubuntu') {
                build 'ISIS-Builds/Ubuntu'
      }
      stage('Timmy') {
        agent { label "dmz-progmac" }
        steps {echo "Foo"}
      }
   }
  }}
}
