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
	

# def insert_encryption(db_conn, Id_encryption, Encryption_name):
# 	curs = db_conn.cursor() 
# 	curs.execute("""INSERT INTO Encryption (Id_encryption, Encryption_name) values (?, ?)""",
# 				(Id_encryption, Encryption_name))


# def insert_device_info(db_conn, time, mac, ip):
# 	curs = db_conn.cursor() 
# 	curs.execute("""INSERT INTO  Devide_info (Time, Mac, Ip) values (?, ?, ?)""",
# 				(time, mac, ip))


# def insert_quality(db_conn, Id_quality, Qual_Rpi1, Qual_Rpi2, Qual_Rpi3):
# 	curs = db_conn.cursor() 
# 	curs.execute("""INSERT INTO  Quality (Id_quality, Qual_Rpi1, Qual_Rpi2, Qual_Rpi3) values (?, ?, ?, ?)""",
# 				(Id_quality, Qual_Rpi1, Qual_Rpi2, Qual_Rpi3))
	

# def person_data_from_element(element): 
#     first = element.find("firstName").text 
#     last = element.find("lastName").text 
#     profession = element.find("profession").text 
#     return first, last, profession 

# def xmlparser():
	
#  	ragnarok = xml.etree.ElementTree.parse("ragnarok.xml")
#  	APs = ragnarok.findall("info_AP")
#  	for info_AP in APs:

def ap_data_from_element(info_AP):
	mac = info_AP.find("mac").text
	essid = info_AP.find("essid").text
	channel = info_AP.find("channel").text
	beacon = info_AP.find("last_beacon").text
	quality = info_AP.find("quality").text
	signal = info_AP.find("signal").text
	frequency = info_AP.find("frequency").text
	encryption = info_AP.find("encryption").text
	return mac, channel, frequency, quality, signal, essid, beacon, encryption


if __name__ == '__main__':
	conn = mysql.connector.connect(host="localhost",user="root",password="root", database="ragnarok_bdd")
	
	ragnarok = xml.etree.ElementTree.parse("ragnarok.xml")
	APs = ragnarok.findall("info_AP")
	for element in APs:
		mac, channel, frequency, quality, signal, essid, beacon, encryption = ap_data_from_element(element)
		print(essid)
		insert_info_ap(conn, mac, essid, "10:00", 1, channel, beacon, signal, frequency, 1, "AA:AA:AA:AA:AA:AA")


