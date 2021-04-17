 /* WordSearch.java
   CSC 226 - Summer 2020
   Assignment 6 - Word Search Problem Template
   
   You should be able to compile your program with the command
   
	javac WordSearch.java
	
   To conveniently test the algorithm, create a text file
   containing a word and puzzle (in the format described below) 
   and run the program with
   
	java WordSearch file.txt
	
   where file.txt is replaced by the name of the text file.
   
   Note, I didn't need any of the algs4.jar code. You may include it
   if you need to.
   
   The input consists of a graph (as an adjacency matrix) in the following format:
   
    <dimnsion of puzzle>
	<word string>
	<character grid row 1>
	...
	<character grid row n>

   R. Little - 08/7/2020
*/

import java.util.Scanner;
import java.io.File;
import java.lang.Math;

//Do not change the name of the WordSearch class.
public class WordSearch {
	// List of global variables. You may to this list.
	public int startRow; // Starting row of word in puzzle
	public int startCol; // Starting column of word in puzzle
	public int endRow; 	 // Ending row of word in puzzle
	public int endCol;   // Ending column of word in puzzle
	public int R; //size of ascii library
	public int M; //length of our pattern word
	public long Q; //a large prime number
	public char[] pat; //our given pattern
	public long h; //our pattern's hashvalue
	public long hReverse; //reverse hashvalue
	public long Rm;

	/* WordSearch(word)
		Use the WordSearch construcutor to do any preprocessing
		of the search word that may be needed. Examples, the dfa[][]
		for KMP, the right[] for Boyer-Moore, the patHash for Rabin-
		Karp.
	*/
    public WordSearch(char[] word) {
		
		/* ... Your code here ... */
		this.h = 0;
		this.hReverse = 0;
		this.M = word.length;
		this.R = 256;
		this.Q = 14737;
		this.pat = word;
		this.Rm = (long)(Q-(((Math.pow(R, M-1)%Q)+Q)%Q));
		
		for(int i= 0, j = M-1;i < M; i++, j--){
			h = (h*R+word[i])%Q; 
			hReverse = (hReverse*R+word[j])%Q;
		}
		
    } 

	
 
