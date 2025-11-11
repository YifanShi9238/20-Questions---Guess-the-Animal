#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "lab5.h"

extern Node *g_root;
extern EditStack g_undo;
extern EditStack g_redo;
extern Hash g_index;

/* TODO 31: Implement play_game
 * Main game loop using iterative traversal with a stack
 * 
 * Key requirements:
 * - Use FrameStack (NO recursion!)
 * - Push frames for each decision point
 * - Track parent and answer for learning
 * 
 * Steps:
 * 1. Initialize and display game UI
 * 2. Initialize FrameStack
 * 3. Push root frame with answeredYes = -1
 * 4. Set parent = NULL, parentAnswer = -1
 * 5. While stack not empty:
 *    a. Pop current frame
 *    b. If current node is a question:
 *       - Display question and get user's answer (y/n)
 *       - Set parent = current node
 *       - Set parentAnswer = answer
 *       - Push appropriate child (yes or no) onto stack
 *    c. If current node is a leaf (animal):
 *       - Ask "Is it a [animal]?"
 *       - If correct: celebrate and break
 *       - If wrong: LEARNING PHASE
 *         i. Get correct animal name from user
 *         ii. Get distinguishing question
 *         iii. Get answer for new animal (y/n for the question)
 *         iv. Create new question node and new animal node
 *         v. Link them: if newAnswer is yes, newQuestion->yes = newAnimal
 *         vi. Update parent pointer (or g_root if parent is NULL)
 *         vii. Create Edit record and push to g_undo
 *         viii. Clear g_redo stack
 *         ix. Update g_index with canonicalized question
 * 6. Free stack
 */
void play_game() {
    clear();
    attron(COLOR_PAIR(5) | A_BOLD);
    mvprintw(0, 0, "%-80s", " Playing 20 Questions");
    attroff(COLOR_PAIR(5) | A_BOLD);
    
    mvprintw(2, 2, "Think of an animal, and I'll try to guess it!");
    mvprintw(3, 2, "Press any key to start...");
    refresh();
    getch();
    
    // TODO: Implement the game loop
    // Initialize FrameStack
    // Push root
    // Loop until stack empty or guess is correct
    // Handle question nodes and leaf nodes differently
    
    FrameStack stack;
    fs_init(&stack);

    if (g_root == NULL) {
    show_message("No knowledge yet! Please add an animal first.", 1);
    fs_free(&stack);
    return;
    }
    
    // TODO: Your implementation here
    fs_push(&stack, g_root, -1);

    Node *parent = NULL;
    int parentAnswer = -1;
    int guessedCorrectly = 0;

    while(!fs_empty(&stack)) {
        Frame frame = fs_pop(&stack);
        Node *current = frame.node;

        // Question node
        if (current->isQuestion == 1) {
            int answer = get_yes_no(5, 2, current->text);
            parent = current;
            parentAnswer = answer;

            if (answer) {
                fs_push(&stack, current->yes, 1);
            } else {
                fs_push(&stack, current->no, 0);
            }
        }

        //Animal (leaf) node
        else {
            char prompt[256];
            snprintf(prompt, sizeof(prompt), "Is it a %s?", current->text);
            int answer = get_yes_no(7, 2, prompt);

            clear();
            attron(COLOR_PAIR(5) | A_BOLD);
            mvprintw(0, 0, "%-80s", " Playing 20 Questions");
            attroff(COLOR_PAIR(5) | A_BOLD);

            if (answer) {
                attron(COLOR_PAIR(5) | A_BOLD);
                mvprintw(9, 2, "Yay! I guessed it right!");
                attroff(COLOR_PAIR(5) | A_BOLD);
                refresh();
                getch();
                guessedCorrectly = 1;
                break;
            } else {
                // Learning phase
                char *correctAnimal = get_input(9, 2, "I give up! What animal were you thinking of? ");
                char *distinguishingQuestion = get_input(11, 2, "Please provide a question that distinguishes your animal: ");
                int newAnswer = get_yes_no(13, 2, "For your animal, what is the answer to that question? (y/n) ");

                // Create new nodes
                Node *newAnimal = create_animal_node(correctAnimal);
                Node *newQuestion = create_question_node(distinguishingQuestion);

                if (newAnswer == 1) {
                    // Yes for the new question
                    newQuestion->yes = newAnimal;
                    newQuestion->no = current;
                } else {
                    // No for the new question
                    newQuestion->no = newAnimal;
                    newQuestion->yes = current;
                }
            
                // Update parent pointer or g_root
                if (parent == NULL) {
                    g_root = newQuestion;
                } else if (parentAnswer == 1) {
                    parent->yes = newQuestion;
                } else {
                    parent->no = newQuestion;
                }

                // Record edit for undo/redo
                Edit edit;
                edit.parent = parent;
                edit.oldLeaf = current;
                edit.newLeaf = newAnimal;
                edit.newQuestion = newQuestion;
                edit.wasYesChild = (parentAnswer == 1);
                es_push(&g_undo, edit);
                es_clear(&g_redo);
                attron(COLOR_PAIR(5) | A_BOLD);
                mvprintw(15, 2, "Thanks! I've learned something new.");
                attroff(COLOR_PAIR(5) | A_BOLD);
                refresh();
                getch();
                break;
            }     
        }   
        refresh();
    }

    fs_free(&stack);
}

/* TODO 32: Implement undo_last_edit
 * Undo the most recent tree modification
 * 
 * Steps:
 * 1. Check if g_undo stack is empty, return 0 if so
 * 2. Pop edit from g_undo
 * 3. Restore the tree structure:
 *    - If edit.parent is NULL:
 *      - Set g_root = edit.oldLeaf
 *    - Else if edit.wasYesChild:
 *      - Set edit.parent->yes = edit.oldLeaf
 *    - Else:
 *      - Set edit.parent->no = edit.oldLeaf
 * 4. Push edit to g_redo stack
 * 5. Return 1
 * 
 * Note: We don't free newQuestion/newLeaf because they might be redone
 */
int undo_last_edit() {
    // TODO: Implement this function
    if (es_empty(&g_undo))
        return 0;

    Edit edit = es_pop(&g_undo);

    if (edit.parent == NULL) {
        g_root = edit.oldLeaf;
    } else if (edit.wasYesChild) {
        edit.parent->yes = edit.oldLeaf;
    } else {
        edit.parent->no = edit.oldLeaf;
    }

    es_push(&g_redo, edit);

    return 1;
}

/* TODO 33: Implement redo_last_edit
 * Redo a previously undone edit
 * 
 * Steps:
 * 1. Check if g_redo stack is empty, return 0 if so
 * 2. Pop edit from g_redo
 * 3. Reapply the tree modification:
 *    - If edit.parent is NULL:
 *      - Set g_root = edit.newQuestion
 *    - Else if edit.wasYesChild:
 *      - Set edit.parent->yes = edit.newQuestion
 *    - Else:
 *      - Set edit.parent->no = edit.newQuestion
 * 4. Push edit back to g_undo stack
 * 5. Return 1
 */
int redo_last_edit() {
    // TODO: Implement this function
    if (es_empty(&g_redo))
        return 0;

    Edit edit = es_pop(&g_redo);

    if (edit.parent == NULL) {
        g_root = edit.newQuestion;
    } else if (edit.wasYesChild) {
        edit.parent->yes = edit.newQuestion;
    } else {
        edit.parent->no = edit.newQuestion;
    }

    es_push(&g_undo, edit);

    return 1;
}

