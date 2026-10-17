// yy_buffer_state.h - flex's input buffer of the script compiler.
#ifndef MBENGINE_YY_BUFFER_STATE_H
#define MBENGINE_YY_BUFFER_STATE_H

struct yy_buffer_state {
    FILE *yy_input_file;
    char *yy_ch_buf;
    char *yy_buf_pos;
    unsigned int yy_buf_size;
    int yy_n_chars;
    int yy_is_our_buffer;
    int yy_is_interactive;
    int yy_at_bol;
    int yy_fill_buffer;
    int yy_buffer_status;
};

#endif
