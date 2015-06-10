#include <my_global.h>
#include <mysql.h> //mysql c API header
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  int is_innoDB_supported = 1; //flag for finding whether innoDB is supported or not 
  int INNODB_DATA_WRITES, INNODB_LOG_WRITES, INNODB_DBLWR_WRITES, number_of_disk_writes=0;

  if(argc < 4 || argc > 4){
    fprintf(stderr, "\nFour arguments expected <output_file> <hostname> <username> <password>.\n");
    exit(1);
  }

  //config for mysql
  char *HOST = argv[1];
  char *USER = argv[2];
  char *PASSWORD = argv[3];
  char *DATABASE_NAME = "information_schema";
  int PORT = 0; //make it 0 for mysql default port
  char *SOCKET = NULL;
  int CLIENT_FLAG = 0; 
  
  //initializing mysql connection
  MYSQL *con = mysql_init(NULL);

  //checking for failure connection
  if(con == NULL){
    fprintf(stderr, "\nmysql_init() failed\n");
    exit(1);
  }

  //using mysql connection object to connect to DB
  if(mysql_real_connect(con, HOST, USER, PASSWORD, DATABASE_NAME, PORT, SOCKET, CLIENT_FLAG) == NULL){
    fprintf(stderr, "\nmysql connection coudn't able to establish\n");	
  }

  //query the DB with sql statements
  //checking whether the innoDB engine is installed or not in mysql 
  if(mysql_query(con, "select support from engines where engine = 'InnoDB'")){ //taking support field of InnoDB
    is_innoDB_supported = 0;
    fprintf(stderr, "\nmysql_query failed\n");  
  }

  MYSQL_RES *result = mysql_store_result(con);
  MYSQL_ROW row = mysql_fetch_row(result);

  if(is_innoDB_supported == 1){
    //if support field is DEFAULT or YES then it is supported for mysql 
    if(strcmp(row[0], "DEFAULT") == 0 || strcmp(row[0], "YES") == 0){
      printf("\n-----------------------------------------------------------------\n");
      printf("\nInnoDB plugin is aleady installed and engine is also supported!\n");
      printf("\n-----------------------------------------------------------------\n");

      //number_of_disk_writes = INNODB_DATA_WRITES - INNODB_LOG_WRITES - INNODB_DBLWR_WRITES
      
      mysql_free_result(result);
      //getting INNODB_DATA_WRITES value
      if(mysql_query(con, "select VARIABLE_VALUE from global_status where VARIABLE_NAME = 'INNODB_DATA_WRITES'"))
        fprintf(stderr, "\nmysql_query failed\n");

      result = mysql_store_result(con);
      row = mysql_fetch_row(result);
      INNODB_DATA_WRITES = atoi(row[0]);

      mysql_free_result(result);
      //getting INNODB_LOG_WRITES
      if(mysql_query(con, "select VARIABLE_VALUE from global_status where VARIABLE_NAME='INNODB_LOG_WRITES'"))
        fprintf(stderr, "\nmysql_query failed\n");
      
      result = mysql_store_result(con);
      row = mysql_fetch_row(result);
      INNODB_LOG_WRITES = atoi(row[0]);

      mysql_free_result(result);
      // getting INNODB_DBLWR_WRITES
      if(mysql_query(con, "select VARIABLE_VALUE from global_status where VARIABLE_NAME='INNODB_DBLWR_WRITES'"))
        fprintf(stderr, "\nmysql_query failed\n");

      result = mysql_store_result(con);
      row = mysql_fetch_row(result);
      INNODB_DBLWR_WRITES = atoi(row[0]);

      number_of_disk_writes = INNODB_DATA_WRITES - INNODB_LOG_WRITES - INNODB_DBLWR_WRITES;
      
      printf("\nNumber of Disk Writes by MySQL: %d\n", number_of_disk_writes);
      printf("\n-----------------------------------------------------------------\n");
    }
    else
      exit(1);
  }

  //freeing mysql result object and closing connections
  mysql_free_result(result);
  mysql_close(con);

  exit(0);
}
