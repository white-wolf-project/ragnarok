#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <libxml/parser.h>
#include <include/xml.h>

#include <include/client.h>

unsigned char *parsed_ip, *parsed_port, *parsed_iface;
char *ipaddr, *port, *iface;


/**
 * @brief
 * Check if it is a node with no children
 * @param xmlNode * node : XML node to check
 * @return void, no return
 */

int is_leaf(xmlNode * node)
{
	xmlNode * child = node->children;
	while(child)
	{
		if(child->type == XML_ELEMENT_NODE) return 0;
		child = child->next;
	}
	return 1;
}

/**
 * @brief
 * Find the 3 info we want in the tree, then put them in a new var which are not unsigned, to work with my other functions like this :
 * @code
 * ipaddr = (char *)parsed_ip;
 * port = (char *)parsed_port;
 * iface = (char *)parsed_iface;
 * @endcode
 * @param xmlNode * node : set root element
 * @param indent_len : the length to indent
 * @return void, no return
 */
void print_xml(xmlNode * node, int indent_len)
{
	int i = 0;
	while(node)
	{
		if(node->type == XML_ELEMENT_NODE)
		{
			if (is_leaf(node)?xmlNodeGetContent(node):xmlGetProp(node, (const xmlChar *)"id"))
			{
				switch(i) {
					case 0:
						parsed_ip = is_leaf(node) ? xmlNodeGetContent(node):xmlGetProp(node, (const xmlChar *)"id");
						break;
					case 1:
						parsed_port = is_leaf(node) ? xmlNodeGetContent(node):xmlGetProp(node, (const xmlChar *)"id");
						break;
					case 2:
						parsed_iface = is_leaf(node) ? xmlNodeGetContent(node):xmlGetProp(node, (const xmlChar *)"id");
						break;
				}
				/* you can print all values here if i == 2, usefull for debug, I keep it here*/
				/*if (i == 2)
				{
					printf("%s\n", parsed_ip);
					printf("%s\n", parsed_port);
					printf("%s\n", parsed_iface);
				}*/
			} i++;
		}
		ipaddr = (char *)parsed_ip;
		port = (char *)parsed_port;
		iface = (char *)parsed_iface;
		print_xml(node->children, indent_len + 1);
		node = node->next;
	}
}


/**
 * @brief
 * Function to parse our config files
 * @param xmlfile : pointer to char which is the file to parse
 * @param results : pointer in struct addrinfo
 * @return returns 0 if everything's done well or -2 if we can't parse XML file
 */
int parse_config_file(const char *xmlfile){

	if (xmlfile == NULL || access(xmlfile, F_OK) == -1)
	{
		fprintf(stderr, "%s : %s\n", xmlfile, strerror(errno));
		return -1;
	} else {
		xmlDoc *doc = NULL;
		xmlNode *root_element = NULL;
		doc = xmlReadFile(xmlfile, NULL, 0);

		if (doc == NULL) {
			printf("Could not parse the XML file");
			return -2;
		}

		root_element = xmlDocGetRootElement(doc);
		print_xml(root_element, 1);
		xmlFreeDoc(doc);
		xmlCleanupParser();
	}

	return 0;
}