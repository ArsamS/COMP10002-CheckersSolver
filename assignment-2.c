/* Program to print and play checker games.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  September 2021, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: Arsam Samadi Bahrami
  Dated: 10/10/2021

*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <ctype.h>

/* some #define's from my sample solution ------------------------------------*/
#define BOARD_SIZE 8       // board size
#define ROWS_WITH_PIECES 3 // number of initial rows with pieces
#define CELL_EMPTY '.'     // empty cell character
#define CELL_BPIECE 'b'    // black piece character
#define CELL_WPIECE 'w'    // white piece character
#define CELL_BTOWER 'B'    // black tower character
#define CELL_WTOWER 'W'    // white tower character
#define COST_PIECE 1       // one piece cost
#define COST_TOWER 3       // one tower cost
#define TREE_DEPTH 3       // minimax tree depth
#define COMP_ACTIONS 10    // number of computed actions

/* one type definition from my sample solution -------------------------------*/
typedef unsigned char board_t[BOARD_SIZE][BOARD_SIZE]; // board type

/****************************************************************/

#define HOR_DIVIDER "   +---+---+---+---+---+---+---+---+"
    // board  divider for each  row
#define VER_DIVIDER '|'                 
    // board divider for each column
#define MOVE_DIVIDER "====================================="
    // divider for every move
#define PIECES_PER_ROW 4
    // number of initial pieces per row
#define INITIAL_PIECES PIECES_PER_ROW * ROWS_WITH_PIECES
    // number of initial pieces
#define ALPHABET_BEGIN 65
    // Unicode decimal for when Capital A begins
#define COMPUTED_SIGN "***"
    // Printed to show an action was computed
#define DIRECTIONS 4
    //  number of directions (NE, SE, SW, NW)

typedef struct node node_t;

void initial_setup(board_t board);
void print_board(board_t board, char source_col,
    int source_row, char dest_col, int dest_row,
    int move_no, char player, int computed);
int test_move(board_t board, char source_col,
    int source_row, char dest_col, int dest_row,
    int move_no, char player, int show_errors,
    int computed);
int capture(board_t board, char source_col,
    int source_row, char dest_col, int dest_row,
    char source_cell, char player);
int convert_col(char col_letter);
char convert_col_back(int col_letter);
int convert_row(int row_no);
int convert_row_back(int row_no);
int find_cost(board_t board);
void display_error(int error_id);
void create_tree(board_t board, char player,
int move_no, int level, node_t *root);
char find_player(int move_no);
void execute_command(board_t board, int move_no, char command);
void next_action(board_t board, int move_no, node_t *root);
void check_victory(board_t board);
int minmax_tree(node_t *root, int max, int level);
void recursive_free(node_t *root);

node_t *create_node(int source_row, int source_col,int dest_row, int dest_col);
node_t *add_sibling(node_t *root, int source_row, int source_col,
int dest_row, int dest_col);
node_t *add_child(node_t *root, int source_row, int source_col,
int dest_row, int dest_col);
node_t *find_at_depth_one(node_t *root, int value);


struct node
{
    int source_row;
    int source_col;
    int dest_row;
    int dest_col;
    int board_cost;
    node_t *child;
    node_t *sibling;
};

/*
the following website was used to help develop my understanding of how
to organise child sibling trees which I was finding very difficult

https://www.geeksforgeeks.org/
creating-tree-left-child-right-sibling-representation/
*/

// used to create a new node in the tree, storing information about its
// source and destination cell + its child and next sibling
node_t
*create_node(int source_row, int source_col,
int dest_row, int dest_col)
{
    struct node* node = (struct node*)malloc(sizeof(struct node));
    node->source_row =  source_row;
    node->source_col =  source_col;
    node->dest_row =  dest_row;
    node->dest_col =  dest_col;
    
    node->child = NULL;
    node->sibling = NULL;
    return node;
}

// used to add a sibling to another node in the tree
node_t
*add_sibling(node_t *root, int source_row, int source_col,
int dest_row, int dest_col)
{
    if (root == NULL)
    {
        return NULL;
    }
    // follow the path of siblings until it can be inserted
    while (root->sibling)
    {
        root = root->sibling;
    }

    return (root->sibling = create_node(source_row, source_col,
        dest_row, dest_col));
}

// used to add a child to another node in the tree
node_t
*add_child(node_t *root, int source_row, int source_col,
int dest_row, int dest_col)
{
    if (root == NULL)
    {
        return NULL;
    }
    // if the node already has a child, add the node as its sibling
    if (root->child)
    {
        return add_sibling(root->child, source_row,
            source_col, dest_row, dest_col);
    }
    else
    {
        return (root->child = create_node(source_row,
            source_col, dest_row, dest_col));   
    }
}

