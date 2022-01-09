/* Movie Studio / Vegas Pro version spoofer 
 * by Paper
 *
 * You should have received a copy of the MIT License along with this program.
 * If not, you can access it at https://opensource.org/licenses/MIT.
*/

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#else
typedef enum { false, true } bool; /* less than C99 */
#endif
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

void set_data(unsigned char magic[], uint16_t version, FILE* target) {
	int i;
	fseek(target, 0x46, SEEK_SET);
	fputc(version, target);
	for (i=0; i<=sizeof(*magic); ++i) {
		fseek(target, 0x18+i, SEEK_SET);
		fputc(magic[i], target);
	}
}

int copy_file(const char* source_file, const char* target_file) {
	FILE *source, *target;

	source = fopen(source_file, "rb");
	if (source == NULL) {
		return 1;
	}
	target = fopen(target_file, "wb");
	if (target == NULL) {
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
	int c, temp, option_index = 0;
	char buf[127];
	struct arguments {
		const char* input;
		const char* output;
		uint16_t version;
		const char* type;
	} args;
	args.input = " ";
	args.output = " ";
	args.version = -1;
	args.type = " ";

	while ((c = getopt_long(argc, argv, "i:o:v:t:h", options_long, &option_index)) != -1)
		switch(c) {
			case 'i':
				args.input = strdup(optarg);
				break;
			case 'o':
				args.output = strdup(optarg);
				break;
			case 'v':
				args.version = abs(atoi(strdup(optarg))); /* abs() for possible negative inputs */
				break;
			case 't':
				args.type = strdup(optarg);
				break;
			case 'h':
			default:
				fprintf(stderr, "msvpvf by Paper\nusage: %s -i infile [-o outfile] -v version -t [vf, veg]", argv[0]);
				return 1;
		}
	copy_file(args.input, args.output);
	FILE* outfile = fopen(args.output, "r+b");
	if (strcmp(args.type, "veg") == 0) {
		unsigned char T[] = {0xEF, 0x29, 0xC4, 0x46, 0x4A, 0x90, 0xD2, 0x11, 0x87, 0x22, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A};
		set_data(T, args.version, outfile);
	} else if (strcmp(args.type, "vf") == 0) {
		unsigned char T[] = {0xF6, 0x1B, 0x3C, 0x53, 0x35, 0xD6, 0xF3, 0x43, 0x8A, 0x90, 0x64, 0xB8, 0x87, 0x23, 0x1F, 0x7F};
		set_data(T, args.version, outfile);
	} else {
		fprintf(stderr, "Type %s is invalid!", args.type);
		goto exit;
	}
	exit:
		if (&outfile) fclose(outfile);
		if (&args) {
			free((char*)args.input);
			free((char*)args.output);
			free((char*)args.type);
		}
		return 0;
}
