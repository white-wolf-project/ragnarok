#!/usr/bin/env python
# -*- coding: utf-8 -*-

import mysql.connector
import xml.etree.ElementTree 
import os

path = os.getcwd
os.chdir("/home/eehp/Desktop")

conn = mysql.connector.connect(host="localhost",user="root",password="root", database="ragnarok_bdd")


def insert_info_ap(db_conn, mac, essid, time, id_encryption, channel, beacon, signal, frequency, id_quality, MAC_Rasb): 
	curs = db_conn.cursor() 
	sql = "INSERT INTO Info_AP VALUES ('%s', '%s', '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s');" % \
		(mac, essid, time, id_encryption, channel, beacon, signal, frequency, id_quality, MAC_Rasb)
	curs.execute(sql)
	
def insert_device_info(db_conn, time, mac, ip):
	curs = db_conn.cursor()
	sql = "INSERT INTO Device_info VALUES ('%s', '%s', '%s');" % \
		(time, mac_rasb, ip)  
	curs.execute(sql)

def insert_encryption(db_conn, Encryption_name):
	curs = db_conn.cursor()
	sql1 = "INSERT INTO Encryption VALUES (NULL,'%s');" % \
		(Encryption_name)
	sql2 = "SELECT COUNT(Encryption_name) FROM Encryption WHERE Encryption_name='%s';" % \
		(Encryption_name)
	curs.execute(sql2)
	rows = curs.fetchone()
	nbligne = rows[0]
	print (nbligne)
	if nbligne == 0:
		curs.execute(sql1)
	else:
		pass
	
	

def insert_quality(db_conn, Qual_Rpi1, Qual_Rpi2, Qual_Rpi3):
	curs = db_conn.cursor() 
	sql = "INSERT INTO Quality VALUES (NULL, '%s', '%s', '%s');" % \
		(Qual_Rpi1, Qual_Rpi2, Qual_Rpi3)
	curs.execute(sql)

def ap_data_from_element(info_AP,info_rasb):
	mac = info_AP.find("mac").text
	essid = info_AP.find("essid").text
	channel = info_AP.find("channel").text
	beacon = info_AP.find("last_beacon").text
	quality = info_AP.find("quality").text
	signal = info_AP.find("signal").text
	frequency = info_AP.find("frequency").text
	encryption = info_AP.find("encryption").text
	time = info_rasb.find("time").text
	mac_rasb = info_rasb.find("mac").text
	return mac, channel, frequency, quality, signal, essid, beacon, encryption, time, mac_rasb


if __name__ == '__main__':
	conn = mysql.connector.connect(host="localhost",user="root",password="root", database="ragnarok_bdd")
	ragnarok = xml.etree.ElementTree.parse("ragnarok.xml")
	APs = ragnarok.findall("info_AP")
	RASBs = ragnarok.find("rpi_info")

	for AP in APs:
		mac, channel, frequency, quality, signal, essid, beacon, encryption, time, mac_rasb = ap_data_from_element(AP,RASBs)
		insert_encryption(conn, encryption)
		conn.commit()
		insert_info_ap(conn, mac, essid, time, 1, channel, beacon, signal, frequency, 1, mac_rasb)
	conn.commit()
	insert_device_info(conn, time, mac_rasb, 0)
	conn.commit()

	qual1,qual2,qual3 = 0, 1, 2
	insert_quality(conn,qual1,qual2,qual3)
	conn.commit()