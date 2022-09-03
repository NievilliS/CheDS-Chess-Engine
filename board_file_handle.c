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
        if(Tool_Match(str, "1.0.1", 6))
            return VER1_0_1(board, str);
    }

    return 0;
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
    Board_clone(&board, *orig_brd);

    /*
     * ATTRIBUTE FORMATING FOR 1.0.1:
     * ABBC CDDD
     * DEFF 0000
     * 
     * A - Allow default special move flag
     * B - Queen's side castling info
     * C - King's side castling info
     * D - Peasant File info
     * E - Turn flag, 0 = white, 1 = black
     * F - Win, 0 = ongoing game, 1 = white, 2 = black, 3 = draw
     */
    long attributes_index = 11;

    board.CB_ALLOW_DEFAULT_SPM = (str[attributes_index] >> 7) & 0x01u;
    board.META_CASTLE_A = (str[attributes_index] >> 5) & 0x03u;
    board.META_CASTLE_H = (str[attributes_index] >> 3) & 0x03u;
    board.META_PASSANT_FILE = ((str[attributes_index] << 1) & 0x0Eu) | ((str[attributes_index + 1] >> 7) & 0x01u);
    board.turn = (str[attributes_index + 1] >> 6) & 0x01u;
    board.win = (str[attributes_index + 1] >> 4) & 0x03u;

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
    long content_index = 13;
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
