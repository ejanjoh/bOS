/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-08-17
 *      Updated:    
 *
 *      Project:    bOS
 *      File name:  cli.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 10      Created
 *
 *
 *      Reference:
 *
 ******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "string.h"
#include "assert.h"
#include "io.h"
#include "process_control.h"
#include "semaphore.h"

// buffers for the system "stdin"
#define IO_IN_BUFFER_SIZE 1024
char io_in_buffer[IO_IN_BUFFER_SIZE];
io_buffer_t io_in;

// buffers for the system "stdout"
#define IO_OUT_BUFFER_SIZE 1024
char io_out_buffer[IO_OUT_BUFFER_SIZE];
io_buffer_t io_out;

#define TOKEN_SIZE  512

// Container for a tokenized command.
typedef struct {
    char token1[TOKEN_SIZE];
    char token2[TOKEN_SIZE];
    char token3[TOKEN_SIZE];
    char token4[TOKEN_SIZE];
    uint32_t cnt;
} tokens_t;


// Defines the correlation between command names and the functions handling the commands.
typedef struct {
    const char *cmd;        // The name of the command
    void (*pFunct)();       // Function pointer to the command service routine
    uint32_t elements;      // The number of token (cmd + arguments) associated to the command
} cmds_t;

extern void cmd0(void);
extern void cmd1(const char *c);
extern void cmd2(const char *c1, const char *c2);
extern void cmd3(const char *c1, const char *c2, const char *c3);

// Contain all valid commands, given from terminal, in the system...
static cmds_t cmds[] = {{"cmd0", cmd0, 1},              // test only
                        {"cmd1", cmd1, 2},              // test only
                        {"cmd2", cmd2, 3},              // test only
                        {"cmd3", cmd3, 4}};             // test only

// Mutex to protect io_out buffers
semaphore_t gMutexIoOut;

extern void (* _puts)(const char *str, const uint32_t len);
extern int32_t _printf(const uint32_t len, const char *format, ...);

// Slice upp the commands in tokens or elements
static void tokenize(tokens_t *tokens, char *line)
{
    char *pCurrToken = NULL;            // pointer to specific token within tokens
    uint32_t prev = 0;                  // previous char in command: 0 = white, 1 = writeable char

    tokens->cnt = 0;
    if (*line == '\0') return;          // not a cmd, just a new line...
    pCurrToken = tokens->token1;        // if white only, secure a token to write to...

    while ('\0' != *line) {
    
        // If previous was white and new is char, i.e. first char in a new token
        if (!prev && ('!' <= *line && *line <= '~')) {
            switch (tokens->cnt) {
                case 0:
                    pCurrToken = tokens->token1;
                    break;
                case 1:
                    pCurrToken = tokens->token2;
                    break;
                case 2:
                    pCurrToken = tokens->token3;
                    break;
                case 3:
                    pCurrToken = tokens->token4;
                    break;
                default:
                    puts("to many tokens\n", 100);
                    tokens->cnt = 0;
                    return;
            } // switch
        
            *pCurrToken = *line;
            prev = 1;
            tokens->cnt++;
        }

        // If previous was char and new is char, i.e. the same token
        else if (prev && ('!' <= *line && *line <= '~')) {
            *pCurrToken = *line;
            prev = 1;
        }
    
        // If previous was char and new is white, i.e. end of this token
        else if (prev && ('!' > *line || *line > '~')) {
            *pCurrToken = '\0';             // eol of token
            prev = 0;
        }
        
        // If previous was white and new is white, i.e. ignore white chars
        else if (!prev && ('!' > *line || *line > '~')) {
            prev = 0;
        }

        line++;
        if (prev) pCurrToken++;
    } // while ('\0' != *line)

    *pCurrToken = '\0';                     // eol of current token
    return;
}


// Do we have something that might be a command?
static uint32_t iscmd(tokens_t *tokens)
{
    if ('\0' == *tokens->token1) return 0;
    return tokens->cnt;
}


// Check if the commands exists...
static cmds_t *findcmd(const char *command)
{
    cmds_t *pCmd = NULL;
    cmds_t *pEndCmd = cmds + sizeof(cmds)/sizeof(cmds[0]);

    for (pCmd = cmds; pCmd < pEndCmd; pCmd++)
        if (0 == strncmp(pCmd->cmd, command, TOKEN_SIZE))
            return pCmd;

    return NULL;
}


// Parse the (first) token(s) to a defined command
static void parse(tokens_t *tokens)
{
    cmds_t *cmd = NULL;

    if ( (cmd = findcmd(tokens->token1)) ) {
        if (tokens->cnt != cmd->elements) {
            puts("command, number of arguments isn't correct\n", 100);
            return;
        }

        // execute the command
        switch (cmd->elements) {
            case 1:
                (*(cmd->pFunct))();
                break;
            case 2:
                (*(cmd->pFunct))(&tokens->token2);
                break;
            case 3:
                (*(cmd->pFunct))(&tokens->token2, &tokens->token3);
                break;
            case 4:
                (*(cmd->pFunct))(&tokens->token2, &tokens->token3, &tokens->token4);
                break;
            default:
                ASSERT(0);
                break;
        }
    } // if (...)
    else {
        puts("command not found\n", 100);
    }
    return;
}


void cli_io_init(void)
{
    _io_buffer_init(&io_in, io_in_buffer, IO_IN_BUFFER_SIZE);
    _io_buffer_init(&io_out, io_out_buffer, IO_OUT_BUFFER_SIZE);
    
    semaphore_init(&gMutexIoOut, MUTEX_INIT_VALUE);
    return;
}


void cli_io_in_proc(void)
{
    char line[IO_IN_BUFFER_SIZE];
    tokens_t tokens;

    
    while (1) {
        if (_io_buffer_get_lines(&io_in)) {
            //_printf(100, "proc1a (%u)\n",  io_in.lines);
            _io_buffer_read_string(&io_in, line);
            // Redirect the input if needed to getc/getchar, gets etc...
            tokenize(&tokens, line);
            if (iscmd(&tokens)) parse(&tokens);
        }
        else {
            //_printf(100, "proc1b (%u)\n",  io_in.lines);
            proc_ctrl_context_switch(blocked);
        }
    } // while (1)

    ASSERT(0);
    return;
}


void cli_io_out_proc(void)
{
    char line[IO_OUT_BUFFER_SIZE];


    while (1) {
        if (_io_buffer_get_lines(&io_out)) {
            //_printf(100, "proc2a (%u)\n",  io_out.lines);
            _io_buffer_read_string(&io_out, line);
            _puts(line, IO_OUT_BUFFER_SIZE);
        }
        else {
            //_printf(100, "proc2b (%u)\n",  io_out.lines);
            proc_ctrl_context_switch(blocked);
        }
    }

    return;
}








