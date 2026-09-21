#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */


typedef struct _Decode{
    char *steg_fname ;
    FILE *fptr_stego;

    //sec file
    char sec_fname[60];
    FILE *fptr_sec;
    int sec_fsize;

    
}Decode;

/* Encoding function prototype */

/* Check operation type */
//OperationType check_operation_type(char *argv[]);

Status read_and_validate_decode_args(char *argv[], Decode *decode);

Status do_decoding(Decode *decode);

Status check_pass(Decode *decode);

Status file_size_name_decode(Decode *decode);

Status get_secret_info_size( Decode *decode);

Status create_sec_file(Decode *decode);

Status decode_sec_data(Decode *decode);
char decode_lsb_byte(char *buffer,int size);

int decode_lsb(char *buffer,int size);

void show_spinner(const char *message);

#endif