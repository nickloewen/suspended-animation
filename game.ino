// Array of cells
int cells[8][8];
// Buffer for state of cells while changing other array
int cellsBuffer[8][8];

int humourDad = 0;

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
  
  // make binary row values from cells array
  for (int x=0; x<8; x++) {
    String acc = "";
    for (int y=0; y<8; y++) {
      acc = acc + String(cells[x][y]);
    }
    Serial.println(acc);
    writeBuffer[x] = strtol(acc.c_str(), NULL, 2);
    Serial.println(writeBuffer[x]);
  }

  // update LEDs
  for (int r=1; r<9; r++) {
      Write_Max7219(r,writeBuffer[r-1]);
  }
  Serial.println("Tick \n");
  delay(500);
}

void generation() {
  Serial.println("next generation \n");
  // copy cells to buffer
  for (int x=0; x<8; x++) {
    for (int y=0; y<8; y++) {
      cellsBuffer[x][y] = cells[x][y];
    }
  } 

  Serial.println("cells copied \n"); 
  
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
  Serial.println("generation complete \n");
}
