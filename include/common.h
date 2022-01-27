#include <unistd.h>
#include <getopt.h>

static struct option options_long[5];
char* strremove(char* str, const char* sub);
void set_data(unsigned char magic[], uint16_t version, FILE* target);
int copy_file(char* source_file, char* target_file);
