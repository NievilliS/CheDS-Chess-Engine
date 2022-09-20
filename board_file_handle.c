/****************************************************
 *  CHESS ENGINE by David Schoosleitner             *
 * Date: 15.08.2022                                 *
 * File: board_file_handle.h                        *
 ****************************************************
 * Responsibilities:                                *
 *  - loading board from primitive                  *
 *                                                  *
 * Requires:                                        *
 *  - ...                                           *
 ****************************************************/

/***** INCLUDES *****/
#include "board.h"
#include <math.h>
#include <stdio.h>

/***** VERSION ENCODERS DECLARATION *****/
ui8 VER1_0_1(Board_t *board, char *str);

/***** FUNCTIONALITY *****/

/** @fn ui8 Board_load_from_string(Board_t *board, char *str)
 * @brief Load a board from a correctly formatted string
 * @param board Pointer to board to be initialized
 * @param str Board-meta containing string
 * @returns is successful: 1 | else: 0
 */
ui8 Board_load_from_string(Board_t *board, char *str)
{

    //Check identifier
    if(!Tool_Match(str, "CHEDS", 0))
        return 0;

    //Check version and use compatible version
    unsigned char VLEN = str[5];

    if(VLEN == 5) {
        if(Tool_Match(str, "1.0.", 6))
            return VER1_0_1(board, str);
    }

    return 0;
}

/** @fn ui8 Board_load_from_file(Board_t *board, char *path)
 * @brief Load a board bin from path
 * @param board Pointer to board to be initialized
 * @param path Path to file
 * @returns is successful: 1 | else: 0
 */
ui8 Board_load_from_file(Board_t *board, char *path) {
    // File content into stack, processed by method above
    char buffer[350] = {0};
    FILE* file = fopen(path, "rb");
    int res;

    if(file == NULL) {
        return 0;
    }
    
    fread(buffer, sizeof(char), 350, file);

    if(fclose(file)) {
        return 0;
    }

    // Call method above
    return Board_load_from_string(board, buffer);
}

/** @fn ui8 Board_save_to_file(Board_t board, char *path)
 * @brief Save board content into path
 * @param board Board to be saved
 * @param path Path to file
 * @returns is successful: 1 | else: 0
 */
