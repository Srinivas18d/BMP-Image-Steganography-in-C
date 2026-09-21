#include <stdio.h>
#include "types.h"
#include "common.h"
#include "decode.h"
#include <string.h>


Status do_decoding(Decode *decode)
{
    Status result = e_success;

    printf("\n=== Steganography Decoding Process ===\n");
    printf("Stego Image : %s\n\n", decode->steg_fname);

    decode->fptr_stego = fopen(decode->steg_fname, "rb");
    if (decode->fptr_stego == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decode->steg_fname);
        return e_failure;
    }
    show_spinner("Opening stego image");

    if (check_pass(decode) == e_failure)
    {
        printf("[ERROR] Password check failed\n");
        result = e_failure; goto cleanup;
    }
    show_spinner("Checking password");

    if (file_size_name_decode(decode) == e_failure)
    {
        printf("[ERROR] File extension decode failed\n");
        result = e_failure; goto cleanup;
    }
    show_spinner("Decoding file extension");

    if (get_secret_info_size(decode) == e_failure)
    {
        printf("[ERROR] Secret file size decode failed\n");
        result = e_failure; goto cleanup;
    }
    show_spinner("Decoding secret file size");

    if (create_sec_file(decode) == e_failure)
    {
        printf("[ERROR] Secret file creation failed\n");
        result = e_failure; goto cleanup;
    }
    show_spinner("Creating secret file");

    if (decode_sec_data(decode) == e_failure)
    {
        printf("[ERROR] Secret file data decode failed\n");
        result = e_failure; goto cleanup;
    }
    show_spinner("Decoding secret file data");

    printf("\n========== Decoding Completed Successfully ==========\n");
    printf("Decoded secret saved as: %s\n", decode->sec_fname);

cleanup:
    fclose(decode->fptr_stego);
    return result;
}

Status check_pass(Decode *decode)
{
    char s[strlen(MAGIC_STRING)+1];
    fseek(decode->fptr_stego,54,SEEK_SET);
    int i;
    for( i=0;i<strlen(MAGIC_STRING);i++)
    {
        char buff[8];
        fread(buff,8,1,decode->fptr_stego);
        s[i]=decode_lsb_byte(buff,8);
    }
    s[i]='\0';
    if(strcmp(MAGIC_STRING,s)==0)
    {
        printf("==============================Password mathced============================== \n");
        return e_success;
    }
    return e_failure;
}
char decode_lsb_byte(char *buffer,int size)
{
    unsigned char ch=0;
    for(int i=0;i<size;i++)
    {
        unsigned char bit = buffer[i] & 1;
        ch |= (bit << (size -1 - i));
    }
    return ch;
}

Status file_size_name_decode(Decode *decode)
{
    char buff[32];
    fread(buff, 32, 1, decode->fptr_stego);
    int size = decode_lsb(buff, 32);

    // Reject clearly invalid or malicious extension sizes
    if (size < 2 || size > 10)
    {
        fprintf(stderr, "ERROR: invalid extension size decoded (%d)\n", size);
        return e_failure;
    }

    char ext[size + 1];
    int i;
    for (i = 0; i < size; i++)
    {
        char buff1[8];
        fread(buff1, 8, 1, decode->fptr_stego);
        ext[i] = decode_lsb_byte(buff1, 8);
    }
    ext[i] = '\0';

    char name[50];
    printf("The Encoded Secret file type : %s\nEnter the name of your file where you want to store secret data : ", ext);
    scanf(" %49s", name);

    // Ensure name + ext will fit in decode->sec_fname before combining
    if (strlen(name) + strlen(ext) >= sizeof(decode->sec_fname))
    {
        fprintf(stderr, "ERROR: combined filename too long for buffer\n");
        return e_failure;
    }

    strcpy(decode->sec_fname, name);
    strcat(decode->sec_fname, ext);

    printf("The Secret file : %s\n", decode->sec_fname);
    return e_success;
}

int decode_lsb(char *buffer,int size)
{
    unsigned int w=0;
    for(int i=0;i<size;i++)
    {
        unsigned int bit = buffer[i] & 1;
        w |= (bit << (size - i-1));
    }
    return w;
}
Status get_secret_info_size( Decode *decode)
{
    char buff[32];
    fread(buff,32,1,decode->fptr_stego);
    decode ->sec_fsize =decode_lsb(buff,32);
    printf("secret data file size : %d bytes \n",decode ->sec_fsize);
    if(decode ->sec_fsize <= 0)
    {
        fprintf(stderr,"ERROR: invalid secret file size decoded (%d)\n", decode->sec_fsize);
        return e_failure;
    }

    /* Sanity check: a corrupted or malicious stego image could claim a huge
     * size. Each encoded byte consumes 8 bytes of image data, so make sure
     * that many bytes actually remain in the file from the current position. */
    long current_pos = ftell(decode->fptr_stego);
    long saved_pos = current_pos;
    fseek(decode->fptr_stego, 0, SEEK_END);
    long remaining = ftell(decode->fptr_stego) - saved_pos;
    fseek(decode->fptr_stego, saved_pos, SEEK_SET);

    if(current_pos < 0 || remaining < 0 || (long)decode->sec_fsize * 8 > remaining)
    {
        fprintf(stderr,"ERROR: decoded secret file size (%d bytes) exceeds data available in image\n", decode->sec_fsize);
        return e_failure;
    }

    return e_success;
}
Status create_sec_file(Decode *decode)
{
    decode ->fptr_sec=fopen(decode ->sec_fname,"wb");
    if (decode ->fptr_sec == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decode ->sec_fname);

    	return e_failure;
    }
    return e_success;
}
Status decode_sec_data(Decode *decode)
{
    int i;char ch;
    for(  i=0;i<decode->sec_fsize;i++)
    {
        char buff1[8];
        if(fread(buff1,8,1,decode->fptr_stego)!=1)
        {
            fprintf(stderr,"ERROR: stego image ran out of data while decoding\n");
            fclose(decode->fptr_sec);
            return e_failure;
        }
        ch=decode_lsb_byte(buff1,8);
        fwrite(&ch,1,1,decode->fptr_sec);
    }
    fclose(decode->fptr_sec);
    return e_success;
}