/*

// this  section was only used for debugging and visualisation of the tree

void
create_spaces(int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        printf("\t");
    }
    
}

void
traverse_tree(node_t *root, int level)
{
    if (root == NULL)
    {
        return;
    }
    while (root)
    {
        create_spaces(level);

        printf("(%d) ", root->board_cost);
        printf("%c%d, %c%d\n", convert_col_back(root->source_col),
        convert_row_back(root->source_row), convert_col_back(root->dest_col),
        convert_row_back(root->dest_row));
        if (root->child)
        {
            traverse_tree(root->child, level + 1);
        }
        root = root->sibling;
    }
}

*/

// used to assign cost values to children, and minmax values to the parents
int 
minmax_tree(node_t *root, int max, int level)
{
    int max_val =  0, next = 1, first = 1;
    // go from max to min and vice versa for the next layer
    if (max)
    {
        next = 0;
    }
    while (root)
    {
        // store the most min or max value of the child in the parent
        if (root->child)
        {
            root->board_cost = minmax_tree(root->child, next, level + 1);
        }
        if (first)
        {
            max_val = root->board_cost;
            first = 0;
        }
        // change functionality based on if finding max or min
        if (max)
        {
            if (root->board_cost < max_val)
            {
                max_val = root->board_cost;
            }
        }
        else
        {
            if (root->board_cost > max_val)
            {
                max_val = root->board_cost;
            }
        }
        // move to the next sibling and repeat
        root = root->sibling;
    }
    return max_val;
}

// this function is used along the minmax function to find the
// left-most occurence of the desired cost
node_t
*find_at_depth_one(node_t *root, int value)
{
    // search within the children of the root
    root = root->child;
    while (root)
    {
        if ((root->board_cost) == value)
        {
            return root;
        }
        root = root->sibling;
    }
    return NULL;
}

// function used to recursively free each node in the tree
void
recursive_free(node_t *root)
{
    if (root)
    {
        recursive_free(root->child);
        recursive_free(root->sibling);
        free(root);
    }
}

/****************************************************************/

int main(int argc, char *argv[])
{
    // use helper function to initialise the board, then print it
    board_t board;
    initial_setup(board);
    char source_col = 0, dest_col = 0, player = 0, command = 0;
    int source_row = 0, dest_row = 0,
        move_no = 0, scans, show_errors = 1;
    print_board(board, source_col, source_row,
        dest_col, dest_row, move_no, player, 0);

    move_no ++;
    
    // scan the text file for moves and commands
    while ((scans = scanf("%c%d - %c%d\n", &source_col, &source_row,
        &dest_col, &dest_row)) && (scans != EOF))
    {
        // test the legality of a detected move
        if (scans == 4)
        {
            test_move(board, source_col, source_row,
                dest_col, dest_row, move_no,
                find_player(move_no), show_errors, 0);
            move_no ++;
        }
        // store either the 'A' or 'P' commands if found
        if (scans == 1)
        {
            command = source_col;
        }
    }

    if (command)
    {
        execute_command(board, move_no, command);
    }

    return EXIT_SUCCESS; // exit program with the success code
}

/****************************************************************/

// this  function is used to carry out the 'A' or 'P' command
void
execute_command(board_t board, int move_no, char command)
{
    // alter repetitions based on command type
    int i, limit = 1;

    if (command == 'P')
    {
        limit = COMP_ACTIONS;
    }
    // for each repetition create a fresh tree, find the next action,
    // and then free the tree
    for (i = 0; i < limit; i++)
    {
        node_t *root = create_node(0, 0, 0, 0);
        create_tree(board, find_player(move_no), move_no, 1, root);
        next_action(board, move_no, root);
        recursive_free(root);
        root = NULL;
        move_no ++;
    }
}

// this function is used to search for victory early to avoid errors
void
check_victory(board_t board)
{
    int row, col, w_tally = 0, b_tally = 0;
    char winner;
    for (row = 0; row < BOARD_SIZE; row++)
    {
        for (col = 0; col < BOARD_SIZE; col++)
        {
            if (tolower(board[row][col]) == CELL_WPIECE)
            {
                w_tally ++;
            }
            if (tolower(board[row][col]) == CELL_BPIECE)
            {
                b_tally ++;
            }
        }
    }
    // victor is found if opponent has ran out of pieces
    if (w_tally == 0)
    {
        winner = CELL_BPIECE;
    }
    if (b_tally == 0)
    {
        winner = CELL_WPIECE;
    }
    if (winner == CELL_BPIECE)
    {
        printf("BLACK WIN!\n");
        exit(EXIT_SUCCESS);
    }
    if (winner == CELL_WPIECE)
    {
        printf("WHITE WIN!\n");
        exit(EXIT_SUCCESS);
    }
}

