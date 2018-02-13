#!/usr/bin/env python
#-*- coding: utf-8 -*-

#XML generator

from lxml import etree

ragnarok = etree.Element("ragnarok")

device = etree.SubElement(ragnarok, "device")
device.set("id", "")
essid = etree.SubElement(device, "essid")
essid.text = ""
mac = etree.SubElement(device, "mac")
mac.text = ""
timestamp = etree.SubElement(device, "timestamp")
timestamp.text = ""

info_ap = etree.SubElement(ragnarok, "info_ap")
mac = etree.SubElement(info_ap, "mac")
mac.text = ""
id_encryption = etree.SubElement(info_ap, "id_encryption")
id_encryption.text = ""
channel = etree.SubElement(info_ap, "channel")
channel.text = ""
beacon = etree.SubElement(info_ap, "beacon")
beacon.text = ""
id_quality = etree.SubElement(info_ap, "id_quality")
id_quality.text = ""

encryption = etree.SubElement(ragnarok, "encryption")
encryption_name = etree.SubElement(encryption, "encryption_name")
encryption_name.text = ""
id_encryption = etree.SubElement(encryption, "id_encryption")
id_encryption.text = ""

timeStat = etree.SubElement(ragnarok, "time")
time = etree.SubElement(timeStat,"time")
time.text = ""
timestamp = etree.SubElement(timeStat, "timestamp")
timestamp.text = ""

quality = etree.SubElement(ragnarok, "quality")
id_quality = etree.SubElement(quality, "id_quality")
id_quality.text = ""
qual_rpi0 = etree.SubElement(quality,"qual_rpi0")
qual_rpi0.text = ""
qual_rpi1 = etree.SubElement(quality,"qual_rpi1")
qual_rpi1.text = ""
qual_rpi2 = etree.SubElement(quality,"qual_rpi2")
qual_rpi2.text = ""


print(etree.tostring(ragnarok, pretty_print=True))
#with open("log.xml", "r") as xml_file:
#	xml_file.read