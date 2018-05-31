/**
 * @file server_util.c
 * @author Mathieu Hautebas
 * @date May 30th 2018
 * @brief file containing functions for the server.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <my_global.h>
#include <mysql.h>
#include <include/server.h>
#include <include/server_util.h>
#include <include/common.h>

/**
 * @brief
 * check if it's the beginning, the end or the middle of the XML file
 * @param const char *data
 * @return returns 0 if it's XML start 1 if xml ends or 2
 */
int check4xml(const char *data){
	if (strstr(data , "-xml-") != NULL){
		return 0;
	} else if (strstr(data, "-end_xml-") != NULL){
		return 1;
	} else {
		return 2;
	}
}
/**
 * @brief
 * function used to write our XML data to a file as raw data
 * @param FILE *fp_xml
 * @param const char *xml_data
 * @param int xml_data_len
 * @param int xml_start
 * @param int xml_end
 * if xml data started we write to our XML after -xml- which is a sort of tag I made
 * if xml data ends then set first char of end_of_xml to \0.
 * else it's just our XML data
 * @return void
 */
void write_to_xml(FILE *fp_xml, const char *xml_data, int xml_data_len, int xml_start, int xml_end){
	char *end_of_xml = strstr(xml_data, "-end_xml-");

	if (xml_start == 1) {
		fwrite(xml_data + strlen("-xml-"), sizeof(char), xml_data_len - strlen("-xml-"), fp_xml);
	} else if (xml_end == 1) {
		end_of_xml[0] = '\0';
		fwrite(xml_data, sizeof(char), xml_data_len - strlen("-end_xml-"), fp_xml);
	} else if (xml_start == 0 && xml_end == 0) {
		fwrite(xml_data, sizeof(char), xml_data_len, fp_xml);
	}
}

/**
 * @brief
 * @verbatim handle_xml_data() @endverbatim is used to run the python3 code
 * we call it once we have 3 xml files. At some point all the code will written be in C
 * @param int counter
 * if xml data started we write to our XML after -xml- which is a sort of tag I made
 * if xml data ends then set first char of end_of_xml to \0.
 * else it's just our XML data
 * @return void
 */
int handle_xml_data(){
	const char *arg_tab[4] = {0};
	char python_cmd[256];
	#ifdef RELEASE
		arg_tab[0] = NULL;
		arg_tab[1] = "/var/opt/ragnarok1.xml";
		arg_tab[2] = "/var/opt/ragnarok2.xml";
		arg_tab[3] = "/var/opt/ragnarok3.xml";
	#else
		arg_tab[0] = NULL;
		arg_tab[1] = "server_0.xml";
		arg_tab[2] = "server_1.xml";
		arg_tab[3] = "server_2.xml";
	#endif

	sprintf(python_cmd, "python3 src/python/xmlparser.py %s %s %s", arg_tab[1], arg_tab[2], arg_tab[3]);
	log_it("check if ragnarok_bdd exists");

	/* this statement is not really useful, we don't need it for the moment */
	if (check4db("ragnarok_bdd") != true) {
		log_it("ragnarok_bdd does not exist, creating it");
		system("src/python/ragnarok_bdd.py");
		sleep(1);
		system(python_cmd);
	} else {
		log_it("ragnarok_bdd found");
		system(python_cmd);
	}

	return 0;
}

/**
 * @brief
 * TODO : change db name
 * Check if the ragnarok_bdd datase exists
 * We open a connection to the SQL server. Then we check if you can create a new DB.
 * If DB is created we just delete it and return false to let Python scripts do their job.
 * @param const char *db_name : name of database to check for
 * @return true if database exists
 * @see https://dev.mysql.com/
 */
bool check4db(const char *db_name)
{
	char err[128];
	char db_query[64];
	MYSQL *con = mysql_init(NULL);

	if (con == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		exit(1);
	}

	/* connection to mysql server */
	if (mysql_real_connect(con, "localhost", "root", "root", NULL, 0, NULL, 0) == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return false;
	}
	/* try to create DB, if it already exists then return true else we return false */
	sprintf(db_query, "CREATE DATABASE %s", db_name);
	if (mysql_query(con, db_query))
	{
		sprintf(err, "Can't create database '%s'; database exists", db_name);
		if (!strcmp(mysql_error(con), err))
		{
			mysql_close(con);
			return true;
		}
	} else {
		/* we delete DB after testing to not break python scripts */
		sprintf(db_query, "DROP DATABASE %s", db_name);
		mysql_query(con, db_query);
		mysql_close(con);
		return false;
	}
	return false;
}

/**
 * @brief
 * TODO : change db name
 * DROP database. We don't update it properly our database, so we decided to just delete it.
 * @param const char *db_name : name of database to check for
 * @return a number != 0 if something goes wrong
 * @see https://dev.mysql.com/
 */
int remove_db(const char *db_name){
	char db_query[64];
	char err[128];
	MYSQL *con = mysql_init(NULL);

	if (con == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		exit(1);
	}

	/* connection to mysql server */
	if (mysql_real_connect(con, "localhost", "root", "root", NULL, 0, NULL, 0) == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return -1;
	}

	/* drop database */
	sprintf(err, "Can't drop database '%s'; database doesn't exist", db_name);
	sprintf(db_query, "DROP DATABASE %s", db_name);
	if (mysql_query(con, db_query))
	{
		/*
		 * at the first run we should have an error and ret -2
		 * because it can't delete something
		 * that does not exists
		*/
		if (strcmp(mysql_error(con) ,err))
		{
			debug("%s\n", mysql_error(con));
		}
		mysql_close(con);
		return -2;
	}
	return 0;
}
