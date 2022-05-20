#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <libgen.h>
#ifdef _MSC_VER
#define strdup(p) _strdup(p)
#endif

static struct option options_long[] = {
	{"input", required_argument, NULL, 'i'},
	{"output", required_argument, NULL, 'o'},
	{"version", required_argument, NULL, 'v'},
	{"type", required_argument, NULL, 't'},
	{"help", 0, NULL, 'h'}
};

char* strremove(char* str, const char* sub) {
    size_t len = strlen(sub);
    if (len > 0) {
        char* p = str;
        while ((p = strstr(p, sub)) != NULL) {
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }
    return str;
}

void set_data(unsigned char magic[], uint16_t version, FILE* target) {
	int i;
	fseek(target, 0x46, SEEK_SET);
	fputc(version, target);
	for (i=0; i<=sizeof(*magic); ++i) {
		fseek(target, 0x18+i, SEEK_SET);
		fputc(magic[i], target);
	}
}

int copy_file(char* source_file, char* target_file) {
	FILE *source, *target;

	source = fopen(source_file, "rb");

	if (source == NULL) return 1;

	target = fopen(target_file, "wb");

	if (target == NULL) {
		fclose(source);
		return 1;
	}

	size_t n, m;
	unsigned char buff[8192];
	do {
		n = fread(buff, 1, sizeof(buff), source);
		if (n) m = fwrite(buff, 1, n, target);
		else m = 0;
	} while ((n > 0) && (n == m));

	fclose(target);
	fclose(source);
	return 0;
}

int main(int argc, char *argv[]) {
	int c, option_index = 0;
	unsigned char magic[16];
	FILE* outfile;
	struct arguments {
		char input[256];
		char output[256];
		int version;
		char type[256];
	} args;
	strcpy(args.input, " ");
	strcpy(args.output, " ");
	args.version = -1;
	strcpy(args.type, " ");

	while ((c = getopt_long(argc, argv, "i:o:v:t:h", options_long, &option_index)) != -1)
		switch(c) {
			case 'i':
				strncpy(args.input, optarg, sizeof(args.input)-1);  /* subtract 1 to make sure it's "null-safe" */
				break;
			case 'o':
				strncpy(args.output, optarg, sizeof(args.input)-1);
				break;
			case 'v':
				args.version = abs(atoi(strdup(optarg)));  /* abs() for possible negative inputs */
				break;
			case 't':
				strncpy(args.type, optarg, sizeof(args.input)-1);
				break;
			case 'h':
			default:
				printf("msvpvf by Paper\nusage: %s (-i/--input) infile [(-o/--output) outfile] (-v/--version) version (-t/--type) [vf, veg]\n", argv[0]);
				return 0;
		}
	if (argc == 1) {
		printf("msvpvf by Paper\nusage: %s (-i/--input) infile [(-o/--output) outfile] (-v/--version) version (-t/--type) [vf, veg]\n", argv[0]);
		return 0;
	}
	if (strcmp(args.input, " ") == 0) {
		printf("Input file name?\n");
		fflush(stdout);
		fgets(args.input, sizeof(args.input)-1, stdin);
		args.input[strcspn(args.input, "\r\n")] = 0;
	}
	if (access(args.input, F_OK) != 0) {
		fprintf(stderr, "Input file \"%s\" doesn't exist! Exiting.", args.input);
		return 1;
	}
	FILE* input_file = fopen(args.input, "r");
	if (fgetc(input_file) == EOF) {
		fprintf(stderr, "Input file \"%s\" is empty.", args.input);
		fclose(input_file);
		return 1;
	}
	fseek(input_file, 0x46, SEEK_SET);
	printf("Input file version: %d\n", fgetc(input_file));
	fseek(input_file, 0x18, SEEK_SET);
	int file_version = fgetc(input_file);
	printf("Input file type: ");
	switch (file_version) {
		case 0xEF:
			printf("VEGAS Pro\n\n");
			break;
		case 0xF6:
			printf("Movie Studio\n\n");
			break;
		default:
			printf("Unknown\n\n");
			break;
	}
	int* ptr = &args.version;
	if (args.version == -1) {
		printf("What version of VEGAS would you like to spoof to?: ");
		fflush(stdout);
		scanf("%d", ptr);
	}
	if (strcmp(args.type, " ") == 0) {
		printf("Would you like it to be VEGAS Pro or Movie Studio? [veg/vf]: ");
		fflush(stdout);
		scanf("%3s", args.type);
	}
	fflush(stdout);
	if (strcmp(args.output, " ") == 0) { /* string manipulation hell */
		char* temp = (char*)calloc(256, sizeof(char));
		temp[0] = '\0';
		char str_version[16];
		sprintf(str_version, "V%d", args.version);
		strncat(temp, str_version, 2);
		strncat(temp, "_", 1);
		strncat(temp, basename(args.input), 248);
		strcpy(temp, strremove(temp, strrchr(basename(args.input), ('.')))); /* remove file extension */
		strncat(temp, ".", 1);
		strncat(temp, args.type, 3);
		strncpy(args.output, temp, 255);
		free(temp);
	}
	if (strcmp(args.type, "veg") == 0) {
		const unsigned char T[] = {0xEF, 0x29, 0xC4, 0x46, 0x4A, 0x90, 0xD2, 0x11, 0x87, 0x22, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A};
		for (option_index = 0; option_index <= 15; option_index++) { /* this line is repeated, but it's probably best to just leave it be. */
			magic[option_index] = T[option_index];
		}
	} else if (strcmp(args.type, "vf") == 0) {
		const unsigned char T[] = {0xF6, 0x1B, 0x3C, 0x53, 0x35, 0xD6, 0xF3, 0x43, 0x8A, 0x90, 0x64, 0xB8, 0x87, 0x23, 0x1F, 0x7F};
		for (option_index = 0; option_index <= 15; option_index++) {
			magic[option_index] = T[option_index];
		}
	} else {
		fprintf(stderr, "Type %s is invalid!", args.type);
		return 1;
	}
	copy_file(args.input, args.output);
#ifdef _WIN32 /* disallowed characters in filenames */
	if (strcspn(args.input, "<>:\"/\\|?*") == strlen(args.input)+1) {
#elif defined(__unix__)
	if (strcspn(args.input, "/") == strlen(args.input)+1) {
#else
	if (NULL) {
#endif
		fprintf(stderr, "Invalid output filename detected! Exiting...");
		return 1;
	}
	outfile = fopen(args.output, "r+b");
	if (outfile == NULL) {
		fprintf(stderr, "Failed to open file %s! Do you have write permissions?", args.output);
		return 1;
	}
	set_data(magic, args.version, outfile);
	fclose(outfile);
	return 0;
}
