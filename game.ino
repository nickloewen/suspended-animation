// Array of cells
int cells[8][8];
// Buffer for state of cells while changing other array
int cellsBuffer[8][8];

int generations; // number of generations elapsed since start of game
int maxGenerations = 100; // maximum number of generations before game is restarted

int serialDebugging = 0; // 0 = off; 1 = on 

void setup() {
  // LED array setup
  pinMode(Max7219_pinCLK,OUTPUT);
  pinMode(Max7219_pinCS,OUTPUT);
  pinMode(Max7219_pinDIN,OUTPUT);
  delay(50);
  Init_MAX7219();
  
  // LIFE setup  
  startGame();
  
  // serial
  if (serialDebugging) { Serial.begin(9600); }
}
 
void loop() {
  int writeBuffer[8];
  
  // update game state
  generation();
  
  // check if current generation is different from previous one
  int repeated = repeatedScreen();
  
  if ((repeated == 1) || (generations > maxGenerations)) {
    delay(100);
    startGame();
  }
  
  // convert cells array to binary values that LED matrix can read
  cellsToBinary(writeBuffer);

  // update LEDs
  for (int r=1; r<9; r++) { Write_Max7219(r, writeBuffer[r-1]); }
  
  // delay between generations
  delay(500);
}

void generation() {
  // copy cells to buffer
  for (int x=0; x<8; x++) {
    for (int y=0; y<8; y++) {
      cellsBuffer[x][y] = cells[x][y];
    }
  } 

  for (int x=0; x<8; x++) {
    for (int y=0; y<8; y++) { 
      int neighbours = 0; // count neighbours
      for (int xx=x-1; xx<=x+1; xx++) {
        for (int yy=y-1; yy<=y+1; yy++) {          
          if (((xx >=0) && (xx<8)) && ( (yy >=0) && (yy < 8) )) {
            if ((xx!=x) || (yy!=y)) {
              if (cellsBuffer[xx][yy] == 1) {
                neighbours++; 
              }
            }
          }
        } // end of yy
      } // end of xx
      if (cellsBuffer[x][y] == 1) { // alive
        if (neighbours < 2 || neighbours > 3) {
          cells[x][y] = 0;
        }
      } else { // dead
        if (neighbours == 3) {
          cells[x][y] = 1;
        }
      } // end of if
    }
  }
  generations++;
  if (serialDebugging) {
    Serial.print("generation: ");
    Serial.println(generations);
  }
}

void startGame() {
  // setup
  generations = 1;
  randomSeed(analogRead(0));

  if (serialDebugging) { Serial.println("NEW GAME"); }

  flashScreen();
  
  // initialize each cell
  for (int x=0; x<8; x++) {
    for (int y=0; y<8; y++) {
      float state = random(100);
      state > 35 ? state = 0 : state = 1; // probability of cell being alive at start
      cells[x][y] = state;
    }
  }
}

void flashScreen() {
  int off = B00000000;
  int on = B11111111;
 
  for (int r=1; r<9; r++) { Write_Max7219(r, off); }
  delay(50);
  for (int r=1; r<9; r++) { Write_Max7219(r, on); }
  delay(50);
  for (int r=1; r<9; r++) { Write_Max7219(r, off); }
  delay(50);
}

int repeatedScreen() {
  for (int x=0; x<8; x++) {
    for (int y=0; y<8; y++) {
      if (cells[x][y] != cellsBuffer[x][y]) {
        return 0;
      }
    }
  }
  return 1;
}

void cellsToBinary(int writeBuffer[]) {
  // make binary row values from cells array
  for (int x=0; x<8; x++) {
    String acc = "";
    for (int y=0; y<8; y++) {
      acc = acc + String(cells[x][y]);
    }
    if (serialDebugging) { Serial.println(acc); }
    writeBuffer[x] = strtol(acc.c_str(), NULL, 2);
  }
}
