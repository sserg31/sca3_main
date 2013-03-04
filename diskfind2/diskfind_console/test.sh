#!/bin/bash
#./diskfind_console -d ./ -w собака -w word -w слово -t doc -i test.txt -e utf8 -e koi8r
./diskfind_console -d ./dict -w гавгавгав -w "какая-то фраза" -t doc -i /dev/sdb1 -e utf8 -e koi8r
