#!/usr/bin/env bash

apt-get update
apt-get uninstall -y apache2
apt-get install -y libmysqlclient-dev
apt-get install -y nginx
if ! [ -L /var/www ]; then
	rm -rf /var/www
	ln -fs /vagrant /var/www
fi