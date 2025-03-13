int interpreter (char *command_args[], int args_size);
int help ();
int help ();
int quit ();
int set (char *var, char *value);
int print (char *var);
int source (char *script);
int badcommandFileDoesNotExist ();
int echo (char *command_args[], int args_size);

int my_mkdir (char *var);

int my_ls ();
int my_cd (char *var);
int my_touch (char *var);
int my_pwd ();
int run (char *command_args[], int args_size);
int str_isalphanum (char *name);
/////// exec
int exec (char *args[], int args_size);

///Policies

void allocate_batch_script ();
void extract_source (char *line, int current_scripts);