ui8 Board_save_to_file(Board_t board, char *path)
{
    FILE* file = fopen(path, "wb");
    int res;

    // Saving as 1.0.1 file

    /*
     * ATTRIBUTE FORMATING FOR 1.0.1:
     * ABBC CDDD
     * DEFF 0000
     * TTTT TTTT
     * TTTT TTTT
     * 
     * A - Allow default special move flag
     * B - Queen's side castling info
     * C - King's side castling info
     * D - Peasant File info
     * E - Turn flag, 0 = white, 1 = black
     * F - Win, 0 = ongoing game, 1 = white, 2 = black, 3 = draw
     * T - Turn count number
     */

    /*
     * CONTENT FORMATTING FOR 1.0.1:
     * CCFF FRRRR
     * 
     * C - Command
     * F - File
     * R - Rank
     * 
     * Typically:
     * 
     * 01FF FRRR  ->  Set white piece
     * 10FF FRRR  ->  Set black piece
     * 11PP PPPP  ->  Set piece ID
     * 00-- ----  ->  Exit
     */

    // Success at non-negative value
    #define savecheck   \
    if(res < 0)         \
        return 0

    // Values
    const char *begin = CB_CURRENT_FILE_HEADER;

    const unsigned char attr[4] = {
        ((board.CB_ALLOW_DEFAULT_SPM & 0x01u) << 7) |
        ((board.META_CASTLE_A & 0x03u) << 5)        |
        ((board.META_CASTLE_H & 0x03u) << 3)        |
        ((board.META_PASSANT_FILE & 0x0Eu) >> 1)    ,

        ((board.META_PASSANT_FILE & 0x01u) << 7)    |
        ((board.turn & 0x01u) << 6)                 |
        ((board.win & 0x03u) << 4)                  ,

        (ui8) (board.turn_nr >> 8)                  ,

        (ui8) (board.turn_nr)                       ,
    };

    const unsigned char white_cmd = 0x40;
    const unsigned char black_cmd = 0x80;

    // Body data
    ui8 f, r;
    ui8 piece;
    ui8 color;

    // String buffers for all pieces
    char str_pawns[66] =    {(0xC0u | (CB_PAWN & 0x3Fu))};
    char str_rooks[66] =    {(0xC0u | (CB_ROOK & 0x3Fu))};
    char str_knights[66] =  {(0xC0u | (CB_KNIGHT & 0x3Fu))};
    char str_bishops[66] =  {(0xC0u | (CB_BISHOP & 0x3Fu))};
    char str_queens[66] =   {(0xC0u | (CB_QUEEN & 0x3Fu))};
    char str_kings[66] =    {(0xC0u | (CB_KING & 0x3Fu))};

    int ind_pawns =     1;
    int ind_rooks =     1;
    int ind_knights =   1;
    int ind_bishops =   1;
    int ind_queens =    1;
    int ind_kings =     1;

    unsigned char out = 0x00u;

    // Save all pieces into the string buffers
    for(f = 0; f < 8; f++) for(r = 0; r < 8; r++) {
        piece = (board.content[f][r]) & ~(CB_WHITE_MASK | CB_BLACK_MASK);
        color = Board_color_field(board, f, r);

        out =
            ((color == 1) ? (white_cmd) : (black_cmd))  |
            ((f & 0x07u) << 3)                          |
            (r & 0x07u)                                 ;

        switch(piece) {
            case CB_PAWN:
                str_pawns[ind_pawns++] = out;
            break;
            case CB_ROOK:
                str_rooks[ind_rooks++] = out;
            break;
            case CB_KNIGHT:
                str_knights[ind_knights++] = out;
            break;
            case CB_BISHOP:
                str_bishops[ind_bishops++] = out;
            break;
            case CB_QUEEN:
                str_queens[ind_queens++] = out;
            break;
            case CB_KING:
                str_kings[ind_kings++] = out;
            break;
            default:
                continue;
        }
    }

    // Header file writing
    res = fputs(begin, file);
    savecheck;
    
    for(ui8 I = 0; I < 4; I++) {
        res = fputc(attr[I], file);
        savecheck;
    }

    // Body file writing
    if(ind_pawns > 1) {
        res = fputs(str_pawns, file);
        savecheck;
    }
    
    if(ind_rooks > 1) {
        res = fputs(str_rooks, file);
        savecheck;
    }
    
    if(ind_knights > 1) {
        res = fputs(str_knights, file);
        savecheck;
    }
    
    if(ind_bishops > 1) {
        res = fputs(str_bishops, file);
        savecheck;
    }
    
    if(ind_queens > 1) {
        res = fputs(str_queens, file);
        savecheck;
    }
    
    if(ind_kings > 1) {
        res = fputs(str_kings, file);
        savecheck;
    }

    // Exit code '\1'
    fputc(1, file);
    savecheck;

    return (fclose(file) == 0);
}

/** @fn ui8 Tool_Match(char *literal, const char *match, long at)
 * @brief Matches a literal with another literal. An offset is available
 * @param literal String to be checked
 * @param match The string that is used to match with
 * @param at Check at this string index
 * @returns if it matches or not 1 | 0
 */
ui8 Tool_Match(char *literal, const char *match, long at)
{
    long index_literal = at;
    long index_match = 0;

    //simply iterate till match[i] == 0
    while(match[index_match])
    {
        //string length overshot
        if(!literal[index_literal])
            return 0;
        //mismatch
        if(literal[index_literal] != match[index_match])
            return 0;
        //iterate
        index_literal++;
        index_match++;
    }
    return 1;
}

/** @fn ui8 VER1_0_1(Board_t *board, char *str)
 * @brief CHEDS Version 1.0.1 string reader
 * @param board Pointer to board to be initialized
 * @returns is successful: 1 | else: 0
 */
