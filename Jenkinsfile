// vim: ft=groovy

pipeline 
{
    agent any
    stages
    {
        parallel
        {
            stage('Mac')
            {
                agent{ label 'mac'}
                steps{
                    echo "Foo"
                    sleep 20
                }
            }
            stage('CentOS')
            {
                steps
                {
                    build 'ISIS-Builds/CentOS'
                }
            }
            stage('Fedora')
            {
                steps
                {
                    build 'ISIS-Builds/Fedora'
                }
            }
            stage('Ubuntu')
            {
                steps
                {
                    build 'ISIS-Builds/Ubuntu'
                }
            }
        }
    }
}

