// vim: ft=groovy

def myFunc() {
    println("IN MY FUNCTION")
    checkout scm
}

checkout scm

return this
