import xml.etree.ElementTree

def xmlparser():
	
	ragnarok = xml.etree.ElementTree.parse("ragnarok.xml")

	APs = ragnarok.findall("info_AP")
	for info_AP in APs:
		print (info_AP.find("mac").text)
		print (info_AP.find("channel").text)
		print (info_AP.find("frequency").text)
		print (info_AP.find("quality").text)
		print (info_AP.find("signal").text)
		print (info_AP.find("essid").text)
		print (info_AP.find("last_beacon").text)
		print (info_AP.find("encryption").text)

def main():

	xmlparser()


if __name__ == '__main__':
	main()

