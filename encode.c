#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"
#include <string.h>



/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }


    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
    //printf("image_capacity : %u \n",encInfo->image_capacity);
    //printf("size_secret_file : %ld \n",encInfo->size_secret_file);
    if((encInfo->image_capacity) >
       ((strlen(MAGIC_STRING)*8)               /* magic string        */
        + 32                                    /* extension length field */
        + (strlen(encInfo->extn_secret_file)*8) /* extension bytes     */
        + 32                                    /* secret file size field */
        + ((encInfo->size_secret_file)*8)))     /* secret file data    */
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
Status do_encoding(EncodeInfo *encInfo)
{
    Status result = e_success;

    encInfo->fptr_src_image = NULL;
    encInfo->fptr_secret = NULL;
    encInfo->fptr_stego_image = NULL;

    printf("\n=== Steganography Encoding Process ===\n");
    printf("Source Image : %s\n", encInfo->src_image_fname);
    printf("Secret File  : %s\n", encInfo->secret_fname);
    printf("Output Image : %s\n\n", encInfo->stego_image_fname);

    if(open_files(encInfo)==e_failure)
    {
        printf("[ERROR] Problem opening files\n");
        goto cleanup;
    }
    show_spinner("Opening files");

    if(check_capacity(encInfo)==e_failure)
    {
        printf("[ERROR] Source image does not have enough capacity\n");
        result = e_failure; goto cleanup;
    }
    show_spinner("Checking capacity");

    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        printf("[ERROR] BMP header copy failed\n");
        result = e_failure; goto cleanup;
    }
    show_spinner("Copying BMP header");

    if(encode_magic_string(MAGIC_STRING, encInfo)==e_failure)
    {
        printf("[ERROR] Magic string encoding failed\n");
        result = e_failure; goto cleanup;
    }
    show_spinner("Encoding magic string");

    if(encode_ext_secret_file_size((int)strlen(encInfo->extn_secret_file),encInfo)==e_failure)
    {
        printf("[ERROR] Extension size encoding failed\n");
        result = e_failure; goto cleanup;
    }
    show_spinner("Encoding extension size");

    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_failure)
    {
        printf("[ERROR] Extension encoding failed\n");
        result = e_failure; goto cleanup;
    }
    show_spinner("Encoding extension");

    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_failure)
    {
        printf("[ERROR] Secret file size encoding failed\n");
        result = e_failure; goto cleanup;
    }
    show_spinner("Encoding secret file size");

    if(encode_secret_file_data(encInfo)==e_failure)
    {
        printf("[ERROR] Secret file data encoding failed\n");
        result = e_failure; goto cleanup;
    }
    show_spinner("Encoding secret file data");

    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        printf("[ERROR] Remaining image data copy failed\n");
        result = e_failure; goto cleanup;
    }
    show_spinner("Copying remaining image data");

    printf("\n========== Encoding Completed Successfully ==========\n");
    printf("Encoded image saved as: %s\n", encInfo->stego_image_fname);

cleanup:
    if(encInfo->fptr_src_image)    { fclose(encInfo->fptr_src_image);    encInfo->fptr_src_image = NULL; }
    if(encInfo->fptr_secret)       { fclose(encInfo->fptr_secret);       encInfo->fptr_secret = NULL; }
    if(encInfo->fptr_stego_image)  { fclose(encInfo->fptr_stego_image);  encInfo->fptr_stego_image = NULL; }
    return result;
}


uint get_file_size(FILE *fptr)
{
    
    uint size;
    fseek(fptr, 0, SEEK_END);   // move to end
    size = ftell(fptr);         // get position = size
    fseek(fptr, 0, SEEK_SET);   // reset to beginning
    return size;
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);
    rewind(fptr_dest_image);
    char head[54];
    if(fread(head,1, 54, fptr_src_image)!=54)
    {
        printf("read fail\n");
        return e_failure;
    }
    if(fwrite(head,1, 54,fptr_dest_image)!=54)
    {

        printf("write fail\n");
        return e_failure;
    }
    return e_success;
    
}
// Generic function to conceal any data type into LSBs
Status encode_to_lsb(uint data, unsigned char *buffer, int num_bits)
{
    for (int i = 0; i < num_bits; i++) {
        buffer[i] &= ~1;
        buffer[i] |= (data >> (num_bits - 1 - i)) & 1;
    }
    return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char buff[sizeof(*magic_string)*8];
    for(int i=0;i<strlen(magic_string);i++)
    {
        fread(buff,8,1,encInfo->fptr_src_image);
        encode_to_lsb(magic_string[i],buff,8);
        fwrite(buff,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}
Status encode_ext_secret_file_size(int size, EncodeInfo *encInfo)
{
    char buff1[sizeof(size)*8];
    fread(buff1,sizeof(size)*8,1,encInfo->fptr_src_image);
    encode_to_lsb(size,buff1,sizeof(size)*8);
    fwrite(buff1,sizeof(size)*8,1,encInfo->fptr_stego_image);
    return e_success;

}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char buff2[8];
    for(int i=0;i<strlen(encInfo->extn_secret_file);i++)
    {
        fread(buff2,8,1,encInfo->fptr_src_image);
        encode_to_lsb(file_extn[i],buff2,8);
        fwrite(buff2,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    char buff3[sizeof(file_size)*8];
    fread(buff3,sizeof(file_size)*8,1,encInfo->fptr_src_image);
    encode_to_lsb(file_size,buff3,sizeof(file_size)*8);
    fwrite(buff3,sizeof(file_size)*8,1,encInfo->fptr_stego_image);
    return e_success;
    
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    /* Stream the secret file one byte at a time rather than buffering the
     * whole file in a stack array sized by (untrusted) file length, which
     * could exhaust the stack for large secret files. */
    char buff_sr[8];
    for(int i=0;i<encInfo->size_secret_file;i++)
    {
        char ch;
        if(fread(&ch,1,1,encInfo->fptr_secret)!=1)
        {
            fprintf(stderr,"ERROR: failed to read secret file data\n");
            return e_failure;
        }
        if(fread(buff_sr,8,1,encInfo->fptr_src_image)!=1)
        {
            fprintf(stderr,"ERROR: source image ran out of data while encoding\n");
            return e_failure;
        }
        encode_to_lsb(ch,buff_sr,8);
        fwrite(buff_sr,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while(fread(&ch,1,1, fptr_src)>0)
    {
        fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;
}
