#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"
#include <string.h>

#include <unistd.h>

// Spinner animation
void show_spinner(const char *message)
{
    const char *spinner[] = {"◐","◓","◑","◒"};
    for (int i = 0; i <8 ; i++)   // spin 2 full cycles
    {
        printf("\r%s %s", message, spinner[i%4]);
        fflush(stdout);
        usleep(100000); // 0.1 second
    }
    printf("\r%s [OK]\n", message);
}

int main(int argc, char *argv[])
{
    if(argc<2)
    {
        printf("Choose your option (encoding/decoding) -e or -d\n");
        printf("with respective files \n"); 
        return 0;
    }
    OperationType st;
    st=check_operation_type(argv);
    if(st==e_encode)
    {
        if(argc<4)
        {
            printf("Usage: %s -e <src.bmp> <secret_file> [stego_output.bmp]\n", argv[0]);
            return -1;
        }
        EncodeInfo encInfo;
        if(read_and_validate_encode_args(argv,&encInfo)==e_success)
        {
           if(do_encoding(&encInfo)==e_failure)
           {
                printf("Encoding Failed \n");
                return -1;
           }
        }
        else
        {
            return -1;
        }
    }
    else if(st==e_decode)
    {
        if(argc<3)
        {
            printf("Usage: %s -d <stego.bmp>\n", argv[0]);
            return -1;
        }
        Decode decode;
        if(read_and_validate_decode_args(argv,&decode)==e_success)
        {
           if(do_decoding(&decode)==e_failure)
           {
                printf("Decoding Failed \n");
                return -1;
           }
        }
        else
        {
            return -1;
        }
    }
    else
    {
        printf("Error : provide valid option \n");
        return -1;
    }   

    return 0;
}
OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0) return e_encode;
    if (strcmp(argv[1], "-d") == 0) return e_decode;
    return e_unsupported;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // --- Validate source BMP file ---
    char *ext = strrchr(argv[2], '.');
    if (!(ext && strcmp(ext, ".bmp") == 0))
    {
        printf("Error: source file must be a .bmp file\n");
        return e_failure;
    }
    encInfo->src_image_fname = argv[2];

    // --- Validate secret TXT file ---
    ext = strrchr(argv[3], '.');
    if (!ext)
    {
        printf("Error: secret file must have an extension\n");
        return e_failure;
    }

    // Allowed secret file extensions
    const char *allowed_exts[] = {".txt", ".pdf", ".docx", ".png", ".jpg"};
    int valid = 0;
    for (int i = 0; i < sizeof(allowed_exts)/sizeof(allowed_exts[0]); i++)
    {
        if (strcmp(ext, allowed_exts[i]) == 0)
        {
            valid = 1;
            break;
        }
    }

    if (!valid)
    {
        printf("Error: unsupported secret file type '%s'\n", ext);
        return e_failure;
    }

    encInfo->secret_fname = argv[3];
    strcpy(encInfo->extn_secret_file, ext); // store extension (e.g., ".pdf")

    // --- Validate stego BMP file (optional) ---
    if (argv[4] != NULL)
    {
        ext = strrchr(argv[4], '.');
        if (!(ext && strcmp(ext, ".bmp") == 0))
        {
            printf("Error: destination file must b e a .bmp file\n");
            return e_failure;
        }
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "stego_img.bmp"; // default
    }

    return e_success;
}
Status read_and_validate_decode_args(char *argv[], Decode *decode)
{
    // --- Validate  BMP file ---
    char *ext = strrchr(argv[2], '.');
    if (!(ext && strcmp(ext, ".bmp") == 0))
    {
        printf("Error: source file must be a .bmp file\n");
        return e_failure;
    }
    decode->steg_fname =argv[2];
    return e_success;
}