// used to calculate the next action based on the calculated tree
void
next_action(board_t board, int move_no, node_t *root)
{
    int max = 1, value, source_row, dest_row;
    char source_col, dest_col, player;

    // set min or max preference based on if player is white or black
    player = find_player(move_no);
    if (player == CELL_WPIECE)
    {
        max = 0;
    }

    // determine the next action based on minmax calculations and check
    // if there is already a victor
    value = minmax_tree(root, max, 0);
    check_victory(board);

    // helper function used to alter the board and print it
    node_t* ptr = find_at_depth_one(root, value); 
    source_col = convert_col_back(ptr->source_col);
    source_row = convert_row_back(ptr->source_row);
    dest_col = convert_col_back(ptr->dest_col);
    dest_row = convert_row_back(ptr->dest_row);

    test_move(board, source_col, source_row, dest_col,
    dest_row, move_no, player, 1, 1);
}

// short helper function to find player type based on if the action number
// is odd or even
char
find_player(int move_no)
{
    char player;

    player = CELL_WPIECE;
    if (move_no % 2 != 0)
    {
        player = CELL_BPIECE;
    }

    return player;
}


/****************************************************************/

// used to create a tree of depth three of possible moves
void
create_tree(board_t board, char player, int move_no, int level, node_t *root)
{
    // array set up to loop through direction moves starting with NE
    int add[4][2] = {{-1, 1}, {1,1}, {1,-1}, {-1,-1}};
    
    int row, col, i, j, dir, multiply_factor,
    k, l, source_row, dest_row, test;
    char new_player, source_col, dest_col;
    node_t *id;

    board_t clone_board, test_board;

    for (row = 0; row < BOARD_SIZE; row++)
    {
        for (col = 0; col < BOARD_SIZE; col++)
        {
            // create a clone board in order to not affect the main board
            for (i = 0; i < BOARD_SIZE; i++)
            {
                for (j = 0; j < BOARD_SIZE; j++)
                {
                    clone_board[i][j] = board[i][j];
                }
            }
            // travel through via row-major order
            if (player == tolower(board[row][col]))
            {
                // try all four directions
                for (dir = 0; dir < DIRECTIONS; dir++)
                {
                    // create a second clone board to maintain a 
                    // consistent tree
                    for (k = 0; k < BOARD_SIZE; k++)
                    {
                        for (l = 0; l < BOARD_SIZE; l++)
                        {
                            test_board[k][l] = clone_board[k][l];
                        }
                    }
                    // attempt both moves and captures
                    for (multiply_factor = 1; multiply_factor <= 2;
                    multiply_factor++)
                    {
                        source_row = convert_row_back(row);
                        source_col = convert_col_back(col);
                        dest_row = convert_row_back(row +
                            multiply_factor * add[dir][0]);
                        dest_col = convert_col_back(col +
                            multiply_factor * add[dir][1]);      

                        // if a move is possible, add it to the tree
                        test = test_move(test_board, source_col, source_row,
                        dest_col, dest_row, move_no, player, 0, 0);
                        if (test)
                        {
                            if (player == CELL_WPIECE)
                            {
                                new_player = CELL_BPIECE;
                            }
                            if (player == CELL_BPIECE)
                            {
                                new_player = CELL_WPIECE;
                            }

                            id = add_child(root, row, col, row +
                            multiply_factor * add[dir][0],
                            col + multiply_factor * add[dir][1]);
                            // recursion for children if depth is < 3
                            if (level < TREE_DEPTH)
                            {
                                create_tree(test_board, new_player,
                                move_no + 1, level + 1, id);
                            }
                            // assign board costs for children at max depth
                            if (level == TREE_DEPTH)
                            {
                                id->board_cost = find_cost(test_board);
                            }
                        }
                    }
                }
            }
        }
    }
}

/****************************************************************/

