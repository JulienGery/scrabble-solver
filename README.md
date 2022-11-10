# scrabble-solver
Give a file with word example [here](https://github.com/Thecoolsim/French-Scrabble-ODS8). Expects upperCases, one word per line.
Compile files (example with gcc): 
```
g++ -O3 script.cpp -o script
g++ -O3 scrabbleSolver.cpp -o scrabbleSolver
```  
Give the text file to the script to generate new files:
```
./script myTextFile.txt
```
Then use the scrabbleSolver to find every possibility:
```
./scrabbleSolver "vteafsjxdr?"
```
'?' is the joker.