/**************************************************************************
 * @file     magic.c
 * @brief    This file does two actions:
 *             1. Apend a magic number at the end of the file
 *             2. Make input file aligned with 256 bytes
 *           Info appended at the end of file
 * Author    Mahadeva HN
 ******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC_SIZE 		4
#define FOOTER_SIZE		8
#define MAX_FILE_SIZE	(1024 * 256)
#define CRC_POLYNOMIAL 	0xEDB88320

char fbuff[MAX_FILE_SIZE];

typedef struct footer{
	uint32_t crc;
	char magic[MAGIC_SIZE];
}footer_t;

uint32_t getFileSize(FILE *file)
{
	fseek( file, 0L, SEEK_END);
	uint32_t size = ftell( file );
	fseek( file, 0L, SEEK_SET);
	return size;
}

uint32_t get_crc(uint8_t *data, uint32_t len)
{
       uint32_t crc = 0;
       uint32_t  i, k, j = 0;

       crc = ~crc;
       for(k = 0; k < len; k++)
       {
               crc = crc ^ data[j++];
               for (i = 0; i < 8; i++)
               {
                       crc = (crc >> 1) ^ (CRC_POLYNOMIAL & -(crc & 1));
               }
       }
       return ~crc;
}

/* Make sure file is padded & aligned with 256 bytes -
   Internal flash minimum alignment for write */
int main(int argc, char* argv[])
{
	  FILE *binfile;
	  char *filename = NULL;
	  uint8_t buff[FOOTER_SIZE];
	  char *magic = NULL;
	  long f_size = 0;
	  footer_t *foot = (footer_t *)buff;
	  uint8_t foot_buff[MAGIC_SIZE];
	  footer_t *file_foot = (footer_t *)foot_buff;
	  int offset = 0;

	  if (argc < 3) {
		  printf("Usage: %s filename.bin bin_name\n", argv[0]);
		  exit(1);
	  }

	  filename = argv[1];

	  /* Append Magic string at the end */
	  binfile = fopen(filename, "r+b");
	  if (binfile == NULL) {
		  perror("fopen");
		  exit(1);
	  }

	  magic = argv[2];

	  f_size = getFileSize(binfile);

	  /* Move to the starting of file */
	  fseek(binfile, 0x0, SEEK_SET);

	  /* Read from the file */
	  if ((fread(fbuff, f_size, 1, binfile)) != 1) {
		  perror("fread");
		  exit(1);
	  }

	  fseek(binfile, -8, SEEK_END);
	  fread(foot_buff, FOOTER_SIZE, 1, binfile);

	  int ret = strncmp(file_foot->magic, magic, MAGIC_SIZE);

	  if (ret == 0) {
			 offset = 8;
	  } else {
			 offset = 0;
	  }

	  foot->crc = get_crc(fbuff, (f_size-offset));
	  printf("crc = 0x%x\n", foot->crc);

	  /* Copy the magic string to a buffer */
	  strncpy(&buff[4], magic, MAGIC_SIZE);
	  printf("magic = %s\n", foot->magic);

	  if (offset == 0) {
		  fseek(binfile, 0x0, SEEK_END);
	  } else {
		  fseek(binfile, -8, SEEK_END);
	  }

	  /* Write the magic string */
	  fwrite(buff, FOOTER_SIZE, 1, binfile);

	  fclose(binfile);

  return 0;
}
