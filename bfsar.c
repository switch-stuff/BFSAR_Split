#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s file.bfsar", argv[0]);
		return 1;
	}

	FILE *in = fopen(argv[1], "rb");

	char *outDir = malloc(strlen(argv[1]) + 10);

	sprintf(outDir, "%s_extracted", argv[1]);

	mkdir(outDir, 0777);

	fseek(in, 0, 2);
	int fsize = ftell(in);
	rewind(in);

	while (!feof(in)) {
		int32_t magic, doRound = 1;
		fread(&magic, sizeof(int32_t), 1, in);

		char ext[6];

		if (magic == 0x56415746)
			strcpy(ext, "bfwav");
		else if (magic == 0x50545346)
			strcpy(ext, "bfstp");
		else if (magic == 0x51455346)
			strcpy(ext, "bfseq");
		else
			doRound = 0;

		if (doRound) {
			int64_t pos;
			int32_t size;

			fread(&pos, sizeof(int64_t), 1, in);
			fread(&size, sizeof(int32_t), 1, in);

			uint8_t *data = malloc(size - 0x10);
			fread(data, 1, size - 0x10, in);

			int relPos = ftell(in) - size;

			char range[0x20], name[0x200];

			sprintf(range, "0x%x-0x%x", relPos, relPos + size);

			printf("Extracting %s at %s...\n", ext, range);

			sprintf(name, "%s/%s.%s", outDir, range, ext);

			FILE *out = fopen(name, "wb");

			fwrite(&magic, sizeof(int32_t), 1, out);		
			fwrite(&pos, sizeof(int64_t), 1, out);
			fwrite(&size, sizeof(int32_t), 1, out);
			fwrite(data, 1, size - 0x10, out);

			free(data);

			fclose(out);

			if (relPos + size == fsize)
				break;
			else
				fseek(in, ftell(in) - ftell(in) % 4, 0);
		}
	}

	puts("\nDone!");
	fclose(in);

	return 0;
}