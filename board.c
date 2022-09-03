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

/***** INCLUDES *****/
#include "board.h"
#include <math.h>

/***** FUNCTIONALITY *****/

/** @fn void Board_init(Board_t *board)
 * @brief This gets called to initialize the board
 * @brief and can be customized (take reference from Board_init_default)
 * @param board Pointer to board to be initialized
 * @returns none
 */
void Board_init(Board_t *board) {
    board->CB_ALLOW_DEFAULT_SPM = 0;

    Board_init_default(board);
}

/** @fn void Board_init_default(Board_t *board)
 * @brief Standard game of chess setup
 * @param board Pointer to board to be initialized
 * @returns none
 */
void Board_init_default(Board_t *board) {
    //White pieces
    board CB_SET_CONTENT(WHITE, ROOK, A, 1);
    board CB_SET_CONTENT(WHITE, ROOK, H, 1);
    board CB_SET_CONTENT(WHITE, KNIGHT, B, 1);
    board CB_SET_CONTENT(WHITE, KNIGHT, G, 1);
    board CB_SET_CONTENT(WHITE, BISHOP, C, 1);
    board CB_SET_CONTENT(WHITE, BISHOP, F, 1);
    board CB_SET_CONTENT(WHITE, QUEEN, D, 1);
    board CB_SET_CONTENT(WHITE, KING, E, 1);

    //White pawns
    ui8 i = 0;
    for(; i < 8; i++)
        board->content[i][1] = (CB_PAWN | CB_WHITE_MASK);

    //Black pieces
    board CB_SET_CONTENT(BLACK, ROOK, A, 8);
    board CB_SET_CONTENT(BLACK, ROOK, H, 8);
    board CB_SET_CONTENT(BLACK, KNIGHT, B, 8);
    board CB_SET_CONTENT(BLACK, KNIGHT, G, 8);
    board CB_SET_CONTENT(BLACK, BISHOP, C, 8);
    board CB_SET_CONTENT(BLACK, BISHOP, F, 8);
    board CB_SET_CONTENT(BLACK, QUEEN, D, 8);
    board CB_SET_CONTENT(BLACK, KING, E, 8);

    //Black pawns
    for(; i > 0; i--)
        board->content[i-1][6] = (CB_PAWN | CB_BLACK_MASK);

    //Clean inner
    for(ui8 f = 0; f < 8; f++) for(ui8 r = 2; r < 6; r++) board->content[f][r] = CB_EMPTY;

    //Configure turn
    board->turn = CB_TURN_FIRST;

    //Set special move flags (en passant, castle)
    board->CB_ALLOW_DEFAULT_SPM = 1;
    board->META_PASSANT_FILE = 8;
    board->META_CASTLE_A = 3;
    board->META_CASTLE_H = 3;
    board->win = 0;
}

/** @fn ui8 Board_legal_move(Board_t, BoardMove_t)
 * @brief Checks if said movement is actually legal
 * @param board Board to be checked
 * @param move Move to be checked
 * @returns 1 - is legal move || 0 - is not || 2 - capture || 5 - en passant capture || 6 - castle queen's || 7 - castle king's || 8 - promotion rook || 9 - promotion knight || 10 - promotion bishop || 11 - promotion queen
 */
