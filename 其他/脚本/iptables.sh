#/bin/bash

iptables -F

iptables -P INPUT DROP
iptables -P FORWARD DROP
iptables -P OUTPUT ACCEPT

# ssh
iptables -A INPUT -p tcp --dport 22 -m state --state NEW,ESTABLISHED -j ACCEPT

# oracle
iptables -A INPUT -p tcp --dport 1521 -m state --state NEW,ESTABLISHED -j ACCEPT

# dns
iptables -A INPUT -p udp  --sport 53 -j ACCEPT
iptables -A INPUT -p tcp  --sport 53 -j ACCEPT

# http
iptables -A INPUT -p tcp --sport 80 -m state --state ESTABLISHED -j ACCEPT
iptables -A INPUT -p tcp --sport 443 -m state --state ESTABLISHED -j ACCEPT

# ftp
iptables -A INPUT -p tcp -m multiport --dport 20,21  -m state --state NEW,ESTABLISHED -j ACCEPT
iptables -A INPUT -p tcp --dport 61001:62000 -j ACCEPT

# ping
iptables -A INPUT -p icmp -j ACCEPT

# localhost
iptables -A INPUT -s 127.0.0.1/32 -j ACCEPT
