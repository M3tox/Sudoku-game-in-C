#include <stdio.h>
#include <stdlib.h>

#define TABLESIZE 9

#define EASY 43
#define NORMAL 30
#define HARD 26

typedef unsigned char u8;

enum {
	false,
	true,
	// numbers represent how many boxes will be hidden to the user
	/// calculation done, so we can simply enter how many boxes we want to display, which is easier
	easy = (TABLESIZE*TABLESIZE)-  EASY,	// 38 hidden
	normal = (TABLESIZE*TABLESIZE)-NORMAL,	// 51 hidden
	hard = (TABLESIZE*TABLESIZE) - HARD		// 55 hidden
};


void BuildDisplayedContent(char**, char**, u8);
u8 ValidateNumber(char**, char, u8, u8);
void shuffle(char*);
u8 FillOtherBlocks(char**, u8, u8);
void GenerateNumbers(char**);
void DrawTable(char**);
u8 CheckCoords(char**,char*);


void main() {

	// Allocate memory for whole content, which holds all numbers.
	// Mind the +1 because last row will hold the random number array
	// which will be used to fill the cells
	char TableContent[TABLESIZE+1][TABLESIZE];
	// allocate memory for a second table. This is the one which will be displayed to the user
	char ContentDisplayed[TABLESIZE][TABLESIZE];
	
	char UserInput[] = "AA";
	u8 DifficultySet;
	u8 InputLoop = false;
	// generate full sudoku table content
	GenerateNumbers(TableContent);

	// game loop
	while (true) {
		// Difficulty Selection
		do {

				printf("Please select Difficulty for new game or quit:\n\n- (e)asy\n- (n)ormal\n- (h)ard\n\n- e(x)it\n\n");
				scanf_s("%s", UserInput, 2);

				switch (UserInput[0]) {
				case 'e':
					DifficultySet = easy;
					InputLoop = false;
					break;

				case 'n':
					DifficultySet = normal;
					InputLoop = false;
					break;

				case 'h':
					DifficultySet = hard;
					InputLoop = false;
					break;

				case 'x':
					return 0;
					break;
				default:
					printf("Invalid Input!\n");
					InputLoop++;
					break;
				}


		} while (InputLoop);


		// create a second table with the difficulty set.
		BuildDisplayedContent(TableContent, ContentDisplayed, DifficultySet);

		// draw table content on screen
		DrawTable(ContentDisplayed);
		InputLoop = true;

		// loop until all missing cells have been filled by the user
		do {

			if (DifficultySet) {
				printf("Please select the cell you want to modify by typing the collumn name and row number, e.g. 'B3'.\n");
				scanf_s("%s", UserInput, 3);


				if (CheckCoords(ContentDisplayed, UserInput)) {

					u8 CoordBuffer[2] = { UserInput[1] - '1',UserInput[0] - 'A' };

					printf("You have selected %s. Which value would you like to insert? Type any number from 1 to 9.\n", UserInput);
					scanf_s("%s", UserInput, 2);

					if (TableContent[CoordBuffer[0]][CoordBuffer[1]] == UserInput[0]) {

						ContentDisplayed[CoordBuffer[0]][CoordBuffer[1]] = UserInput[0];
						printf("Number %c valid and inserted.\n", UserInput[0]);
						DifficultySet--;
						DrawTable(ContentDisplayed);

					}
					else {
						printf("Incorrect number!\n");
					}

				}
				else {
					printf("Given coordinates invalid!\n");
				}
			}
			else
				InputLoop = false;


		} while (InputLoop);

		InputLoop = true;
		printf("You have finished the game!\n");
	}

}


u8 CheckCoords(char displayedContent[][TABLESIZE], char* UserInput) {

	// check if user input is within the table bounds
	if ((UserInput[0] >= 'A' && UserInput[0] < 'A' + TABLESIZE) &&
		(UserInput[1] >= '1' && UserInput[1] < '1' + TABLESIZE)) {

		// check if given cell is empty
		if (displayedContent[UserInput[1]-'1'][UserInput[0] - 'A'] == ' ')
			return true;
	}
		
	return false;
}

void BuildDisplayedContent(char fullConent[][TABLESIZE], char displayedContent[][TABLESIZE], u8 Difficulty) {

	u8 randNum;

	memcpy(displayedContent, fullConent, TABLESIZE*TABLESIZE);

	for (u8 hide = 0; hide < Difficulty; hide++) {

		do {
			randNum = rand() % (TABLESIZE*TABLESIZE);
		} while (displayedContent[(u8)(randNum / TABLESIZE)][randNum%TABLESIZE] == ' ');
		displayedContent[(u8)(randNum / TABLESIZE)][randNum%TABLESIZE] = ' ';
	}
}


u8 ValidateNumber(char content[][TABLESIZE], char NewNum, u8 x, u8 y) {

	// Check horizontal and vertical if there are any matches
	/// start with horizontal:
	for (u8 x2 = 0; x2 < TABLESIZE; x2++) {
		// compare value from other same row to current position
		if (content[y][x2] == NewNum && x2 != x) {
			return false;
		}
	}

	/// check verticals
	for (u8 y2 = 0; y2 < TABLESIZE; y2++) {
		// compare value from other same row to current position
		if (content[y2][x] == NewNum && y2 != y) {
			return false;
		}
	}

	/// check if variable was already used in sub square
	u8 y_offset = (u8)(y / 3) * 3;
	u8 x_offset = (u8)(x / 3) * 3;

	for (u8 y2 = y_offset; y2 < y_offset + 3; y2++) {
		for (u8 x2 = x_offset; x2 < x_offset + 3; x2++) {
			// compare new number to insert with iterated content
			if (NewNum == content[y2][x2])
				return false;
		}
	}

	return true;
}

