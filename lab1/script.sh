#!/bin/bash

pwd
dt=`date '+%d/%m/%Y_%H:%M:%S'`
echo $dt
echo $PATH | tr ':' '\n'