// set up the initial layout of the board
void
initial_setup(board_t board)
{
    int row, col, odd;

    for (row = 0; row < BOARD_SIZE; row++)
    {
        for (col = 0; col < BOARD_SIZE; col++)
        {
            // assign even cells in first three rows to white
            odd = row % 2;
            board[row][col] = CELL_EMPTY;
            if (row < ROWS_WITH_PIECES)
            {
                if ((col % 2) != odd)
                {
                    board[row][col] = CELL_WPIECE;
                }
            }
            // assign odd cells in the last three rows to black
            if (row > ROWS_WITH_PIECES + 1)
            {
                if ((col % 2) != odd)
                {
                    board[row][col] = CELL_BPIECE;
                }
            }
            odd ++;
        }
    }
}

/****************************************************************/

// print the current state of the inputted board
void
print_board(board_t board, char source_col,
int source_row, char dest_col, int dest_row,
int move_no, char player, int computed)
{
    int row, col;    
    // initial set-up text
    if (move_no == 0)
    {
        printf("BOARD SIZE: %dx%d\n", BOARD_SIZE, BOARD_SIZE);
        printf("#BLACK PIECES: %d\n", INITIAL_PIECES);
        printf("#WHITE PIECES: %d\n", INITIAL_PIECES);
    }
    // formatting the top section
    else
    {
        printf("%s\n", MOVE_DIVIDER);
        if (computed)
        {
            printf("%s ", COMPUTED_SIGN);
        }
        if (player == CELL_WPIECE)
        {
            printf("WHITE ACTION #%d: ", move_no);
        }
        if (player == CELL_BPIECE)
        {
            printf("BLACK ACTION #%d: ", move_no);
        }
        printf("%c%d-%c%d\n", source_col, source_row, dest_col, dest_row);
        printf("BOARD COST: %d\n", find_cost(board));
    }

    // more formatting for the headers
    for (col = 0; col < BOARD_SIZE; col++)
    {
        if (col == 0)
        {
            printf("  ");
        }

        printf("%4c", col + ALPHABET_BEGIN);

        if (col == BOARD_SIZE - 1)
        {
            printf("\n");
        } 
    }

    // formatting for cells
    for (row = 0; row < BOARD_SIZE; row++)
    {
        printf("%s\n", HOR_DIVIDER);
        printf(" %d ", row + 1);
        for (col = 0; col < BOARD_SIZE; col++)
        {
            printf("%c", VER_DIVIDER);
            printf(" %c ", board[row][col]);
            if (col == BOARD_SIZE - 1)
            {
                printf("%c\n", VER_DIVIDER);
            }
        }
        if (row == BOARD_SIZE - 1)
        {
            printf("%s\n", HOR_DIVIDER);
        }
    }
}

/****************************************************************/

// test whether a move is valid, alter the board, and call to print
int
test_move(board_t board, char source_col, 
int source_row, char dest_col, int dest_row,
int move_no, char player, int show_errors,
int computed) 
{
    char source_cell, dest_cell;
    int col_gap, row_gap,
    running = 1, error_id = 0, result = 0;
    
    // while loop used to be able to exit in the case of an error
    while (running)
    {
        // find if source cell or destination cell are out of bounds
        if ((convert_col(source_col) >= BOARD_SIZE)
        || (convert_col(source_col) < 0)
        || (source_row > BOARD_SIZE)
        || (source_row < 1))
        {
            error_id = 1;
            break;
        }
        if  ((convert_col(dest_col) >= BOARD_SIZE)
        || (convert_col(dest_col) < 0)
        || (dest_row > BOARD_SIZE) 
        || (dest_row < 1))
        {
            error_id = 2;
            break;
        }

        source_cell = board[convert_row(source_row)][convert_col(source_col)];
        dest_cell = board[convert_row(dest_row)][convert_col(dest_col)];

        // find if source cell is non-empty and destination is empty
        if (source_cell == CELL_EMPTY)
        {
            error_id = 3;
            break;
        }
        if (dest_cell != CELL_EMPTY)
        {
            error_id = 4;
            break;
        }
        
        // make sure own piece is being moved
        if (((player == CELL_WPIECE) &&
        (tolower(source_cell) == CELL_BPIECE)) ||
        (((player == CELL_BPIECE) &&
        (tolower(source_cell) == CELL_WPIECE))))
        {
            error_id = 5;
            break;
        }
        
        col_gap = dest_col - source_col;
        row_gap = dest_row - source_row;

        // make sure no illegal jumps are being made
        if (abs(row_gap) == abs(col_gap))
        {
            if (((source_cell ==  CELL_WPIECE) && (row_gap < 0)) ||
            ((source_cell ==  CELL_BPIECE) && (row_gap > 0))){
                error_id = 6;
                break;
            }
            if (abs(row_gap) == 1)
            {
                // alter the board in case of a simple move
                board[convert_row(dest_row)]
                     [convert_col(dest_col)] = source_cell;
                board[convert_row(source_row)]
                     [convert_col(source_col)] = CELL_EMPTY;
            }
            else if (abs(row_gap) == 2)
            {
                // call a helper function in case of an attempted capture
                if (capture(board, source_col, source_row,
                    dest_col, dest_row, source_cell, player))
                {
                    result = 1;
                }
                else
                {
                    error_id = 6;
                    break;
                }
            }
            else
            {
                error_id = 6;
                break;
            }
            
            // convert to a tower if the end is reached
            if ((player == CELL_WPIECE) && (dest_row == BOARD_SIZE))
            {
                board[convert_row(dest_row)]
                [convert_col(dest_col)] = CELL_WTOWER;
            }
            if ((player == CELL_BPIECE) && (dest_row == 1))
            {
                board[convert_row(dest_row)]
                [convert_col(dest_col)] = CELL_BTOWER;
            }
            result = 1;

            if (show_errors)
            {
                // make sure the board isn't printed when the tree
                // is being constructed and that *** is displayed
                // during computed moves
                print_board(board, source_col, source_row,
                dest_col, dest_row, move_no, player, computed);
                 
            }
        }
        else
        {
            error_id = 6;
            break;
        }
        running = 0;
    }

    if (error_id)
    {
        // show errors if in stage 0
        if (show_errors)
        {
            display_error(error_id);
        }
        // say the move is invalid if in stage 1
        else
        {
            return 0;
        }
    }

    return result;
}