ui8 Board_legal_move(Board_t board, BoardMove_t move) {
    //Moveset is defined static
    
    //Bound check
    if(move.to_file < 0 || move.to_file > 7 || move.to_rank < 0 || move.to_rank > 7)
        return 0;

    //Check pieces
    switch(board.content[move.from_file][move.from_rank] & ~(CB_WHITE_MASK | CB_BLACK_MASK)) {

        //Case for pawn, depends on direction, check board.turn
        case CB_PAWN: {
            signed short direction = ((Board_color_field(board, move.from_file, move.from_rank) == 1) ? 1 : -1);

            //Promotion generic
            if(((move.from_rank + direction == 7) && (move.to_rank == 0)) || ((move.from_rank + direction == 0) && (move.to_rank == 7))) {
                switch(move.to_file) {
                    case 0: return 8;
                    case 1: return 9;
                    case 2: return 10;
                    case 3: return 11;
                }
            }

            else if(move.to_rank == 0 || move.to_rank == 7) {
                return 0;
            }

            //Move forward by one
            else if((move.from_rank + direction == move.to_rank) && (abs(move.to_file - move.from_file) < 2)) {
                //Directly ahead
                if(move.from_file == move.to_file)
                    return !Board_color_field(board, move.to_file, move.to_rank);

                //Sidewards (capture)
                if((direction == 1) && (Board_color_field(board, move.to_file, move.to_rank) == 2))
                    return 2;
                if((direction == -1) && (Board_color_field(board, move.to_file, move.to_rank) == 1))
                    return 2;

                //En Passant capture
                if(board.CB_ALLOW_DEFAULT_SPM) {
                    if(move.to_file == board.META_PASSANT_FILE) {
                        if(Board_color_field(board, move.from_file, move.from_rank) != Board_color_field(board, move.to_file, move.from_rank)) {
                            return 5;
                        }
                    }
                }

                //Else no valid collision
                return 0;
            }

            //Move forward by two
            else if((move.from_rank + direction * 2 == move.to_rank) && (move.from_file == move.to_file) && ((move.from_rank == 1) || (move.from_rank == 6))) {
                //Collision check for both fields
                if((!Board_color_field(board, move.to_file, move.to_rank)) && (!Board_color_field(board, move.to_file, move.from_rank + direction)))
                    return 1;
                return 0;
            }

            //Invalid move
            return 0;
        }
        break;

        //Case for rook, any linear direction
        case CB_QUEEN:
        case CB_ROOK:
            //Direction check
            if((move.from_rank == move.to_rank) ^ (move.from_file == move.to_file)) {
                //Collission
                ui8 i;

                //File moves
                if(move.from_rank == move.to_rank) {
                    //Positive move
                    if(move.from_file < move.to_file)
                        //Iterate until last field
                        for(i = move.from_file + 1; i < move.to_file; i++) {
                            if(Board_color_field(board, i, move.from_rank))
                                return 0;
                        }

                    //Negative move
                    else
                        //Iterate until last field
                        for(i = move.from_file - 1; i > move.to_file; i--) {
                            if(Board_color_field(board, i, move.from_rank))
                                return 0;
                        }
                }
                
                //Rank moves
                else {
                    //Positive move
                    if(move.from_rank < move.to_rank)
                        //Iterate until last field
                        for(i = move.from_rank + 1; i < move.to_rank; i++) {
                            if(Board_color_field(board, move.from_file, i))
                                return 0;
                        }

                    //Negative move
                    else
                        //Iterate until last field
                        for(i = move.from_rank - 1; i > move.to_rank; i--) {
                            if(Board_color_field(board, move.from_file, i))
                                return 0;
                        }
                }

                //Collision with own pieces
                if(((Board_color_field(board, move.from_file, move.from_rank) == 1) && (Board_color_field(board, move.to_file, move.to_rank) == 1)) ||
                    ((Board_color_field(board, move.from_file, move.from_rank) == 2) && (Board_color_field(board, move.to_file, move.to_rank) == 2)))
                    return 0;
                //Capture
                if(((Board_color_field(board, move.from_file, move.from_rank) == 1) && (Board_color_field(board, move.to_file, move.to_rank) == 2)) ||
                    ((Board_color_field(board, move.from_file, move.from_rank) == 2) && (Board_color_field(board, move.to_file, move.to_rank) == 1)))
                    return 2;
                return 1;
            }

            //QUEEN STUFF
            else {
                if((board.content[move.from_file][move.from_rank] & ~(CB_WHITE_MASK | CB_BLACK_MASK)) != CB_QUEEN)
                    return 0;
            }

        case CB_BISHOP:
            //Same field check
            if(move.to_rank == move.from_rank) return 0;

            //Direction check
            if( (move.to_file - move.from_file == move.to_rank - move.from_rank) || (move.to_file - move.from_file == move.from_rank - move.to_rank) ) {

                ui8 i = 1;

                //Positive file
                if(move.from_file < move.to_file) {

                    //Positive rank
                    if(move.from_rank < move.to_rank) {
                        
                        //Iterate for collision
                        for(i = 1; i + move.from_file < move.to_file; i++)
                            if(Board_color_field(board, move.from_file + i, move.from_rank + i))
                                return 0;
                    }

                    //Negative rank
                    else {
                        
                        //Iterate for collision
                        for(i = 1; i + move.from_file < move.to_file; i++)
                            if(Board_color_field(board, move.from_file + i, move.from_rank - i))
                                return 0;
                    }
                }

                //Negative file
                else {

                    //Positive rank
                    if(move.from_rank < move.to_rank) {
                        
                        //Iterate for collision
                        for(i = 1; move.from_file - i > move.to_file; i++)
                            if(Board_color_field(board, move.from_file - i, move.from_rank + i))
                                return 0;
                    }

                    //Negative rank
                    else {
                        
                        //Iterate for collision
                        for(i = 1; move.from_file - i > move.to_file; i++) {
                            if(Board_color_field(board, move.from_file - i, move.from_rank - i))
                                return 0;
                        }
                    }
                }

                //Collision with own pieces
                if(((Board_color_field(board, move.from_file, move.from_rank) == 1) && (Board_color_field(board, move.to_file, move.to_rank) == 1)) ||
                    ((Board_color_field(board, move.from_file, move.from_rank) == 2) && (Board_color_field(board, move.to_file, move.to_rank) == 2)))
                    return 0;
                //Capture
                if(((Board_color_field(board, move.from_file, move.from_rank) == 1) && (Board_color_field(board, move.to_file, move.to_rank) == 2)) ||
                    ((Board_color_field(board, move.from_file, move.from_rank) == 2) && (Board_color_field(board, move.to_file, move.to_rank) == 1)))
                    return 2;
                return 1;
            }
            
            return 0;

        case CB_KNIGHT: {

            //Movement
            signed int movements[8][2] = {
                {1,2},
                {1,-2},
                {-1,2},
                {-1,-2},
                {2,1},
                {2,-1},
                {-2,1},
                {-2,-1},
            };

            //Check all, valid on one
            ui8 invalid = 1;
            for(ui8 i = 0; i < 8 && invalid; i++)
                if((((signed) move.from_file - (signed) move.to_file) == movements[i][0]) && (((signed) move.from_rank - (signed) move.to_rank) == movements[i][1]))
                    invalid = 0;

            if(invalid)
                return 0;

            //Capture or not
            if(!Board_color_field(board, move.to_file, move.to_rank))
                return 1;
            else if(((Board_color_field(board, move.from_file, move.from_rank) == 1) && (Board_color_field(board, move.to_file, move.to_rank) == 2)) ||
                ((Board_color_field(board, move.from_file, move.from_rank) == 2) && (Board_color_field(board, move.to_file, move.to_rank) == 1)))
                return 2;
            return 0;
        }
        case CB_KING:

            //Castling check
            if(board.CB_ALLOW_DEFAULT_SPM && move.from_file == CB_FILE_E && (move.from_rank == 0 || move.from_rank == 7)) {
                //White
                if(Board_color_field(board, move.from_file, move.from_rank) == 1 && Board_color_field(board, move.to_file, move.to_rank) == 1) {
                    //Queen's side
                    if((board.META_CASTLE_A & 2) && move.to_file == CB_FILE_A && move.to_rank == 0) {
                        //File B to D must be empty
                        for(ui8 i = 1; i < 4; i++)
                            if(board.content[i][0] != CB_EMPTY) return 0;

                        //C1 mustn't be defended
                        if(Board_defended_field(board, 2, 0) & 1) return 0;
                        return 6;
                    }

                    //King's side
                    if((board.META_CASTLE_H & 2) && move.to_file == CB_FILE_H && move.to_rank == 0) {
                        //File F and G must be empty
                        if(board.content[5][0] != CB_EMPTY) return 0;
                        if(board.content[6][0] != CB_EMPTY) return 0;

                        //G1 mustn't be defended
                        if(Board_defended_field(board, 6, 0) & 1) return 0;
                        return 7;
                    }
                }

                //Black
                else if(Board_color_field(board, move.from_file, move.from_rank) == 2 && Board_color_field(board, move.to_file, move.to_rank) == 2) {
                    //Queen's side
                    if((board.META_CASTLE_A & 1) && move.to_file == CB_FILE_A && move.to_rank == 7) {
                        //File B to D must be empty
                        for(ui8 i = 1; i < 4; i++)
                            if(board.content[i][7] != CB_EMPTY) return 0;

                        //C8 mustn't be defended
                        if(Board_defended_field(board, 2, 7) & 2) return 0;
                        return 6;
                    }

                    //King's side
                    if((board.META_CASTLE_H & 1) && move.to_file == CB_FILE_H && move.to_rank == 7) {
                        //File F and G must be empty
                        if(board.content[5][7] != CB_EMPTY) return 0;
                        if(board.content[6][7] != CB_EMPTY) return 0;

                        //G8 mustn't be defended
                        if(Board_defended_field(board, 6, 7) & 2) return 0;
                        return 7;
                    }
                }
            }

            //Movement
            if(!((abs((signed) move.from_file - (signed) move.to_file) < 2) && (abs((signed) move.from_rank - (signed) move.to_rank) < 2)))
                return 0;

            //Cant not move
            if(move.to_file == move.from_file && move.to_rank == move.from_rank) return 0;

            //Check if field is in check
            if(Board_defended_field(board, move.to_file, move.to_rank) & Board_color_field(board, move.from_file, move.from_rank))
                return 0;

            //Capture
            if(!Board_color_field(board, move.to_file, move.to_rank))
                return 1;
            else if(((Board_color_field(board, move.from_file, move.from_rank) == 1) && (Board_color_field(board, move.to_file, move.to_rank) == 2)) ||
                ((Board_color_field(board, move.from_file, move.from_rank) == 2) && (Board_color_field(board, move.to_file, move.to_rank) == 1)))
                return 2;
            return 0;
        default: return 0;
    }
}

