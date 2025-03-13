#define MEM_SIZE 1000
#define MAX_LINE_LENGTH 100
#define MAX_SCRIPTS 3
void mem_init ();
char *mem_get_value (char *var);
void mem_set_value (char *var, char *value);




struct memory_struct {
    char *var;
    char *value;
};
/**  
Script data structure and script memory to store the scripts
**/

typedef struct {
    char **content;
    size_t lines;
} Script;


typedef struct {
    Script *scripts;
    size_t totalLines;
    size_t totalScripts;
    size_t capacity;
} Script_memory;
//
extern Script_memory scriptmemory;

//Script memory methods 

void script_init ();
void script_free ();
void script_free_index (int index);
char **get_script_content (int index);
int script_add_script (const char *source);
void script_load_script (int script_id, const char *source);

int count_lines (const char *filename);

//Background batch script helper :



///Helper debug 
void print_script ();
