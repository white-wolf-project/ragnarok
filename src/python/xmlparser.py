import xml.etree.ElementTree
import sqlite3


def insert_info_ap(db_conn, mac, essid, time, Id_encryption, Channel, Beacon, Signal, Frequency, id_quality): 
	curs = db_conn.cursor() 
	curs.execute("insert into Info_AP (Mac, ESSID, Time, Id_encryption, Channel, Beacon, Signal, Frequency, Id_quality) values (?, ?, ?, ?, ?, ?, ?, ?, ?)", 
				(mac, essid, time, Id_encryption, Channel, Beacon, Signal, Frequency, id_quality)) 
	db_conn.commit()

def insert_encryption(db_conn, Id_encryption, Encryption_name):
	curs = db_conn.cursor() 
	curs.execute("insert into Encryption (Id_encryption, Encryption_name) values (?, ?)",
				(Id_encryption, Encryption_name))
	db_conn.commit()

def insert_device_info(db_conn, time, mac, ip):
	curs = db_conn.cursor() 
	curs.execute("insert into Devide_info (Time, Mac, Ip) values (?, ?, ?)",
				(time, mac, ip))
	db_conn.commit()

def insert_quality(db_conn, Id_quality, Qual_Rpi1, Qual_Rpi2, Qual_Rpi3)
	curs = db_conn.cursor() 
	curs.execute("insert into Quality (Id_quality, Qual_Rpi1, Qual_Rpi2, Qual_Rpi3) values (?, ?, ?, ?)",
				(Id_quality, Qual_Rpi1, Qual_Rpi2, Qual_Rpi3))
	db_conn.commit()

# def person_data_from_element(element): 
#     first = element.find("firstName").text 
#     last = element.find("lastName").text 
#     profession = element.find("profession").text 
#     return first, last, profession 

# def xmlparser():
	
# 	ragnarok = xml.etree.ElementTree.parse("ragnarok.xml")
# 	APs = ragnarok.findall("info_AP")
# 	for info_AP in APs:

def ap_data_from_element(info_AP):
	mac = info_AP.find("mac").text
	channel = info_AP.find("channel").text
	frequency = info_AP.find("frequency").text
	quality = info_AP.find("quality").text
	signal = info_AP.find("signal").text
	essid = info_AP.find("essid").text
	beacon = info_AP.find("last_beacon").text
	encryption = info_AP.find("encryption").text
	return mac, channel, frequency, quality, signal, essid, beacon, encryption


if __name__ == '__main__':
	conn = sqlite3.connect("ragnarok.sqlite3")
	ragnarok = xml.etree.ElementTree.parse("ragnarok.xml")
	APs = ragnarok.findall("info_AP")
	for element in APs:
		mac, channel, frequency, quality, signal, essid, beacon, encryption = ap_data_from_element(element)
		insert_info_ap(conn, mac, essid, 0, 0, channel, beacon, signal, frequency, 0 )


