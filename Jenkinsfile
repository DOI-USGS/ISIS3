// vim: ft=groovy

pipeline

{
    agent any
    stages
    {
        stage('CI')
        {
            parallel
            {
                stage('Mac')
                {
                    agent{ label 'mac'}
                    steps{
                        build 'ISIS-Builds/Mac'
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
}
