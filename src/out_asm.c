#include "out_asm.h" 

#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "bf_file.h"
#include "stack.h"


int add_asm_line(asm_out *out, const char* line) {
    size_t len = strlen(line);
    uint8_t *tmp = realloc(out->buffer, out->count + len);
    if (!tmp) {
        return -1;
    }
    out->buffer = tmp;
    memcpy(out->buffer + out->count, line, len);
    out->count += len;

    return len;
}

static inline void creat_prolog(asm_out *out) {
    add_asm_line(out, "format ELF64 executable\n");
    add_asm_line(out, "entry _start\n");
    add_asm_line(out, "\n");
    add_asm_line(out, "segment readable executable\n");

    add_asm_line(out, "_start:\n");
    add_asm_line(out, "    mov    rsi, buffer\n");
}

static inline void creat_epilogue(asm_out *out) {
    add_asm_line(out, "    mov    rax, 60\n");
    add_asm_line(out, "    xor    rdi, rdi\n");
    add_asm_line(out, "    syscall\n");
    add_asm_line(out, "\n");
    add_asm_line(out, "segment readable writeable\n");
    add_asm_line(out, "\n");
    add_asm_line(out, "buffer rb 30000 ; reserve 30,000 bytes (≈30 KB)\n");
}

static inline void inc_handle(asm_out *out, uint8_t compres_code) {
    if (compres_code == 1) { 
        add_asm_line(out, "    inc    rsi\n");
    } 
    else {
        char line[64] = {0};
        sprintf(line, "    add    rsi, %d\n", compres_code);
        add_asm_line(out, line);
    }
}

static inline void dec_handle(asm_out *out, uint8_t compres_code) {
    if (compres_code == 1) { 
        add_asm_line(out, "    dec    rsi\n");
    } 
    else {
        char line[124] = {0};
        sprintf(line, "    sub    rsi, %d\n", compres_code);
        add_asm_line(out, line);
    }
}

static inline void add_handle(asm_out *out, uint8_t compres_code) {
    if (compres_code == 1) {
        add_asm_line(out, "    inc    byte [rsi]\n");
    }
    else {
        char line[124] = {0};
        sprintf(line, "    add    byte [rsi], %d\n", compres_code);
        add_asm_line(out, line);
    }
}

static inline void sub_handle(asm_out *out, uint8_t compres_code) {
    if (compres_code == 1) {
        add_asm_line(out, "    dec    byte [rsi]\n");
    }
    else {
        char line[124] = {0};
        sprintf(line, "    sub    byte [rsi], %d\n", compres_code);
        add_asm_line(out, line);
    }
}

static inline void out_handle(asm_out *out, uint8_t compres_code) {

    add_asm_line(out, "    mov    rax, 1\n");
    add_asm_line(out, "    mov    rdi, 1\n");
    add_asm_line(out, "    mov    rdx, 1\n");
    add_asm_line(out, "    syscall\n");
   
}

static inline void get_handle(asm_out *out, uint8_t compres_code) {
    add_asm_line(out, "    mov    rax, 0\n");
    add_asm_line(out, "    mov    rdi, 0\n");
    add_asm_line(out, "    mov    rdx, 1\n");
    add_asm_line(out, "    syscall\n");
}

static int label_count = 0;

static inline void label_handle(bracket_stack *bs, asm_out *out) {
    char label[32] = {0}; 

    add_asm_line(out, "    cmp    byte [rsi], 0\n");
    sprintf(label    ,"    je    end_lab_%d\n", label_count);
    add_asm_line(out, label);

    sprintf(label,"lab_%d:\n", label_count);
    add_asm_line(out, label);

    stack_push(bs, label_count++);
}

static inline void jmp_handle(bracket_stack *bs, asm_out *out) {
    char label[64] = {0}; 

    add_asm_line(out, "    cmp    byte [rsi], 0\n");
    sprintf(label,    "    jne    lab_%d\n", stack_get(bs));
    add_asm_line(out, label);
    
    memset(label, 0, sizeof(label));
    sprintf(label,"end_lab_%d:\n", stack_get(bs));
    add_asm_line(out, label);

    stack_pop(bs);
}


static void interprate_lex(asm_out *out, uint8_t opcode, uint8_t compres_code, bracket_stack *bs) {

    switch (opcode) {
        case INC: {
            inc_handle(out, compres_code);
            break;
        }

        case DEC: { 
            dec_handle(out, compres_code);
            break;
        }

        case ADD: {
            add_handle(out, compres_code);
            break;
        }

        case SUB: {
            sub_handle(out, compres_code);
            break;
        }
        
        case OUT: {
            out_handle(out, compres_code);
            break;
        }

        case GET: {
            get_handle(out, compres_code);
            break;
        }

        case LABEL: {
            label_handle(bs, out);
            break;
        }

        case JMP: {
            jmp_handle(bs, out);
            break;
        }
        default: {
            // nothink
        }
    }
}

asm_out crate_asm(uint8_t *rle_bf, size_t rle_bf_len) {
    asm_out out = {0};
    bracket_stack bs = init_stack();

    creat_prolog(&out);
    int opcode_count = rle_bf_len / 2;
    for (int i = 0; i < opcode_count; i++) {
        interprate_lex(&out, rle_bf[i + 1], rle_bf[i], &bs);
    }

    creat_epilogue(&out);
    return out;
}

int save_out_asm(asm_out *out) {
    FILE *f = fopen("./a.asm", "w");
    if (!f) {
        perror("fopen");
        return -1;
    }
    fputs((char*)out->buffer, f);
    fclose(f);
    return 0;
}

int call_fasm_cpm() {
    char *arg[] = {"fasm", "./a.asm", NULL};
    execvp("fasm", arg);
    return 0;
}