/** @fn ui8 Board_defended_field(Board_t board, ui8 file, ui8 rank)
 * @brief Checks if a field is defended by any or both colors
 * @param board Board to be checked
 * @param file File A-G (0-7 in index 1)
 * @param rank Rank 1-8 (0-7 in index 2)
 * @returns 0 - undefended || 1 - defended by black || 2 - defended by white || 3 - defended by both
 */
ui8 Board_defended_field(Board_t board, ui8 file, ui8 rank) {
	BoardMove_t move = {.to_file = file, .to_rank = rank};

	ui8 ret = 0;

	for(ui8 f = 0; f < 8; f++) for(ui8 r = 0; r < 8; r++) {
		
		//Disallow king recursion
		if((board.content[f][r] & ~(CB_WHITE_MASK | CB_BLACK_MASK)) == CB_KING) {
			if((abs((signed) f - (signed) file) < 2) && (abs((signed) r - (signed) rank) < 2) && !(file == f && rank == r))
				ret |= (3 - Board_color_field(board, f, r));
		}

		//Get if piece
		else if(board.content[f][r] != CB_EMPTY) {
			move.from_file = f;
			move.from_rank = r;
			if(Board_legal_move(board, move)) {
				if(Board_color_field(board, f, r) == 1)
					ret |= 2;
				else
					ret |= 1;
			}
		}
	}

	return ret;
}