// the shuffle function will just use the given array to shuffle the numbers randomly around within the array.
void shuffle(char* NumArray)
{

	for (u8 i = 0; i < TABLESIZE; i++) {

		u8 swap_pos = (rand() % TABLESIZE);
		u8 swap_val = NumArray[swap_pos];
		NumArray[swap_pos] = NumArray[i];
		NumArray[i] = swap_val;
	}

}

u8 FillOtherBlocks(char content[][TABLESIZE], u8 y, u8 x) {

	u8 BlockSize = TABLESIZE / 3;

	// jump to next row when previous row has been filled, so if x axis 9/9, then reset X
	if (x >= TABLESIZE && y < TABLESIZE - 1)
	{
		y++;
		x = 0;
		shuffle(content[TABLESIZE]);
	}

	// return true when table has been completely filled. y-axis = 9/9, x-axis 9/9
	if (y >= TABLESIZE && x >= TABLESIZE)
		return true;

	// following if statements will make sure to skip the already put cell boxes from step 1
	if (y < BlockSize)
	{
		// if here, we are just in the upper region, only working on sub square 2 and 3
		if (x < BlockSize)
			x = BlockSize;
	}
	else if (y < TABLESIZE - BlockSize)
	{
		// if here, we are in the second section, assigning values on sub square 1 and 3
		if (x == (u8)(y / BlockSize)*BlockSize)
			x += BlockSize;
	}
	else
	{
		// if here, we are in the lowest region, putting values in subsquare 7 and 8
		if (x == TABLESIZE - BlockSize)
		{
			y++;
			x = 0;
			if (y >= TABLESIZE)
				return true;
		}
	}


	// iterate though random array and try to put the values
	for (u8 num = 0; num < TABLESIZE; num++)
	{
		if (ValidateNumber(content, content[TABLESIZE][num], x, y))
		{
			content[y][x] = content[TABLESIZE][num];
			if (FillOtherBlocks(content, y, x + 1))
				return true;

			content[y][x] = '0';
		}
	}
	return false;
}



void GenerateNumbers(char content[][TABLESIZE]) {
	// initiliaze random number with variable time seed
	time_t t;
	srand((unsigned)time(&t));

	// step 1: zero whole array to 0, to make sure to validate with proper values and not leftovers
	// Also: It flushes old table data, in case the user wants to generate a new table in a new game
	memset(content, '0', TABLESIZE*TABLESIZE);
	// step 2: allocate and hold a const array of numbers 1 to 9 in character format
	// this array will just be used to copy it once into the last row of the contentTable array.
	// I just want it there because the pointer will be passed to the functions anway.
	const char constNumbers[9] = { '1','2', '3', '4', '5', '6', '7', '8', '9' };
	memcpy(content[TABLESIZE], constNumbers, TABLESIZE);

	// Step 3: Shuffle the array we have just copied and copy its content into
	// 3 sub squares. The sub squares will be called blocks:
	// Block 2, 5 and 9 can be completely independent from the rest
	// 1 0 0
	// 0 1 0
	// 0 0 1
	for (u8 block = 0; block < TABLESIZE / 3; block++) {

		shuffle(content[TABLESIZE]);

		for (u8 y = 0; y < TABLESIZE / 3; y++) {
			for (u8 x = 0; x < TABLESIZE / 3; x++) {
				content[block * 3 + y][block * 3 + x] = content[TABLESIZE][x + y * 3];
			}
		}
	}

	// start recursive function to fill block 2,3,4,6,7 and 8:
	// 0 1 1
	// 1 0 1
	// 1 1 0
	FillOtherBlocks(content, 0, TABLESIZE / 3);

	return;
}

// this function is just used to display the sudoko table into the console properly.
void DrawTable(char content[][TABLESIZE]) {
	// draw table header
	printf("   |");
	for (u8 i = 0; i < TABLESIZE; i += 3) {
		printf("  %c     %c     %c  |", 'A' + i, 'B' + i, 'C' + i);

	}
	printf("\n---+");
	for (u8 i = 0; i < TABLESIZE / 3; i++) {
		printf("-----------------+");
	}
	printf("\n");
	for (u8 x = 0; x < TABLESIZE; x++) {
		if (x == 3 || x == 6) {
			printf("---+");
			for (u8 i = 0; i < TABLESIZE / 3; i++)
				printf("-----------------+");
			printf("\n");
		}
		// print row number
		printf(" %d |", x + 1);
		for (u8 y = 0; y < TABLESIZE; y += 3) {
			// print actual value in each row
			printf("  %c     %c     %c  |", content[x][y], content[x][y + 1], content[x][y + 2]);
		}
		printf("\n");
	}
	// draw table footer
	printf("---+");
	for (u8 i = 0; i < TABLESIZE / 3; i++) {
		printf("-----------------+");
	}

	printf("\n\n");
	return;
}
