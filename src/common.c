#include <stdio.h>
#include <stdint.h>

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
	char ch[4096];
	FILE *source, *target;

	source = fopen(source_file, "rb");

	if (source == NULL)
		return 1;

	target = fopen(target_file, "wb");

	if (target == NULL) {
		fclose(source);
		return 1;
	}

	while (!feof(source)) {
		size_t b = fread(ch, 1, sizeof(ch), source);
		if (b)
			fwrite(ch, 1, b, target);
	}

	fclose(target);
	fclose(source);
	return 0;
}
