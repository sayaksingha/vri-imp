# @(#) $Id:  $
export PATH=$PATH:/usr/local/mysql/bin

mysql -h 'localhost' -uivr -pivr ivr -e "call mmp_mis_cumulative_main_proc()"
