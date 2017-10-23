#ifndef __COMMAND_H
#define __COMMAND_H

#define CMD_LEN 60

typedef struct {  
  char     *cmd_file;                         
  int      max_args;                                  
  void     (*handle)(void);
  char     *help;                                   
}cmd_list_struct;  


unsigned long int get_true_char_stream(unsigned char *dest,const unsigned char *src);
unsigned char  findcmdfunction(unsigned char *dest);
void alarm_arg(void);
void config_arg_d0(void);
void config_arg_d1(void);
void config_arg_d2(void);
void config_arg_d3(void);
void da_arg(void);
void db_arg(void);
void dx_arg(void);
void gg_arg(void);
void aop_arg(void);
void aoerr_arg(void);
void install_arg(void);
void date_arg(void);
void record_arg(void);
void clear_arg(void);
void ci_arg(void);
void setmid_arg(void);
void cf_arg(void);
unsigned char judge_flag_chaoshi(void);



#endif

