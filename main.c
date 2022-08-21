#include "board.h"
#include <stdio.h>

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

/*void printPush(Board_t b) {
    ui8 files = fileN;
    ui8 ranks = rankN;

    BoardMove_t h = {
        .from_file = files,
        .from_rank = ranks
    };

    ui8 result = 0;

    printf("**ABCDEFGH\n*\n");
    for(ui8 r = 0; r < 8; r++) {
        printf("%i ", r+1);
        for(ui8 f = 0; f < 8; f++) {
            h.to_file = f;
            h.to_rank = r;
            result = Board_legal_move(b, h);
            printf("%u", result);
        }
        printf("\n");
    }
}

void dout(Board_t b) {

    ui8 result = 0;

    printf("**ABCDEFGH\n*\n");
    for(ui8 r = 0; r < 8; r++) {
        printf("%i ", r+1);
        for(ui8 f = 0; f < 8; f++) {
            result = Board_defended_field(b, f, r);
            printf("%u", result);
        }
        printf("\n");
    }
}*/

int main(int argc, char **argv) {
    Board_t board;

    /*for(ui8 f = 0; f < 8; f++) for(ui8 r = 0; r < 8; r++) board.content[f][r] = CB_EMPTY;
    board.turn = CB_TURN_WHITE;
    board.CB_ALLOW_DEFAULT_SPM = 1;

    board.content[fileN][rankN] = CB_WHITE_MASK | CB_KING;
    board.content[fileE][rankE] = CB_BLACK_MASK | CB_QUEEN;*/

    Board_init(&board);BoardMove_t m;pout(board);

    ui8 ret;
    unsigned int ct = 1;
    ui8 vl = 1;

    #define bufmax 50
    char buf[bufmax] = {0};

    while(buf[0] != 'x' && buf[0] != 'X') {
        printf("\n> ");
        if(vl)
            printf("%u.", ct);
        scanf("%s", buf);

        if(buf[0] == 'x' || buf[0] == 'X') {
            continue;
        }

        if(buf[0] == 'p' || buf[0] == 'P') {
            pout(board);
            continue;
        }

        if(buf[0] == 'y' || buf[0] == 'Y') {
            Board_init(&board);ct=1;vl=1;pout(board);
            continue;
        }

        if(board.win == 1) {
            printf("\n ================\n=== WHITE WINS ===\n ================\n\n -> Enter Y to reset\n");
            continue;
        }
        if(board.win == 2) {
            printf("\n ================\n=== BLACK WINS ===\n ================\n\n -> Enter Y to reset\n");
            continue;
        }
        if(board.win == 3) {
            printf("\n ==========\n=== DRAW ===\n ==========\n\n -> Enter Y to reset\n");
            continue;
        }

        ret = Board_translate_expression(board, &m, buf);
        if(ret == 0 || ret == 3 || ret == 4) {
            printf("\n====================\nErr code: %u", ret);
            continue;
        }

        ret = Board_apply(&board, m);
        printf("\nBro: %i\n", ret);
        if(ret == 0 || ret == 3 || ret == 4) {
            printf("\n====================\nErr code: %u", ret);
            continue;
        }

        
        if(ret == 125) {
            printf("\n ================\n=== WHITE WINS ===\n ================\n");
        }if(ret == 126) {
            printf("\n ================\n=== BLACK WINS ===\n ================\n");
        }if(ret == 127) {
            printf("\n ==========\n=== DRAW ===\n ==========\n");
        }

        if(board.turn == CB_TURN_WHITE && !vl) ct++;
        vl = board.turn == CB_TURN_WHITE;

        pout(board);
    }

    return 1;
}