	/* search(puzzle)
		Once you have preprocessed the word you need to search the
		puzzle for the word. That happens here. You will assign the
		given global variables here once you find the word and return
		the boolean value "true". If you can't find the word, return 
		"false"
	*/
    public boolean search(char[][] puzzle) {
		
		/* ... Your code here ... */
		
		
		//Horizontal Search
		for(int i = 0; i < puzzle.length; i++){
			long hRoll =0;
			for(int k = 0; k < M;k++){
				hRoll = (hRoll*R+puzzle[i][k])%Q;
			}
			if (hRoll == h){
				startRow = i;
				startCol = 0;
				endRow = i;
				endCol = M-1;
				return true;
			} else if ((hRoll == hReverse)){
				startRow = i;
				startCol = M-1;
				endRow = i;
				endCol = 0;
				return true;
			}
			
			//let's roll
			for (int j = M;j<puzzle.length;j++){
				
				hRoll = (hRoll + (puzzle[i][j-M]*Rm)%Q)%Q;
				hRoll = (hRoll * R + puzzle[i][j]) %Q;
					
				if (hRoll == h){
					startRow = i;
					startCol = j-M+1;
					endRow = i;
					endCol = j;
					return true;
				} else if ((hRoll == hReverse)){
					startRow = i;
					startCol = j;
					endRow = i;
					endCol = j-M+1;
					return true;
				}
			}
		}
		
		//vertical search
		for(int j = 0; j < puzzle.length; j++){
			long hRoll =0;
			for(int k = 0; k < M;k++){
				hRoll = (hRoll*R+puzzle[k][j])%Q;
			}
			if (hRoll == h){
				startRow = 0; 
				startCol = j;
				endRow = M-1;
				endCol = j;
				return true;
			} else if ((hRoll == hReverse)){
				startRow = M-1;
				startCol = j;
				endRow = 0;
				endCol = j;
				return true;
			}
			
			//let's roll
			for (int i = M;i<puzzle.length;i++){
				
				hRoll = (hRoll + (puzzle[i-M][j]*Rm)%Q)%Q;
				hRoll = (hRoll * R + puzzle[i][j]) %Q;
				if( hRoll == h){
					startRow = i-M+1;
					startCol = j;
					endRow = i;
					endCol = j;
					return true;
				} else if ((hRoll == hReverse)){
					startRow = i;
					startCol = j;
					endRow = i-M+1;
					endCol = j;
					return true;
				}	
			}
		}
		
		//northwest to southeast diagonal search, lower triangle.
		for(int i = 0; i < puzzle.length-M+1; i++){
			long hRoll =0;
			for(int k = 0; k < M;k++){
				hRoll = (hRoll*R+puzzle[i+k][k])%Q;
			}
			if( hRoll == h){
				startRow = i;
				startCol = 0;
				endRow = i+M-1;
				endCol = M-1;
				return true;
			} else if ((hRoll == hReverse)){
				startRow = i+M-1;
				startCol = M-1;
				endRow = i;
				endCol = 0;
				return true;
			}
			
			//let's roll
			for (int j = 0;j<puzzle.length-M-i;j++){
				
				hRoll = (hRoll + (puzzle[i+j][j]*Rm)%Q)%Q;
				hRoll = (hRoll * R + puzzle[i+j+M][j+M]) %Q;
				if( hRoll == h){
					startRow = i+j+1;
					startCol = j+1;
					endRow = i+j+M;
					endCol = j+M;
					return true;
				} else if ((hRoll == hReverse)){
					startRow = i+j+M;
					startCol = j+M;
					endRow = i+j+1;
					endCol = j+1;
					return true;
				}
			}
		}
		
		//Northwest to southest diagonal search, upper triangle.
		for(int j = 1; j < puzzle.length-M+1; j++){
			long hRoll =0;
			for(int k = 0; k < M;k++){
				hRoll = (hRoll*R+puzzle[k][k+j])%Q;
			}
			if( hRoll == h){
				startRow = 0;
				startCol = j;
				endRow = M-1;
				endCol = j+M-1;
				return true;
			} else if ((hRoll == hReverse)){
				startRow = M-1;
				startCol = j+M-1;
				endRow = 0;
				endCol = j;
				return true;
			}
			
			//let's roll
			for (int i = 0;i<puzzle.length-M-j;i++){
				
				hRoll = (hRoll + (puzzle[i][i+j]*Rm)%Q)%Q;
				hRoll = (hRoll * R + puzzle[i+M][i+j+M]) %Q;
				if( hRoll == h){
					startRow = i+1;
					startCol = i+j+1;
					endRow = i+M;
					endCol = i+j+M;
					return true;
				} else if ((hRoll == hReverse)){
					startRow = i+M;
					startCol = i+j+M;
					endRow = i+1;
					endCol = i+j+1;
					return true;
				}
			}
		}
		
		//Southwest to northeast diagonal search, upper triangle.
		for(int i = puzzle.length-1; i > M-2; i--){
			long hRoll =0;
			for(int k = 0; k < M;k++){
				hRoll = (hRoll*R+puzzle[i-k][k])%Q;	
			}
			if( hRoll == h){
				startRow = i;
				startCol = 0;
				endRow = i-M+1;
				endCol = M-1;
				return true;
			} else if ((hRoll == hReverse)){
				startRow = i-M+1;
				startCol = M-1;
				endRow = i;
				endCol = 0;
				return true;
			}
			
			//let's roll
			for (int j = 0;j<i-M+1;j++){
				
				hRoll = (hRoll + (puzzle[i-j][j]*Rm)%Q)%Q;
				hRoll = (hRoll * R + puzzle[i-(j+M)][j+M]) %Q;
				if( hRoll == h){
					startRow = i-1;
					startCol = j+1;
					endRow = i-M;
					endCol = j+M;
					return true;
				} else if ((hRoll == hReverse)){
					startRow = i-M;
					startCol = j+M;
					endRow = i-1;
					endCol = j+1;
					return true;
				}
			}
		}
		
		//Southwest to northeast, lower triangle.
		for(int j=1; j < puzzle.length-M; j++){
			long hRoll =0;
			for(int k = puzzle.length-1; k > puzzle.length-M-1;k--){
				hRoll = (hRoll*R+puzzle[k][puzzle.length-1-k+j])%Q;
			}
			if( hRoll == h){
				startRow = 0;
				startCol = j;
				endRow = M-1;
				endCol = j+M-1;
				return true;
			} else if ((hRoll == hReverse)){
				startRow = M-1;
				startCol = j+M-1;
				endRow = 0;
				endCol = j;
				return true;
			}
			
			//let's roll
			for (int i = puzzle.length-1;i>puzzle.length-1-M+j;i--){
				hRoll = (hRoll + (puzzle[i][puzzle.length-i+j-1]*Rm)%Q)%Q;
				hRoll = (hRoll * R + puzzle[i-M][puzzle.length-i+j+M-1]) %Q;
				if( hRoll == h){
					startRow = i+1;
					startCol = i+j+1;
					endRow = i+M;
					endCol = i+j+M;
					return true;
				} else if ((hRoll == hReverse)){
					startRow = i+M;
					startCol = i+j+M;
					endRow = i+M+1;
					endCol = i+j+1;
					return true;
				}
			}
		}
		return false;
    }


	/* main()
	   Contains code to test the WordSearch program. You may modify the
	   testing code if needed, but nothing in this function will be considered
	   during marking, and the testing process used for marking will not
	   execute any of the code below. 
	*/
    public static void main(String[] args) {
		Scanner s;
		if (args.length > 0){
			try{
				s = new Scanner(new File(args[0]));
			} catch(java.io.FileNotFoundException e){
				System.out.printf("Unable to open %s\n",args[0]);
				return;
			}
			System.out.printf("Reading input values from %s.\n",args[0]);
		}else{
			s = new Scanner(System.in);
			System.out.printf("Reading input values from stdin.\n");
		}
		
		int n = s.nextInt(); 			// dimension of the puzzle
		String wordAsString = s.next();
		char[] word = wordAsString.toCharArray(); // search word
		char[][] puzzle = new char[n][n];		  // the puzzle
		
		for (int i = 0; i < n; i++){
			String line = s.next();
			for (int j = 0; j < n; j++)
				puzzle[i][j] = line.charAt(j);
		}
		
		WordSearch searcher = new WordSearch(word); // Preprocess word
        boolean result = searcher.search(puzzle);	// Search for word in puzzle
		
		
		// Output the word, the puzzle and the solution
		System.out.println("Word: " + wordAsString);
		System.out.printf("\nPuzzle:    \n");
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++)
				 System.out.print(puzzle[i][j]);
			System.out.println();
		}
		
		if (!result)
			System.out.printf("\nSolution: Search word not found");
		else {
			int x1 = searcher.startRow;
			int y1 = searcher.startCol;
			int x2 = searcher.endRow;
			int y2 = searcher.endCol;
		
			System.out.printf("\nSolution: Search word starts at coordinate ");
			System.out.print("("+x1+","+y1+")");
			System.out.print(" and ends at coordinate ");
			System.out.print("("+x2+","+y2+")");
		}
    }
	
}


