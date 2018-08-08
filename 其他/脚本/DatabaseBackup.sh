#!/bin/bash
export ORACLE_BASE=/home/zzh/oracle
export ORACLE_HOME=/home/zzh/oracle/app
name=`date "+%Y-%m-%d"`
/home/zzh/oracle/app/bin/exp \'sys/Abcd1234@telemedoa as sysdba\' file=~/dbBackup/${name}.dmp tables=staff users small_pkg big_pkg tags small_out big_out agencies TEMPERTEST
# need to shutdown iptables
#/home/zzh/scripts/MountBack.sh
#cp ~/dbBackup/${name}.dmp  /mnt/windows/db_backup/
