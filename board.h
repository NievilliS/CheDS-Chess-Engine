/****************************************************
 *  CHESS ENGINE by David Schoosleitner             *
 * Date: 15.08.2022                                 *
 * File: board.h                                    *
 ****************************************************
 * Responsibilities:                                *
 *  - board                                         *
 *                                                  *
 * Requires:                                        *
 *  - ...                                           *
 ****************************************************/

#ifndef _CHEDS_BOARD_H_ 
#define _CHEDS_BOARD_H_

    /***** DEFINES *****/
    #define CB_VERSION "1_0_1"

    /**** CHEDS BOARD PIECE TYPE CONSTANT ****/
    #define CB_EMPTY (0)          /* Empty specifier for board */
    #define CB_PAWN (1)           /* Pawn specifier for board */
    #define CB_ROOK (2)           /* Rook specifier for board */
    #define CB_KNIGHT (3)         /* Knight specifier for board */
    #define CB_BISHOP (4)         /* Bishop specifier for board */
    #define CB_QUEEN (5)          /* Queen specifier for board */
    #define CB_KING (6)           /* King specifier for board */

    /**** CHEDS BOARD COLOR MASKS ****/
    #define CB_WHITE_MASK (0x40u)  /* Bit mask for white pieces (bit 7) */
    #define CB_BLACK_MASK (0x20u)  /* Bit mask for black pieces (bit 6) */

    /**** FILE/RANK INDEX GETTER ****/
    #define CB_FILE_A (0)
    #define CB_FILE_B (1)
    #define CB_FILE_C (2)
    #define CB_FILE_D (3)
    #define CB_FILE_E (4)
    #define CB_FILE_F (5)
    #define CB_FILE_G (6)
    #define CB_FILE_H (7)
    #define CB_INDEX(file, rank) [(CB_FILE_ ## file)][(rank-1)]         /* File, rank to C index conversion */

    /**** TURN CONSTANTS ****/
    #define CB_TURN_FIRST (0)
    #define CB_TURN_WHITE (0)
    #define CB_TURN_BLACK (1)

    #define CB_SET_CONTENT(color, piece, file, rank) ->content CB_INDEX(file, rank) = ((CB_ ## piece) | (CB_ ## color ## _MASK))    /* e.g. CB_SET_CONTENT(WHITE, ROOK, A, 1) */

    /***** GLOBAL TYPES, VARS *****/
    typedef char ui8;                          /* uint8_t */

    typedef struct Board_t_s {
        ui8 content[8][8];          /* Raw content */
        ui8 turn;                   /* Whose turn is it */
        ui8 CB_ALLOW_DEFAULT_SPM;   /* Special move flag */
        ui8 META_CASTLE_A;          /* Enable flag for Castling Queen's side */
        ui8 win;                    /* Determines win position */
        ui8 META_CASTLE_H;          /* Enable flag for Castling King's side */
        ui8 META_PASSANT_FILE;      /* En Passant file, exact pawn is easily determined, the pawn remains on the same file and the flag applies for the person who now isn't in turn */
        __UINT16_TYPE__ turn_nr;    /* The turn number */
    } Board_t;                      /* Board struct that stores pieces as indexes */

    typedef struct BoardMove_t_s {
        ui8 from_file;
        ui8 from_rank;
        ui8 to_file;
        ui8 to_rank;
        ui8 special;                 /* 0 = none || 1 = capture minus file || 2 = capture plus file */
    } BoardMove_t;

    /***** GLOBAL METHOD DECLARATIONS *****/
    void Board_init_default(Board_t *);                                 /* Default board placement initializer */
    void Board_init(Board_t *);                                         /* This gets called in main though */
    ui8 Board_legal_move(Board_t, BoardMove_t);                         /* Checks if legal move */
    ui8 Board_color_field(Board_t, ui8, ui8);                           /* Gets a field's color */
    ui8 Board_defended_field(Board_t, ui8, ui8);                        /* Checks if a field is defended by both colors */
    ui8 Tool_Expression_Match(char *, const char *);                    /* Arithmetic chess "regex" matcher */
    ui8 Tool_Match(char *, const char *, long);                         /* String matching tool, does not use expression format */
    ui8 Board_translate_expression(Board_t, BoardMove_t *, char *);     /* Translate expression to move */
    ui8 Board_apply(Board_t *, BoardMove_t);                            /* Applies move to board */
    ui8 Board_in_check(Board_t, ui8);                                   /* Checks of the color is in check */
    void Board_clone(Board_t *, Board_t);                               /* Clones */
    ui8 Board_in_mate(Board_t, ui8);                                    /* Evaluates if the game is lost */

    /***** GLOBAL FILE HANDLE DECLARATIONS *****/
    ui8 Board_load_from_string(Board_t *, char *);                      /* Load a board from cstring */
    ui8 Board_load_from_file(Board_t *, char *);                        /* Load a board from a path */
    ui8 Board_save_to_file(Board_t, char *);                          /* Save board to a path */

#endif
