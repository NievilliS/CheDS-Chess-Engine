#include "board.h"
#include <stdio.h>
#include <stdlib.h>

void pout(Board_t b) {
    printf("\n**ABCDEFGH\n*\n");
    for(ui8 r = 7; r >= 0; r--) {
        printf("%i ", r+1);
        for(ui8 f = 0; f < 8; f++) {
            char oset = 0;
            char out;
            if(Board_color_field(b, f, r) == 2)
                oset = 'a'-'A';
            switch(b.content[f][r] & ~(CB_BLACK_MASK | CB_WHITE_MASK)) {
                case CB_EMPTY: out = '.'; break;
                case CB_ROOK: out = 'R'+oset; break;
                case CB_KNIGHT: out = 'N'+oset; break;
                case CB_BISHOP: out = 'B'+oset; break;
                case CB_QUEEN: out = 'Q'+oset; break;
                case CB_KING: out = 'K'+oset; break;
                case CB_PAWN: out = 'P'+oset; break;
            }
            printf("%c", out);
        }
        printf(" %i\n", r+1);
    }
    printf("*\n**ABCDEFGH\n");
}

void pushlog(dynl_t *log, Board_t board) {
    Board_t *nb = (Board_t*) malloc(sizeof(Board_t));
    Board_clone(nb, board);

    //iterator
    dynl_t *ilog = log;
    //log1 voided
    dynl_t log1 = {
        .n = log,
        .b = nb
    };
    //log2 appended
    dynl_t *log2 = (dynl_t*) malloc(sizeof(dynl_t));

    while(ilog->b != NULL) {
        //Step 1: Save
        log2->b = ilog->b;
        //Step 2: Load
        ilog->b = log1.b;
        //Step 3: Switch
        log1.b = log2->b;
        //Iterate
        ilog = ilog->n;
    }
    //Load
    ilog->b = log1.b;
    //Nullify
    log2->b = NULL;
    log2->n = NULL;
    //Append
    ilog->n = log2;
}

void cdlr(dynl_t *log) {
    if(log == NULL || log->b == NULL)
        return;
    cdlr(log->n);
    free(log->b);
    free(log->n);
}

void collapselog(dynl_t *log) {
    cdlr(log->n);
    free(log->n);
    free(log->b);
    log->n = NULL;
    log->b = NULL;
}

void popfront(dynl_t *log, Board_t *board) {
    Board_clone(board, *(log->b));
    free(log->b);
    
    dynl_t *ilog = log;
    while(ilog->n->b != NULL) {
        ilog->b = ilog->n->b;
        ilog = ilog->n;
    }
    ilog->b = NULL;
    free(ilog->n);
    ilog->n = NULL;
}

