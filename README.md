# ECE 312 - Lab 5 Write-Up

**Name:** Yifan Shi
**EID:** ys27523
**Date:** November 8th 2025

---

## 1. Design Decisions (3-4 sentences)

Briefly describe your key design choices and rationale:

- **Data structure implementations:** How did you approach the stack/queue/hash table? Any specific design trade-offs?

    There are both push/pop for both framestack and editstack so that the user and undo/redo tracking.
    The queue is used in BFS traversal for saving and loading the tree with enqueue and dequeue access to user.
    The array-based stack is simpler to implement compare to linked list, but ths trade off is occasional reallocations when the capacity equal the array size. 

- **Game loop strategy:** How did you structure the iterative traversal? What was your approach to the learning phase?

    


- **Memory management:** What strategy did you use to avoid leaks? Any particular challenges?


---

## 2. Complexity Analysis (3-4 sentences)

Provide Big-O analysis for the following operations. Include brief justification:

- **Game traversal** (from root to leaf): 


- **save_tree():** 


- **Hash table put() in worst case:** 


- **undo_last_edit():** 


---

## 3. Testing & Debugging (2-3 sentences)

Describe your testing process and any major bugs you encountered:

- **Testing approach:** What was your strategy beyond running `make test`?


- **Most difficult bug:** What was the hardest bug to find/fix and how did you solve it?


---

## 4. Implementation Challenges (2-3 sentences)

Which TODOs were most challenging and why?

1. **Hardest TODO:** 


2. **Most time-consuming:** 


3. **Most interesting:** 


---

## 5. Time Spent

Provide approximate hours for each phase:

- Phase 1 (Data Structures): 15 hours
- Phase 2 (Game Logic): 10 hours  
- Phase 3 (Persistence): 10 hours
- Testing/Debugging: _____ hours
- **Total:** _____ hours

---

## 6. Reflection (1-2 sentences)

What was the most valuable lesson from this lab?


---

## Submission Checklist

- [ ] All 32 required TODOs implemented
- [ ] `make test` passes with no failures
- [ ] `make valgrind` shows no memory leaks
- [ ] Game fully functional (play, learn, undo, redo, save, load)
- [ ] Code compiles without warnings (`-Wall -Wextra`)
- [ ] This write-up completed
- [ ] Optional TODO 30 attempted? (Yes/No): No
