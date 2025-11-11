# ECE 312 - Lab 5 Write-Up

**Name:** Yifan Shi
**EID:** ys27523
**Date:** November 10th 2025

---

## 1. Design Decisions (3-4 sentences)

Briefly describe your key design choices and rationale:

- **Data structure implementations:** How did you approach the stack/queue/hash table? Any specific design trade-offs?

    There are both push/pop for both framestack and editstack so that the user and undo/redo tracking.
    The queue is used in BFS traversal for saving and loading the tree with enqueue and dequeue access to user.
    The array-based stack is simpler to implement compare to linked list, but ths trade off is occasional reallocations when the capacity equal the array size. 

- **Game loop strategy:** How did you structure the iterative traversal? What was your approach to the learning phase?
    The game loop was implemented iteratively using a framestack to avoid recursion, where each frame stores the current node and the user's previous answer. The loop begins by pushing the root node and repeatedly popping frames until an animal is reached. For question nodes, user responses determine whether to push the yes or no child next. 
    


- **Memory management:** What strategy did you use to avoid leaks? Any particular challenges?
    Dynamically allocated node is freed using free_tree(). Frame and edit stack is cleaned up using fs_free and es_free(). The main challenge was preventing double frees during undo/redo operations and play_game() function.

---

## 2. Complexity Analysis (3-4 sentences)

Provide Big-O analysis for the following operations. Include brief justification:

- **Game traversal** (from root to leaf): 
    O(q), q is the number of question + 1.

- **save_tree():** 
    O(n), because each of the n nodes is visited once during BFS traversal and written to file.

- **Hash table put() in worst case:** 
    O(n) when all the keys hash to the same bucket using linear probing, which requires traversal every single node

- **undo_last_edit():** 
    O(1), because it pops the top edit from the stack and restores the previous pointer relationship without traversal.

---

## 3. Testing & Debugging (2-3 sentences)

Describe your testing process and any major bugs you encountered:

- **Testing approach:** What was your strategy beyond running `make test`?
    Using make play to test the program and use undo/redo to see the change in node tree. 

- **Most difficult bug:** What was the hardest bug to find/fix and how did you solve it?
    Game logic and how the text is properly shown to the user which is still not fixed.

---

## 4. Implementation Challenges (2-3 sentences)

Which TODOs were most challenging and why?

1. **Hardest TODO:** 
    TODO 31, because it needs high integration effort with data struction. 

2. **Most time-consuming:** 
    TODO 20-26, because the complexity of implementing a hash table

3. **Most interesting:** 
    TODO 32-33, easy for later debugging. 

---

## 5. Time Spent

Provide approximate hours for each phase:

- Phase 1 (Data Structures): 15 hours
- Phase 2 (Game Logic): 10 hours  
- Phase 3 (Persistence): 10 hours
- Testing/Debugging: 5 hours
- **Total:** 40 hours

---

## 6. Reflection (1-2 sentences)

What was the most valuable lesson from this lab?
    The most valuable lesson from this lab was learning how to design an interactive program using iterative tree traversal and data structures such as queues, hash tables, and stacks to implement undo/redo functionality and frame-based traversal.

---

## Submission Checklist

- [ ] All 32 required TODOs implemented
- [ ] `make test` passes with no failures
- [ ] `make valgrind` shows no memory leaks
- [ ] Game fully functional (play, learn, undo, redo, save, load)
- [ ] Code compiles without warnings (`-Wall -Wextra`)
- [ ] This write-up completed
- [ ] Optional TODO 30 attempted? (Yes/No): No