/** @fn ui8 Board_color_field(Board_t board, ui8 file, ui8 rank)
 * @brief Gets a field's color, or 0 if empty
 * @param board Board to be checked
 * @param file File A-G (0-7 in index 1)
 * @param rank Rank 1-8 (0-7 in index 2)
 * @returns 0 - empty || 1 - white || 2 - black
 */
ui8 Board_color_field(Board_t board, ui8 file, ui8 rank) {
    if(board.content[ file ][ rank ] & CB_WHITE_MASK)
        return 1;
    else if(board.content[ file ][ rank ] & CB_BLACK_MASK)
        return 2;
    return 0;
}

/** @fn ui8 Board_apply(Board_t *board, BoardMove_t move)
 * @brief Apply a move to the board and change its content, check and switch turn
 * @param board Pointer to board to be played on
 * @param move Move to apply
 * @returns 0 - unsuccessful || >0 - successful with flags || 125 - white wins || 126 - black wins || 127 - draw or stalemate
 */
ui8 Board_apply(Board_t *board, BoardMove_t move) {
    //Swap tool
    #define swret                                                                                       \
    /* If the applied move doesn't resolve or causes a check, the move can't be done */                 \
    if(Board_in_check(*board, (board->turn == CB_TURN_WHITE) ? 1 : 2)) {                                \
        Board_clone(board, backup);                                                                     \
        return 0;                                                                                       \
    }                                                                                                   \
    /* Swap turn */                                                                                     \
    board->turn = (board->turn == CB_TURN_WHITE) ? CB_TURN_BLACK : CB_TURN_WHITE;                       \
    /* Check for checkmate */                                                                           \
    if(Board_in_mate(*board, (board->turn == CB_TURN_WHITE) ? 1 : 2)) {                                 \
        board->win = ((board->turn == CB_TURN_WHITE) ? 2 : 1);                                          \
        return ((board->turn == CB_TURN_WHITE) ? 126u : 125u);                                          \
    }                                                                                                   \
    /* TODO: To check for stalemate                                                                     \
    if(Board_in_stale(*board, (board->turn == CB_TURN_WHITE) ? 1 : 2)) {}                               \
    */                                                                                                  \
    return

    Board_t backup;
    Board_clone(&backup, *board);

    ui8 curcol = Board_color_field(*board, move.from_file, move.from_rank);

    //Step 0: Turn check
    if(curcol == 1 && board->turn != CB_TURN_WHITE)
        return 0;
    if(curcol == 2 && board->turn != CB_TURN_BLACK)
        return 0;

    //Step 1: Validity check
    ui8 legal = Board_legal_move(*board, move);
    if(!legal)
        return 0;

    //Get this piece and clear it from the board
    ui8 piece = board->content[move.from_file][move.from_rank];
    board->content[move.from_file][move.from_rank] = CB_EMPTY;

    //Step 2: Evaluate special move
    //En passant capture
    if(legal == 5) {
        board->content[move.to_file][move.from_rank] = CB_EMPTY;
        board->content[move.to_file][move.to_rank] = piece;
        board->META_PASSANT_FILE = 8;
        swret 5;
    }
    board->META_PASSANT_FILE = 8;
    //Promotion
    if(legal > 7 && legal < 12) {
        switch(legal) {
            case 8:
                piece &= ~CB_PAWN;
                piece |=  CB_ROOK;
            break;
            case 9:
                piece &= ~CB_PAWN;
                piece |=  CB_KNIGHT;
            break;
            case 10:
                piece &= ~CB_PAWN;
                piece |=  CB_BISHOP;
            break;
            case 11:
                piece &= ~CB_PAWN;
                piece |=  CB_QUEEN;
            break;
        }

        //Direction
        signed short direction = (curcol == 1) ? 1 : -1;
        ui8 rank = move.from_rank + direction;
        ui8 file = move.from_file;

        //Extra capture check
        if(move.special == 1 && (Board_color_field(*board, move.from_file - 1, move.from_rank + direction) != Board_color_field(*board, move.from_file, move.from_rank))) {
            file++;
        }
        else if(move.special == 2 && (Board_color_field(*board, move.from_file + 1, move.from_rank + direction) != Board_color_field(*board, move.from_file, move.from_rank))) {
            file--;
        }
        else if(move.special == 0 && !Board_color_field(*board, file, rank));
        //Neither forwards nor capture matches
        else {
            piece &= CB_WHITE_MASK | CB_BLACK_MASK;
            piece |= CB_PAWN;

            board->content[move.from_file][move.from_rank] = piece;
            return 0;
        }

        //Return
        board->content[file][rank] = piece;
        swret legal;
    }
    //Queen's side castle
    if(legal == 6) {
        ui8 piecerook = board->content[move.to_file][move.to_rank];
        board->content[move.to_file][move.to_rank] = CB_EMPTY;
        board->META_CASTLE_A &= ~(3-curcol);
        board->META_CASTLE_H &= ~(3-curcol);
        board->content[CB_FILE_C][move.to_rank] = piece;
        board->content[CB_FILE_D][move.to_rank] = piecerook;
        swret 6;
    }
    //King's side castle
    if(legal == 7) {
        ui8 piecerook = board->content[move.to_file][move.to_rank];
        board->content[move.to_file][move.to_rank] = CB_EMPTY;
        board->META_CASTLE_A &= ~(3-curcol);
        board->META_CASTLE_H &= ~(3-curcol);
        board->content[CB_FILE_G][move.to_rank] = piece;
        board->content[CB_FILE_F][move.to_rank] = piecerook;
        swret 7;
    }

    //Step 3: King's or Rook's move clearing meta castle flag
    //King resets flags for both
    if((piece & ~(CB_WHITE_MASK | CB_BLACK_MASK)) == CB_KING) {
        board->META_CASTLE_A &= ~(3-curcol);
        board->META_CASTLE_H &= ~(3-curcol);
    }
    //Rook A resets A flag
    if((piece & ~(CB_WHITE_MASK | CB_BLACK_MASK)) == CB_ROOK) {
        board->META_CASTLE_A &= ~(3-curcol);
    }
    //and Rook H resets H flag
    if((piece & ~(CB_WHITE_MASK | CB_BLACK_MASK)) == CB_ROOK) {
        board->META_CASTLE_H &= ~(3-curcol);
    }

    //Step 4: Check Pawn double rank and set en passant flag
    if((piece & ~(CB_WHITE_MASK | CB_BLACK_MASK)) == CB_PAWN && (abs((signed) move.from_rank - (signed) move.to_rank) == 2)) {
        board->META_PASSANT_FILE = move.from_file;
    }

    //Step 5: Move the piece to the field, possibly erasing another piece
    board->content[move.to_file][move.to_rank] = piece;
    swret legal;
}

