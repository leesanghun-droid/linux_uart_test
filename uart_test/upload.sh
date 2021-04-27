#!/bin/bash
aarch64-linux-gnu-g++ main.cpp -o main 
sshpass -p linaro scp main linaro@192.168.1.5:~/cross_test
#sshpass -p linaro ssh linaro@192.168.1.5 "./cross_test/main" 