int main() {
    Board_t board;
    dynl_t *log;
    log = (dynl_t*) malloc(sizeof(dynl_t));
    log->n = NULL;
    log->b = NULL;

    Board_init(&board);BoardMove_t m;pout(board);

    ui8 ret;

    #define bufmax 200
    char buf[bufmax] = {0};

    while(buf[0] != 'x' && buf[0] != 'X') {
        printf("\n> ");
        if((board.turn_nr % 2) == 0)
            printf("%u.", 1 + (board.turn_nr / 2));
        scanf("%s", buf);

        // Exit
        if(buf[0] == 'x' || buf[0] == 'X') {
            continue;
        }

        //Help
        if(buf[0] == '?' || buf[0] == 'H' || buf[0] == 'h') {
            printf("Help:\n\
    - ? - h - print help\n\
    - x - exit application\n\
    - p - printout board\n\
    - res - reset board\n\
    - clr - clear board to empty\n\
    - sa path - save board to path\n\
    - lo path - load board from path\n\
    - un - undo latest moves\n\
    - edit (f)(r)(c)(p) - edit board customly (example: edit f5wp -> sets f5 to white queen, edit h1 -> sets h1 to empty)\n");
            continue;
        }

        // Printout
        if(buf[0] == 'p' || buf[0] == 'P') {
            pout(board);
            continue;
        }

        // Reset
        if((buf[0] == 'r' || buf[0] == 'R') &&
           (buf[1] == 'e' || buf[1] == 'E') &&
           (buf[2] == 's' || buf[2] == 'S')
        ) {
            Board_init(&board);pout(board);
            collapselog(log);
            continue;
        }

        // Clear board empty
        if((buf[0] == 'c' || buf[0] == 'C') &&
           (buf[1] == 'l' || buf[1] == 'L') &&
           (buf[2] == 'r' || buf[2] == 'R')
        ) {
            //laziness
            Board_load_from_string(&board,
                "CHEDS" "\5" "1.0.2" "\0\0\0\0\1"
            );
            pout(board);
            collapselog(log);
            continue;
        }

        // Save
        if((buf[0] == 's' || buf[0] == 'S') &&
           (buf[1] == 'a' || buf[1] == 'A')
        ) {
            printf("Save Path: ");
            char path[260] = {0};
            fscanf(stdin, "%s", path);

            if(Board_save_to_file(board, path)) {
                printf("\n -> Saved to %s! \n\n", path);
                continue;
            }
            printf("\n -> Error on saving file! \n\n");
            continue;
        }

        // Load
        if((buf[0] == 'l' || buf[0] == 'L') &&
           (buf[1] == 'o' || buf[1] == 'O')
        ) {
            printf("Load Path: ");
            char path[260] = {0};
            fscanf(stdin, "%s", path);
            
            if(Board_load_from_file(&board, path)) {
                printf("\n -> Loaded from %s! \n\n", path);
                pout(board);
                collapselog(log);
                continue;
            }
            printf("\n -> Error on loading file! \n\n");
            continue;
        }

        // Edit
        if((buf[0] == 'e' || buf[0] == 'E') &&
           (buf[1] == 'd' || buf[1] == 'D') &&
           (buf[2] == 'i' || buf[2] == 'I') &&
           (buf[3] == 't' || buf[3] == 'T')
        ) {
            printf("edit> ");
            char edit[5] = {0};
            scanf("%s", edit);

            if(edit[0] > 'H') edit[0] -= 'a'-'A';
            if(edit[2] > 'Z') edit[2] -= 'a'-'A';
            if(edit[3] > 'Z') edit[3] -= 'a'-'A';
            if(
               (edit[0] < 'A' || edit[0] > 'H') ||
               (edit[1] < '1' || edit[1] > '8')
            ) {
                printf("Usage:\n- > edit f5wp (sets f5 to white queen)\n- > edit h1 (sets h1 to empty)");
                continue;
            }

            ui8 f = edit[0]-'A';
            ui8 r = edit[1]-'1';

            //clear:
            if(edit[2] == 0 && edit[3] == 0) {
                pushlog(log, board);
                board.content[f][r] = CB_EMPTY;
                pout(board);
                continue;
            }

            //other:
            if(edit[2] != 'W' && edit[2] != 'B') {
                printf("Usage:\n- > edit f5wp (sets f5 to white queen)\n- > edit h1 (sets h1 to empty)");
                continue;
            }

            ui8 p = (edit[2] == 'W') ? CB_WHITE_MASK : CB_BLACK_MASK;
            switch(edit[3]) {
                case 0: case 'P': p |= CB_PAWN; break;
                case 'R': p |= CB_ROOK; break;
                case 'N': p |= CB_KNIGHT; break;
                case 'B': p |= CB_BISHOP; break;
                case 'Q': p |= CB_QUEEN; break;
                case 'K': p |= CB_KING; break;
                default: p = 0; printf("Usage:\n- > edit f5wp (sets f5 to white queen)\n- > edit h1 (sets h1 to empty)");
            }
            if(!p) continue;

            pushlog(log, board);
            board.content[f][r] = p;
            pout(board);
            continue;
        }

        //Undo
        if((buf[0] == 'u' || buf[0] == 'U') &&
           (buf[1] == 'n' || buf[1] == 'N')
        ) {
            if(log->n == NULL) {
                printf("\nNothing to be undone!\n");
                continue;
            }

            printf("\nUndoing last step!\n");
            popfront(log, &board);
            pout(board);
            continue;
        }

        if(board.win == 1) {
            printf("\n ================\n=== WHITE WINS ===\n ================\n\n -> Enter 'res' to reset\n");
            continue;
        }
        if(board.win == 2) {
            printf("\n ================\n=== BLACK WINS ===\n ================\n\n -> Enter 'res' to reset\n");
            continue;
        }
        if(board.win == 3) {
            printf("\n ==========\n=== DRAW ===\n ==========\n\n -> Enter 'res' to reset\n");
            continue;
        }

        ret = Board_translate_expression(board, &m, buf);
        if(ret == 0 || ret == 3 || ret == 4) {
            printf("\n====================\nTranslation error: %u", ret);
            continue;
        }

        Board_t blg;
        Board_clone(&blg, board);
        ret = Board_apply(&board, m);
        if(ret == 0 || ret == 3 || ret == 4) {
            printf("\n====================\nErr code: %u", ret);
            continue;
        }
        pushlog(log, blg);

        
        if(ret == 125) {
            printf("\n ================\n=== WHITE WINS ===\n ================\n");
        }if(ret == 126) {
            printf("\n ================\n=== BLACK WINS ===\n ================\n");
        }if(ret == 127) {
            printf("\n ==========\n=== DRAW ===\n ==========\n");
        }

        pout(board);
    }

    collapselog(log);
    free(log);

    return 1;
}