// Array of cells
int cells[8][8];
// Buffer for state of cells while changing other array
int cellsBuffer[8][8];
// count how many generations have elapsed
int generations = 1;

void setup() {
  // LED array setup
  pinMode(Max7219_pinCLK,OUTPUT);
  pinMode(Max7219_pinCS,OUTPUT);
  pinMode(Max7219_pinDIN,OUTPUT);
  delay(50);
  Init_MAX7219();
  
  // LIFE setup
  
  // initialize cells
  
  randomSeed(analogRead(0));
  
  for (int x=0; x<8; x++) {
    for (int y=0; y<8; y++) {
      float state = random(100);
      state > 35 ? state = 0 : state = 1; // probability of cell being alive at start
      cells[x][y] = state;
    }
  }
  
  // serial
  Serial.begin(9600);
 }
 
void loop() {
  int writeBuffer[8];
  
  generation();
  
  // check if current generation is different from previous one
  
  int allCellsTheSame = 1; // 1 = true, 0 = false  
  for (int x=0; x<8; x++) {
    for (int y=0; y<8; y++) {
      if (cells[x][y] != cellsBuffer[x][y]) {
        allCellsTheSame = 0;
        break;
      }
    }
    if (allCellsTheSame == 0) { break; }
  }
  
  if (allCellsTheSame == 1) {
    Serial.println("all cells the same");
    exit(0);
  }
  
  // make binary row values from cells array
  for (int x=0; x<8; x++) {
    String acc = "";
    for (int y=0; y<8; y++) {
      acc = acc + String(cells[x][y]);
    }
    Serial.println(acc);
    writeBuffer[x] = strtol(acc.c_str(), NULL, 2);
  }

  // update LEDs
  for (int r=1; r<9; r++) {
      Write_Max7219(r,writeBuffer[r-1]);
  }
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
  Serial.print("generation: ");
  Serial.println(generations);
}
