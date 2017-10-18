#ifndef __COMMAND_H
#define __COMMAND_H

#define CMD_LEN 60

typedef struct {  
  char     *cmd_file;                         
  int      max_args;                                  
  void     (*handle)(long int argc,void (*cmd_arg));
  char     *help;                                   
}cmd_list_struct;  


unsigned long int get_true_char_stream(unsigned char *dest,const unsigned char *src);
unsigned char  findcmdfunction(unsigned char *dest);
void alarm_arg(void);
void config_arg( long int argc,void *cmd_arg);
void da_arg(long int argc,void *cmd_arg);
void db_arg(long int argc,void *cmd_arg);
void dx_arg(long int argc,void *cmd_arg);
void gg_arg(long int argc,void *cmd_arg);
void aop_arg(long int argc,void *cmd_arg);
void aoerr_arg(long int argc,void *cmd_arg);
void install_arg(long int argc,void *cmd_arg);
void date_arg(long int argc,void *cmd_arg);
void record_arg(long int argc,void *cmd_arg);
void clear_arg(long int argc,void *cmd_arg);
void ci_arg(long int argc,void *cmd_arg);
void setmid_arg(long int argc,void *cmd_arg);
void cf_arg(long int argc,void *cmd_arg);



#endif