/** @fn ui8 Board_in_check(Board_t board, ui8 color)
 * @brief Gets if a color is in check
 * @param board To be checked
 * @param color 1 = white, 2 = black
 * @returns 1 = in check || 0 = not
 */
ui8 Board_in_check(Board_t board, ui8 color) {
    //Get king of color
    ui8 mask = (color == 1) ? CB_WHITE_MASK : CB_BLACK_MASK;

    ui8 kfile;
    ui8 krank;

    for(ui8 r = 0; r < 8; r++) for(ui8 f = 0; f < 8; f++) {
        if(board.content[f][r] == (mask | CB_KING)) {
            kfile = f;
            krank = r;
            break;
        }
    }

    //Stuff
    BoardMove_t move = {.to_file = kfile, .to_rank = krank};

    for(ui8 r = 0; r < 8; r++) for(ui8 f = 0; f < 8; f++) {
        move.from_file = f;
        move.from_rank = r;
        if(Board_legal_move(board, move) == 2)
            return 1;
    }
    return 0;
}

/** @fn ui8 Board_in_check(Board_t board, ui8 color)
 * @brief Gets if a color is in checkmate
 * @param board To be checked
 * @param color 1 = white, 2 = black
 * @returns 1 = in check || 0 = not
 */
ui8 Board_in_mate(Board_t board, ui8 color) {
    if(!Board_in_check(board, color))
        return 0;

    Board_t check;
    Board_clone(&check, board);

    //Check every piece and every move
    BoardMove_t move;

    for(move.from_file = 0; move.from_file < 8; move.from_file++)
    for(move.from_rank = 0; move.from_rank < 8; move.from_rank++)
    for(move.to_file = 0; move.to_file < 8; move.to_file++)
    for(move.to_rank = 0; move.to_rank < 8; move.to_rank++) {
        //Upon legal move
        ui8 ret = Board_apply(&check, move);
        if(!(ret == 0 || ret == 3 || ret == 4)) {
            //printf("\n<%i,%i,%i,%i>\n", move.from_file, move.from_rank, move.to_file, move.to_rank);
            return 0;
        }
    }
    
    return 1;
}

