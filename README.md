# [Advent of Code 2020](https://adventofcode.com/2020)
Advent of Code is an Advent calendar of small programming puzzles for a variety of skill sets and skill levels that can be solved in any programming language.

## Post Mortem
Overall I'm proud of completing the calendar as well as learning more about input parsing, standard library, and template type traits for conditional compilation.
Having a deadline at work and family events made me a bit behind, but I was able to complete most of them within a day. Some stand outs:
- [Day 5](Advent%202020/problems/days/day_5.cpp) Even though it was fairly obvious, using bit masks are always satisfying.
- [Day 7](Advent%202020/problems/days/day_7.cpp) Thankfully I solved the first problem in a way that made the second problem fairly intuitive and with few edits.
- [Day 8](Advent%202020/problems/days/day_8.cpp) This was the first one that tripped me up. I misread the 2nd problem and spent too much time trying to solve the wrong problem. After realizing that, I was able to figure out a much simpler solution. I enjoyed being able to utilize my familiarity with assembly and function pointers.
- [Day 11](Advent%202020/problems/days/day_11.cpp) The first of many problems revolving around a coordinate system with a state that updates all at once. Making a solid solution helped with future, similar problems.
- [Day 13](Advent%202020/problems/days/day_13.cpp) My most regretable day, I had to look up the algorithm for the [Chinese Remainder Theorem](https://en.wikipedia.org/wiki/Chinese_remainder_theorem) after spending all day trying to figure it out on my own. Also the first problem where I was unknowingly overflowing my integers.
- [Day 15](Advent%202020/problems/days/day_15.cpp) One of the shortest and concise solutions.
- [Day 16](Advent%202020/problems/days/day_16.cpp) As a direct comparisson, this one was fairly hard to follow. Spending more time planning out how to solve problem 2 and maybe reworking the solution may have produced a better result.
- [Day 18](Advent%202020/problems/days/day_18.cpp) This problem was fairly similar to day 8 with utilizing stacks and saving off the current state. The solutions for problem 1 and 2 seem similar enough where I feel there could be a way to combine them.
- [Day 19](Advent%202020/problems/days/day_19.cpp) Too much time was wasted trying to solve the generic problem instead of the specific test case. The middle of the logic is heading towards spaghetti code, so more planning should've been done to make it easier to follow. 
- [Day 20](Advent%202020/problems/days/day_20.cpp) This was the most challenging problem by far. I'm happy with the final result, but the process was long. Partly due to overengineering with the templatizing and enable_ifs, and also from a misunderstanding. Learning regex also took time but could be helpful in the future. I'm most proud of thinking of creating and storing UIDs for the test cases instead of needing to do string compares and recomputing them from operations.
- [Day 21](Advent%202020/problems/days/day_21.cpp) Bad practice of passing around the sets into the instances. Having global variables or different structure would be better.
- [Day 23](Advent%202020/problems/days/day_23.cpp) I hung onto my linked list implementation for too long during the 2nd problem. Searching the list was too slow and I should've switched to a different data structure earlier. I'm happy with settling on an unordered map for it's searching speed and storing the next cup's iterator for walking the order. I think creating a class instead of using an std::pair would've made the code cleaner however.
- [Day 24](Advent%202020/problems/days/day_24.cpp) Coming up with a way to convert the hexagonal grid to a normal 2D grid was vital and I thankfully realized and came up with an algorithm for it fairly early.


## Visual Studio Tool
To reduce needing to create new files, add includes, and create the input file, I created a simple tool for Visual Studio to automate this. This tool:
- Takes the day for the new problems
- Creates the source and header file for them
- Adds these files to the Visual Studio Project and Filter
- Add the newly made header file to the problems include file
- Update the main file to instantiate and solve the new problems
- Create and open the input file for this problem