/****************************************************************/

// short functions to assist with switching back and forth from
// two formats  e.g. 0,0 to A1 and vice versa
int
convert_col(char col_letter){
    return col_letter - ALPHABET_BEGIN;
}

char
convert_col_back(int col_number){
    return col_number + ALPHABET_BEGIN;
}

int
convert_row(int row_no){
    return row_no - 1;
}

int
convert_row_back(int row_no){
    return row_no + 1;
}

/****************************************************************/

// calculate the cost of the board based on the provided formula
int
find_cost(board_t board){
    int row, col, cost = 0;
    for (row = 0; row < BOARD_SIZE; row++)
    {
        for (col = 0; col < BOARD_SIZE; col++)
        {
            if (board[row][col] == CELL_WPIECE)
            {
                cost -= COST_PIECE;
            }
            if (board[row][col] == CELL_BPIECE)
            {
                cost += COST_PIECE;
            }
            if (board[row][col] == CELL_WTOWER)
            {
                cost -= COST_TOWER;
            }
            if (board[row][col] == CELL_BTOWER)
            {
                cost += COST_TOWER;
            }
        }
    }
    return cost;
}

/****************************************************************/

// find if an attempted capture is valid
int
capture(board_t board, char source_col,
int source_row, char dest_col, int dest_row,
char source_cell, char player)
{
    int capture_row = 0, capture_col = 0;
    char capture_cell;
    
    capture_row  = (convert_row(source_row) + convert_row(dest_row)) / 2;
    capture_col = (convert_col(source_col) + convert_col(dest_col)) / 2;
    capture_cell = board[capture_row][capture_col];
    
    // make sure you can only capture the opposite team's pieces
    if (((player == CELL_WPIECE) && (tolower(capture_cell) == CELL_BPIECE))
    || ((player == CELL_BPIECE) && (tolower(capture_cell) == CELL_WPIECE)))
    {
        // alter the board
        board[convert_row(dest_row)][convert_col(dest_col)] = source_cell;
        board[convert_row(source_row)][convert_col(source_col)] = CELL_EMPTY;
        board[capture_row][capture_col] = CELL_EMPTY;
        return 1;
    }
    else
    {
        // return 0 if the attempt was not valid
        return 0;
    }
    
}

/****************************************************************/

// display errors in stage 0 and exit the program
void
display_error(int error_id)
{
    if (error_id == 1)
    {
        printf("ERROR: Source cell is outside of the board.\n");
    }
    if (error_id == 2)
    {
        printf("ERROR: Target cell is outside of the board.\n");
    }
    if (error_id == 3)
    {
        printf("ERROR: Source cell is empty.\n");
    }
    if (error_id == 4)
    {
        printf("ERROR: Target cell is not empty.\n");
    }
    if (error_id == 5)
    {
        printf("ERROR: Source cell holds opponent's piece/tower.\n");
    }
    if (error_id == 6)
    {
        printf("ERROR: Illegal action.\n");
    }
    exit(EXIT_FAILURE);
}

/* algorithms are fun !!! */