/** @fn ui8 Board_clone(Board_t *into, Board_t from)
 * @brief Clones board content
 * @param into Board to be cloned into
 * @param from Original board
 * @returns none
 */
void Board_clone(Board_t *into, Board_t from) {
    for(ui8 r = 0; r < 8; r++) for(ui8 f = 0; f < 8; f++) into->content[f][r] = from.content[f][r];
    into->turn = from.turn;
    into->META_PASSANT_FILE = from.META_PASSANT_FILE;
    into->META_CASTLE_H = from.META_CASTLE_H;
    into->win = from.win;
    into->META_CASTLE_A = from.META_CASTLE_A;
    into->CB_ALLOW_DEFAULT_SPM = from.CB_ALLOW_DEFAULT_SPM;
}

/** @fn ui8 Tool_Expression_Match(char *literal, char *match)
 * @brief Match expression, | = file, _ = rank, @ = piece
 * @param literal To be checked
 * @param match Pattern
 * @returns 1 = valid || 0 = invalid
 */
ui8 Tool_Expression_Match(char *literal, const char *match) {
	char cl = literal[0];
	char cm = match[0];
	long ix = 1;

	while(cl && cm) {

		//Any non-pawn piece match
		if(cm == '@') {
			//Only valid letters
			if(cl != 'K' && cl != 'Q' && cl != 'R' && cl != 'N' && cl != 'B') return 0;
		}

		//Any file match
		else if(cm == '|') {
			//Only A-H
			if(cl < 'A' || cl > 'H') return 0;
		}

		//Any rank match
		else if(cm == '_') {
			//Only 1-8
			if(cl < '1' || cl > '8') return 0;
		}

		//Direct match
		else if(cm != cl) return 0;

		//Next char
		cl = literal[ix];
		cm = match[ix++];
	}

	//Both must be 0 here
	if(cl == cm) return 1;
	return 0;
}

/** @fn ui8 Board_translate_expression(Board_t board, BoardMove_t *move, char *literal)
 * @brief String expression will be translated to move struct, validity of move will be checked
 * @param board Board to be checked
 * @param move Pointer to move struct
 * @param literal Expression
 * @returns 1 - is legal move || 0 - is not || 2 - capture || 3 - piece error || 4 - general expression error
 */
