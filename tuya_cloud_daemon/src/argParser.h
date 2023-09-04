#ifndef ARGPARSER_H
#define ARGPARSER_H

#define DATA_LEN 30

typedef struct Arguments
{
    char *productId;
    char *deviceId;
    char *deviceSecret;
} Arguments;

void parse_arguments(int argc, char **argv, Arguments *args);

#endif