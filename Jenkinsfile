// vim: ft=groovy

pipeline 
{
    agent any
    stages
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
            build 'ISIS-Builds/CentOS'
        }
    }
}