ui8 Board_translate_expression(Board_t board, BoardMove_t *move, char literal[]) {

	//Caps-up literal
	long ix = 0;
	while(literal[ix]) {
		if(literal[ix] >= 'a' && literal[ix] <= 'z')
			literal[ix] -= 'a'-'A';
		ix++;
	}
	ix = 0;
	
	//Move Pawn forward, generalized (incl. promotion), check all pieces on rank
	//e.g. A6 || E4
	if(Tool_Expression_Match(literal, "|_") || Tool_Expression_Match(literal, "|_=@")) {
        ui8 file = literal[0] - 'A';
        ui8 rank = 0;
        ui8 found = 0;
        //Get pawn rank
		for(ix = 0; ix < 8; ix++) {
            if(((Board_color_field(board, file, ix) == 1 && board.turn == CB_TURN_WHITE) ||
                (Board_color_field(board, file, ix) == 2 && board.turn == CB_TURN_BLACK)) &&
                ((board.content[file][ix] & ~(CB_WHITE_MASK | CB_BLACK_MASK)) == CB_PAWN)) {
                if(found)
                    return 4;
                else {
                    found = 1;
                    rank = ix;
                }
            }
        }
        //End reach
        if(!found) return 3;

        move->from_file = file;
        move->from_rank = rank;
        move->to_file = file;
        move->to_rank = literal[1] - '1'; 

        //Promotion process
        if(Tool_Expression_Match(literal, "|_=@")) {
            move->to_rank = (Board_color_field(board, move->from_file, move->from_rank) == 1) ? 0 : 7;
            switch(literal[3]) {
                case 'R':
                    move->to_file = 0;
                break;
                case 'N':
                    move->to_file = 1;
                break;
                case 'B':
                    move->to_file = 2;
                break;
                case 'Q':
                    move->to_file = 3;
                break;
                default:
                    return 4;
            }
        }
        move->special = 0;

        return Board_legal_move(board, *move);
	}

	//Move Pawn forward (incl. promotion), specified
	//e.g. E2E3 || B2B1
	else if(Tool_Expression_Match(literal, "|_|_") || Tool_Expression_Match(literal, "|_|_=@")) {
		move->from_file = literal[0]-'A';
        move->from_rank = literal[1]-'1';
        move->to_file = literal[2]-'A';
        move->to_rank = literal[3]-'1';

        //check for pawn and correct color at from
        if(!(((Board_color_field(board, move->from_file, move->from_rank) == 1 && board.turn == CB_TURN_WHITE) ||
            (Board_color_field(board, move->from_file, move->from_rank) == 2 && board.turn == CB_TURN_BLACK)) &&
            ((board.content[move->from_file][move->from_rank] & ~(CB_WHITE_MASK | CB_BLACK_MASK)) == CB_PAWN)))
            return 3;

        //Promotion process
        if(Tool_Expression_Match(literal, "|_|_=@")) {
            move->to_rank = (Board_color_field(board, move->from_file, move->from_rank) == 1) ? 0 : 7;
            switch(literal[5]) {
                case 'R':
                    move->to_file = 0;
                break;
                case 'N':
                    move->to_file = 1;
                break;
                case 'B':
                    move->to_file = 2;
                break;
                case 'Q':
                    move->to_file = 3;
                break;
                default:
                    return 4;
            }
        }
        move->special = 0;

        return Board_legal_move(board, *move);
	}

	//Capture with Pawn, generalized, check all pieces on file
	//e.g. ExD || AxB
	else if(Tool_Expression_Match(literal, "|X|") || Tool_Expression_Match(literal, "|X|=@")) {
        ui8 file = literal[0] - 'A';
        ui8 rank = 0;
        ui8 found = 0;
        //Get pawn rank
		for(ix = 0; ix < 8; ix++) {
            if(((Board_color_field(board, file, ix) == 1 && board.turn == CB_TURN_WHITE) ||
                (Board_color_field(board, file, ix) == 2 && board.turn == CB_TURN_BLACK)) &&
                ((board.content[file][ix] & ~(CB_WHITE_MASK | CB_BLACK_MASK)) == CB_PAWN)) {
                if(found)
                    return 4;
                else {
                    found = 1;
                    rank = ix;
                }
            }
        }
        //End reach
        if(!found) return 3;

        //Determine rank direction
        signed short direction = (Board_color_field(board, file, rank) == 1) ? 1 : -1;

        move->from_file = file;
        move->from_rank = rank;
        move->to_file = literal[2] - 'A';
        move->to_rank = rank + direction;

        //Promotion process
        if(Tool_Expression_Match(literal, "|X|=@")) {
            ui8 pf = move->to_file;
            move->to_rank = (Board_color_field(board, move->from_file, move->from_rank) == 1) ? 0 : 7;
            switch(literal[4]) {
                case 'R':
                    move->to_file = 0;
                break;
                case 'N':
                    move->to_file = 1;
                break;
                case 'B':
                    move->to_file = 2;
                break;
                case 'Q':
                    move->to_file = 3;
                break;
                default:
                    return 4;
            }

            if(Board_legal_move(board, *move))
                move->special = (move->from_file < pf) ? 1 : 2;
        }

        return Board_legal_move(board, *move);
		//TODO
	}

	//Capture with Pawn, specified
	//e.g. E5xF || G3xH
	else if(Tool_Expression_Match(literal, "|_X|") || Tool_Expression_Match(literal, "|_X|=@")) {
		move->from_file = literal[0]-'A';
        move->from_rank = literal[1]-'1';
        move->to_file = literal[3]-'A';

        signed short direction = (Board_color_field(board,  move->from_file, move->from_rank) == 1) ? 1 : -1;
        move->to_rank = move->from_rank + direction;

        //check for pawn and correct color at from
        if(!(((Board_color_field(board, move->from_file, move->from_rank) == 1 && board.turn == CB_TURN_WHITE) ||
            (Board_color_field(board, move->from_file, move->from_rank) == 2 && board.turn == CB_TURN_BLACK)) &&
            ((board.content[move->from_file][move->from_rank] & ~(CB_WHITE_MASK | CB_BLACK_MASK)) == CB_PAWN)))
            return 3;

        //Promotion process
        if(Tool_Expression_Match(literal, "|_X|=@")) {
            ui8 pf = move->to_file;
            move->to_rank = (Board_color_field(board, move->from_file, move->from_rank) == 1) ? 0 : 7;
            switch(literal[5]) {
                case 'R':
                    move->to_file = 0;
                break;
                case 'N':
                    move->to_file = 1;
                break;
                case 'B':
                    move->to_file = 2;
                break;
                case 'Q':
                    move->to_file = 3;
                break;
                default:
                    return 4;
            }

            if(Board_legal_move(board, *move))
                move->special = (move->from_file < pf) ? 1 : 2;
        }

        return Board_legal_move(board, *move);
	}

	//Move with Piece, generalized, check all specific pieces on board
	//e.g. QH5 || NC3
	else if(Tool_Expression_Match(literal, "@|_") || Tool_Expression_Match(literal, "@X|_")) {
        // The X is a cosmetic that some users will most likely use for capturing
        char tfc = literal[1];
        char trc = literal[2];

        ui8 expect_capture = 0;

        if(Tool_Expression_Match(literal, "@X|_")) {
            tfc = literal[2];
            trc = literal[3];
            expect_capture = 1;
        }

        ui8 valid = 0;
        ui8 piece;

        // Get piece code
        switch(literal[0]) {
            case 'R': piece = CB_ROOK; break;
            case 'N': piece = CB_KNIGHT; break;
            case 'B': piece = CB_BISHOP; break;
            case 'Q': piece = CB_QUEEN; break;
            case 'K': piece = CB_KING; break;
            default: return 4;
        }
        // Apply color mask
        piece |= (board.turn == CB_TURN_WHITE) ? CB_WHITE_MASK : CB_BLACK_MASK;

        move->to_file = tfc - 'A';
        move->to_rank = trc - '1';

        ui8 file = 8;
        ui8 rank = 8;

        // Iterate all movements, if multiple of the same type of piece can perform the same move, return 4
        for(ui8 r = 0; r < 8; r++) for(ui8 f = 0; f < 8; f++) {
            if(board.content[f][r] == piece) {
                move->from_file = f;
                move->from_rank = r;

                if(Board_legal_move(board, *move)) {
                    //Multiple pieces can perform this move!
                    if(valid) return 4;
                    valid = 1;
                    file = f;
                    rank = r;
                }
            }
        }
        
        //Check if pieces were able to perform this
        if(file == 8) return 3;

        //Post set and if expecting capture
        move->from_file = file;
        move->from_rank = rank;

        ui8 ret = Board_legal_move(board, *move);
        if(expect_capture && ret != 2)
            return 4;
        
        return ret;
	}

	//Move with Piece, specific
	//e.g. NF3D4 || RA8A5
	else if(Tool_Expression_Match(literal, "@|_|_") || Tool_Expression_Match(literal, "@|_X|_")) {
		// The X is a cosmetic that some users will most likely use for capturing
        char tfc = literal[3];
        char trc = literal[4];

        ui8 expect_capture = 0;

        if(Tool_Expression_Match(literal, "@|_X|_")) {
            tfc = literal[4];
            trc = literal[5];
            expect_capture = 1;
        }

        ui8 valid = 0;
        ui8 piece;

        // Get piece code
        switch(literal[0]) {
            case 'R': piece = CB_ROOK; break;
            case 'N': piece = CB_KNIGHT; break;
            case 'B': piece = CB_BISHOP; break;
            case 'Q': piece = CB_QUEEN; break;
            case 'K': piece = CB_KING; break;
            default: return 4;
        }
        // Apply color mask
        piece |= (board.turn == CB_TURN_WHITE) ? CB_WHITE_MASK : CB_BLACK_MASK;

        //Check if piece is correct
        move->from_file = literal[1] - 'A';
        move->from_rank = literal[2] - '1';
        move->to_file = tfc - 'A';
        move->to_rank = trc - '1';

        if(board.content[move->from_file][move->from_rank] != piece)
            return 3;

        //if expecting capture
        ui8 ret = Board_legal_move(board, *move);
        if(expect_capture && ret != 2)
            return 4;
        
        return ret;
	}

    //Castle king's side
    else if(Tool_Expression_Match(literal, "0-0")) {
        //Literally just convert and do this lmao
        literal[0] = 'K';
        literal[1] = 'H';
        literal[2] = '8';
        if(board.turn == CB_TURN_WHITE)
            literal[2] = '1';

        return Board_translate_expression(board, move, literal);
    }

    //Castle queen's side
    else if(Tool_Expression_Match(literal, "0-0-0")) {
        //Literally just convert and do this lmao
        literal[0] = 'K';
        literal[1] = 'A';
        literal[2] = '8';
        literal[3] = 0;
        if(board.turn == CB_TURN_WHITE)
            literal[2] = '1';

        return Board_translate_expression(board, move, literal);
    }

    return 4;
}