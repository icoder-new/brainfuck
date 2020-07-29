/*
* Simple brainfuck2c converter on pure C
* Coded by Ehsonjon (a.k.a iCoder)
* DATE: 23.07.2020 (18:03) Tajikistan Asia Time =D
* (C) 2020-2021 All rights reversed!
*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define STRING_SIZE 1024

typedef struct CommandOptions
{
    bool doCompile;
    bool deleteSource;
    char * fileName;
    char outFileName[STRING_SIZE];
    char outFileC[STRING_SIZE];
} CommandOptions;

bool parse_command_line(CommandOptions * options, int argc, char * argv[])
{
    //help message
    if (argc == 1) {
        fprintf(stderr, "\n");
        fprintf(stderr, "How to use : %s filename [-o output|-c|-d|-r]\n",argv[0]);
        fprintf(stderr, "-o output : Set output file name\n-c : Do not compile\n-d : Do not delete C source file\n");
        fprintf(stderr, "[INFO] : You SHOULD type 'export MALLOC_CHECK_=0' manually to remove warning.\n");
        fprintf(stderr, "\n");
        return false;
    }

    // set the default options
    options->doCompile = true;
    options->deleteSource = true;
    options->fileName = argv[1];

    strncpy(options->outFileName, options->fileName, STRING_SIZE);
    strncat(options->outFileName, ".o", STRING_SIZE);

    // parse the remaining options
    int i;
    bool isSetOut = false;

    for (i = 0; i < argc; i++) 
    {
        if (isSetOut){
            isSetOut = false;
            strncpy(options->outFileName, argv[i], STRING_SIZE);
        }
        else if (strcmp(argv[i],"-c") == 0) {
            options->doCompile = false;
        }
        else if (strcmp(argv[i],"-d") == 0) {
            options->deleteSource = false;
        }
        else if (strcmp(argv[i],"-o") == 0) {
            isSetOut = true;
        }
    }

    strncpy(options->outFileC,options->outFileName,STRING_SIZE);
    strncat(options->outFileC,".c", STRING_SIZE);

    // don't delete the source if we won't compile it
    if(!options->doCompile)
    {
        options->deleteSource = false;
    }

    return true;

}

void write_header(FILE * cFile)
{
    fputs("#include <stdio.h>\n",  cFile);
    fputs("#include <stdlib.h>\n", cFile);
    fputs("int main(){\n",         cFile);
    fputs("unsigned char* _=(unsigned char*)malloc(32*1024);/*32kB*/if(_==0){printf(\"MEMORY ERROR!\\n\");return 1;}\n",cFile);
}

void write_footer(FILE * cFile)
{
    fputs("free(_);\nreturn 0;\n}\n", cFile);
}

int bf_fgetc(FILE * bfFile)
{
    int c;
    do {
        c = fgetc(bfFile);
    } while(c != EOF && c != '[' && c != ']' && c != '<' && c != '>' && c != '.' 
        && c != ',' && c != '+' && c != '-');
    return c;
}

void compile_to_c(FILE * bfFile, FILE * cFile)
{
    write_header(cFile);
    int add = 0;
    char prevC = '\0';
    //write codes   
    char c = bf_fgetc(bfFile);
    do {
        int movement_counter = 0;
        while ( c == '>' || c == '<')
        {
            movement_counter += c == '>' ? 1 : -1;
            c = bf_fgetc(bfFile);
        }
        if (movement_counter)
        {
            fprintf(cFile,"_ += %d;", movement_counter);
        }

        int value_counter = 0;
        while ( c == '+' || c == '-')
        {
            value_counter += c == '+' ? 1 : -1;
            c = bf_fgetc(bfFile);
        }
        if (value_counter)
        {
            fprintf(cFile,"*_ += %d;",value_counter);
        }

        if (c == '.') 
        {
            fprintf(cFile, "putchar(*_);\n"); 
            c = bf_fgetc(bfFile); 
        }
        if (c == ',') 
        {
            fprintf(cFile, "*_ = getchar();\n");
            c = bf_fgetc(bfFile); 
        }
        if (c == '[')
        {
            fprintf(cFile, "while(*_) {\n");
            c = bf_fgetc(bfFile);
        }
        if (c == ']')
        {
            fprintf(cFile, "}\n");
            c = bf_fgetc(bfFile);
        }
    } while(c!=EOF);
    write_footer(cFile);
}

void compileCode(CommandOptions * options)
{
    printf("Compile with GCC...\n");
    char op[2048] = "gcc ";
    strncat(op,options->outFileC, 2048);
    strncat(op," -o ", 2048);
    strncat(op,options->outFileName, 2048);
    system(op);
}

int main(int argc, char* argv[]){
    CommandOptions options;

    if( !parse_command_line(&options, argc, argv) )
    {
        return 1;
    }

    printf("[INFO] : You may type 'export MALLOC_CHECK_=0' manually to remove warning.\n");

    //bf file
    FILE* bfFile = fopen(options.fileName,"r");
    if(bfFile==NULL){
        fprintf(stderr, "[ERROR] : FILE %s DOES NOT EXIST\n",options.fileName);
        return 2;
    }

    //c source code
    FILE* cFile = fopen(options.outFileC, "w");
    if(!cFile)
    {
        fclose(bfFile);
        fprintf(stderr, "[ERROR]: COULD NOT OPEN %s FILE FOR WRITING" , options.outFileC);
        return 3;
    }

    compile_to_c(bfFile, cFile);

    fclose(bfFile);
    fclose(cFile);

    if(options.doCompile){
        compileCode(&options);
    }else{
        printf("Output C code : %s\n", options.outFileC);
    }

    if( options.deleteSource )
    {
        unlink(options.outFileC);
    }

    printf("Done.\nOutput file name : %s\n",options.outFileName);
    return 0;
}