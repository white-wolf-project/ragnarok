#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <my_global.h>
#include <mysql.h>

bool check4db(const char *db_name);
int remove_db(const char *db_name);

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
		printf("%s\n", mysql_error(con));
		return -2;
	}
	return 0;
}

void usage(const char *name){
	fprintf(stdout, "usage : %s [arg]\n", name);
	fprintf(stdout, "-c check database\n");
	fprintf(stdout, "-r remove database\n");
	fprintf(stdout, "-h help\n");

}
int main(int argc, char const *argv[])
{
	int i;
	if (argc != 2)
	{
		usage(argv[0]);
	}
	for (i = 1; i < argc; ++i)
	{
		if (!strcmp("-c", argv[i])) {
			if (check4db("ragnarok_bdd")) {
				printf("database exists\n");
			} else {
				printf("database does not exist\n");
			}
		} else if (!strcmp("-r", argv[i])) {
			remove_db("ragnarok_bdd");
		} else if (!strcmp("-h", argv[i])) {
			usage(argv[0]);
		}
	}
		return 0;
}