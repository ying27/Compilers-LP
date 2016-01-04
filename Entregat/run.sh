#!/bin/bash

if antlr -gt karel.g && dlg -ci parser.dlg scan.c && g++ -o karel karel.c scan.c err.c -std=c++11; then
    clear
    echo "*****Compilation succeeded*****"
    ./karel < input2.txt

else
    echo "*****Compilation failed*****"
fi
