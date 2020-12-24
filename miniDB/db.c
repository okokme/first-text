#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char* buffer;
  size_t buffer_length;
  size_t input_length;
} InputBuffer;

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGIZED_COMMAND
} MetaCommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum {
    STATEMENT_SELECT,
    STATEMENT_INSERT
}StatementType;

typedef struct {
    StatementType type;
}Statement;

InputBuffer* new_input_buffer() {
  InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}

void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

void print_prompt() {
    printf("db ->");
}

void read_input(InputBuffer* input_buffer) {
    size_t bytes_read = getline(&(input_buffer->buffer),&(input_buffer->buffer_length), stdin);
    if (input_buffer->buffer_length <= 0) {
        printf("Illegal input");
        close_input_buffer(input_buffer);
        exit(0);
    }
    input_buffer->buffer[bytes_read-1] = 0;
    input_buffer->buffer_length = bytes_read - 1;
}

MetaCommandResult do_meta_commnd(InputBuffer* input_buffer) {
    if (strcmp(input_buffer->buffer, ".q") == 0) {
        close_input_buffer(input_buffer);
        exit(META_COMMAND_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGIZED_COMMAND;
    }
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement) {
    switch(statement->type) {
        case STATEMENT_SELECT: // select
            printf("This is where we would do a select\n");
            break;
        case STATEMENT_INSERT: // insert
            printf("This is where we would do a insert\n");
            break;
    }
}

int main(int argc, char const *argv[])
{
    InputBuffer* input_buffer = new_input_buffer();
    while(1) {
        print_prompt();
        read_input(input_buffer);
        // 系统命令以.开头
        if (input_buffer->buffer[0] == '.') {
            switch (do_meta_commnd(input_buffer)) { // 解析命令
                case META_COMMAND_SUCCESS: //  成功，下一个继续
                    continue;
                case META_COMMAND_UNRECOGIZED_COMMAND: // 失败，报错，下一个继续
                    printf("unrecognized command '%s'\n",input_buffer->buffer);
                    continue;
            }
        }  
        // 进入SQL编译器
        Statement statement;
        switch(prepare_statement(input_buffer, &statement)) {
            case PREPARE_SUCCESS:
                break;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                printf("Unrecognized keyword at start of ’%s‘.\n", input_buffer->buffer);
                continue;
        }
        // 执行任务
        execute_statement(&statement);
        printf("Executed.\n");
    } 
    return 0;
}