ui8 VER1_0_1(Board_t *orig_brd, char *str)
{
    //Intermediate storage in case of mishaps
    Board_t board;

    //Set board content to empty
    for(ui8 f = 0; f < 8; f++) for(ui8 r = 0; r < 8; r++) {
        board.content[f][r] = CB_EMPTY;
    }

    /*
     * ATTRIBUTE FORMATING FOR 1.0.1:
     * ABBC CDDD
     * DEFF 0000
     * TTTT TTTT
     * TTTT TTTT
     * 
     * A - Allow default special move flag
     * B - Queen's side castling info
     * C - King's side castling info
     * D - Peasant File info
     * E - Turn flag, 0 = white, 1 = black
     * F - Win, 0 = ongoing game, 1 = white, 2 = black, 3 = draw
     * T - Turn counter number
     */
    long attributes_index = 11;

    board.CB_ALLOW_DEFAULT_SPM = (str[attributes_index] >> 7) & 0x01u;
    board.META_CASTLE_A = (str[attributes_index] >> 5) & 0x03u;
    board.META_CASTLE_H = (str[attributes_index] >> 3) & 0x03u;
    board.META_PASSANT_FILE = ((str[attributes_index] << 1) & 0x0Eu) | ((str[attributes_index + 1] >> 7) & 0x01u);
    board.turn = (str[attributes_index + 1] >> 6) & 0x01u;
    board.win = (str[attributes_index + 1] >> 4) & 0x03u;
    board.turn_nr = (((__UINT16_TYPE__) str[attributes_index + 2]) << 8) | ((__UINT16_TYPE__) str[attributes_index + 3]);

    /*
     * CONTENT FORMATTING FOR 1.0.1:
     * CCFF FRRRR
     * 
     * C - Command
     * F - File
     * R - Rank
     * 
     * Typically:
     * 
     * 01FF FRRR  ->  Set white piece
     * 10FF FRRR  ->  Set black piece
     * 11PP PPPP  ->  Set piece ID
     * 00-- ----  ->  Exit
     */
    long content_index = 15;
    unsigned char current;
    unsigned char valid_load = 0;
    unsigned char sel_piece = CB_KING;

    //Iterate till string '\0'
    while(current = str[content_index++])
    {
        switch((current >> 6) & (0x03u)) {
            // Exit code
            case 0u:
            {
                // This here must not be '\0', otherwise next conditional accesses unpermitted memory!
                if(current == 0)
                    return 0;
                // Must be at end before the '\0'
                if(str[content_index])
                    return 0;
                else
                    valid_load = 1u;
            }
            break;

            // Set white piece
            case 1u:
            {
                // Get coords and check if field is empty, else error
                ui8 sel_file = (current >> 3) & 0x07u;
                ui8 sel_rank = current & 0x07u;

                if(board.content[sel_file][sel_rank] != CB_EMPTY)
                    return 0;
                
                // Put into field
                board.content[sel_file][sel_rank] = CB_WHITE_MASK | sel_piece;
            }
            break;

            // Set white piece
            case 2u:
            {
                // Get coords and check if field is empty, else error
                ui8 sel_file = (current >> 3) & 0x07u;
                ui8 sel_rank = current & 0x07u;

                if(board.content[sel_file][sel_rank] != CB_EMPTY)
                    return 0;
                
                // Put into field
                board.content[sel_file][sel_rank] = CB_BLACK_MASK | sel_piece;
            }
            break;

            // Select a piece
            case 3u:
            {
                ui8 piece_id = current & 0x3Fu;

                // 1.0.1 Piece to current conversion
                // Not required as of now
                sel_piece = piece_id;
            }
            break;

            // I don't think this is possible because of the "& 3", but safety
            default:
                return 0;
        }
    }

    // Validity check
    if(!valid_load)
        return 0;
    
    // Successful load and clone
    Board_clone(orig_brd, board);
    return 1;
}
