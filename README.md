# ProofPea

This project has 2 parts: A library, and a CLI, that uses the library. The library is intended to be the main part of this project, while the CLI just serves as an example.

## ProofPea Library

A library for performing some helpful operations on propositional logic formulae. The goal for this library is to tick the below boxes:

- [x] Check whether or not a propositional logic formula is well-formed or not
- [x] Ensure the parser can handle associativity rules to handle formulae that aren't guaranteed to be well-bracketed
- [x] Construct a truth table for a given propositional logic formula
- [x] Convert propositional logic formulae to Negation Normal Form (NNF)
- [x] Convert propositional logic formulae to Conjunctive Normal Form (CNF), a.k.a. Product of Sums (PoS) form
- [x] Check if a propositional logic formula is valid or not by converting it to CNF first
- [x] Check if a propositional logic formula is in Horn form
- [x] Check if a Horn formula is satisfiable or not
- [x] Check the equivalence of two given formulae

### Future Plans

Since these features will take a long time to implement, they are planned for later, when I feel like it. I finished most of the above features around 25th Dec 2024, and so I will be taking a break from this project for a while.

- [ ] Check the validity of a given sequent
- [ ] Run performance tests on each of the modules, and try and optimize them as much as possible
- [ ] (Not sure if this is even possible, but I think it is) Given a valid sequent, construct a proof for it (maybe not optimal, just a proof)
- [ ] [Optional] Make a SAT solver
- [ ] [Optional] Optimize the proof constructor by using Proof Search, similar to a kind of Prolog

### Building

> [!WARNING]  
> You need to install the graphviz library `libgvc` to compile this from source, as the debug tool that renders ASTs uses this library. Please make sure that graphviz is installed and is included properly while compiling.

I use `make` for compiling this thing:

- `make all` - Compiles all the code, and spits out the library `.so` and `.a` files in the `build` folder
- `make clean` - Deletes logs in the `logs` folder, and the library `.so` and `.a` files
- `make cleaner` - Deletes logs in the `logs` folder, the library `.so` and `.a` files, and any intermediate object files that were compiled

## [WIP] ProofPea CLI

As of now, work on the CLI part of the project has not started, but there is some starter driver code for the library in the `cli` folder. The main goal of the CLI part of the project is to serve as documentation, and an example of the usage for the library.

### Building

> [!WARNING]  
> You need to install the graphviz library `libgvc` to compile this from source, as the debug tool that renders ASTs uses this library. Please make sure that graphviz is installed and is included properly while compiling.

I use `make` for compiling this thing:

- `make all` - Compiles all the code, and spits out an executable in the `build` folder
- `make run` - Compiles all the code, makes an executable in the `build` folder, and runs it
- `make debug` - Compiles all the code, makes an executable in the `build` folder with debug info, and attaches it to `gdb`
- `make clean` - Deletes logs, and the executable
- `make cleaner` - Deletes logs, the executable, and any object files that were compiled
- `make valgrind` - Compiles all the code, makes an executable in the `build` folder, and runs it with [valgrind](https://valgrind.org/)

## Documentation

As of now, this project is not documented, but there is example driver code for using the library in the `cli` folder. I plan to use GitHub wiki pages to document this eventually.

## Syntax

- `0` is constant FALSE
- `1` is constant TRUE
- `~` is the NOT operator
- `+` is the OR operator
- `^` is the AND operator
- `->` is the IMPLIES operator
- Any single letter, capital or lowercase is considered an atom
- Any single letter, capital or lowercase, followed by an arbitrary number of numbers is also considered an atom

e.g. So, `p`, `P` `q`, `Q`, `p1`, `p23`, `Q7` are all considered unique atoms. `pqr`, `pqr2343` are not valid atoms.

You can use brackets to separate parts of your formulae.

This project is solely focused on simple propositional logic, so operators like XOR, NAND, NOR, or predicate logic syntax implementations are not planned.

## Standards

This project uses the C++ 2017 standard with a GCC compiler. Portability is not guaranteed, but I will try my best to use standardized headers and functions.

## References

- Top Down (Recursive Descent) Parser concepts from https://craftinginterpreters.com/parsing-expressions.html
- "Logic in Computer Science" by Michael Huth and Mark Ryan.
- Another really cool resource I found that I haven't used yet, but could help me in the future:
  - https://www.inf.ed.ac.uk/teaching/courses/lp/2012/slides/lpTheory1.pdf
  - https://www.inf.ed.ac.uk/teaching/courses/lp/2012/slides/lpTheory2.pdf
  - https://www.inf.ed.ac.uk/teaching/courses/lp/2012/slides/lpTheory3.pdf
  - https://www.inf.ed.ac.uk/teaching/courses/lp/2012/slides/lpTheory4.pdf
  - https://www.inf.ed.ac.uk/teaching/courses/lp/2012/slides/lpTheory5.pdf
  - https://www.inf.ed.ac.uk/teaching/courses/lp/2012/slides/lpTheory6.pdf
  - https://www.inf.ed.ac.uk/teaching/courses/lp/2012/slides/lpTheory7.pdf
  - https://www.inf.ed.ac.uk/teaching/courses/lp/2012/slides/lpTheory8.pdf
  - https://www.inf.ed.ac.uk/teaching/courses/lp/2012/slides/lpTheory9.pdf
