const unsigned int canBeEaten = 0x07E7E7E0;
const unsigned int up5down3 = 0x07070707;
const unsigned int up3down5 = 0xE0E0E0E0;



#define isInBounds(i,j) (0<=i && i<=7 && 0<=j && j<=7)

#define getIndex(i,j) (((7-i))*4 + (((7-j))>>1))



typedef struct {
	unsigned int up5or3;
	unsigned int up4;
	unsigned int down5or3;
	unsigned int down4;
} ValidMoves;


typedef struct {
	unsigned int whites;
	unsigned int blacks;
	unsigned int kings;
	
	ValidMoves validMoves;
	unsigned char turn : 1;
	unsigned char lastTurnCapture : 1;
	unsigned char lastCaptureIndex : 5;
	unsigned int canCapture;
} Board;


typedef struct {
	unsigned int moves;
	unsigned int count;
} OnePieceValidMoves;



void updateValidMoves(Board* b) {
	unsigned int curr, opp;
	unsigned int canGoUp, canGoDown;
	unsigned int emptyTiles = ~(b->whites | b->blacks);
	
	if (b->turn == 0) {
		curr = b->whites;
		opp = b->blacks & canBeEaten;
		canGoUp = curr;
		canGoDown = curr & b->kings;
	}
	else {
		curr = b->blacks;
		opp = b->whites & canBeEaten;
		canGoDown = curr;
		canGoUp = curr & b->kings;
	}
	
	
	b->validMoves.up5or3 = (up5down3 & canGoUp & (opp>>5) & (emptyTiles>>9)) |
						   (up3down5 & canGoUp & (opp>>3) & (emptyTiles>>7));
	
	b->validMoves.up4 = (0x0F0F0F0F & canGoUp & (opp>>4) & (emptyTiles>>7)) | 
						(0xF0F0F0F0 & canGoUp & (opp>>4) & (emptyTiles>>9));
	
	b->validMoves.down5or3 = (up5down3 & canGoDown & (opp<<3) & (emptyTiles<<7)) | 
							 (up3down5 & canGoDown & (opp<<5) & (emptyTiles<<9));
	
	b->validMoves.down4 = (0x0F0F0F0F & canGoDown & (opp<<4) & (emptyTiles<<9)) | 
						  (0xF0F0F0F0 & canGoDown & (opp<<4) & (emptyTiles<<7));
	
	b->canCapture = b->validMoves.up5or3 | b->validMoves.up4 | b->validMoves.down5or3 | b->validMoves.down4;
	
	if (!b->canCapture) {
	
		b->validMoves.up5or3 = (up5down3 & canGoUp & (emptyTiles>>5)) |
						  	   (up3down5 & canGoUp & (emptyTiles>>3));
	
		b->validMoves.up4 = canGoUp & (emptyTiles>>4);
	
		b->validMoves.down5or3 = (up5down3 & canGoDown & (emptyTiles<<3)) | 
							 	 (up3down5 & canGoDown & (emptyTiles<<5));
	
		b->validMoves.down4 = canGoDown & (emptyTiles<<4);
	}
}


void setup(Board* b) {
	b->whites = 0x00000FFF;
	b->blacks = 0xFFF00000;
	b->kings = 0;
	
	b->turn = 0;
	b->lastTurnCapture = 0;
	b->lastCaptureIndex = 0;
	b->canCapture = 0;
	
	updateValidMoves(b);
}



OnePieceValidMoves getValidMoves(Board b, int pLoc) {
	unsigned int validMoves = 0;
	unsigned char count = 0;
	unsigned int p = 1U<<pLoc;
	
	if (p & b.canCapture) {
		if (p & 0x0F0F0F0F) {
			if (p & b.validMoves.up5or3) {
				validMoves |= p<<9;
				count++;
			}
			
			if (p & b.validMoves.up4) {
				validMoves |= p<<7;
				count++;
			}
			
			if (p & b.validMoves.down5or3) {
				validMoves |= p>>7;
				count++;
			}
			
			if (p & b.validMoves.down4) {
				validMoves |= p>>9;
				count++;
			}
		}
		else {
			if (p & b.validMoves.up5or3) {
				validMoves |= p<<7;
				count++;
			}
			
			if (p & b.validMoves.up4) {
				validMoves |= p<<9;
				count++;
			}
			
			if (p & b.validMoves.down5or3) {
				validMoves |= p>>9;
				count++;
			}
			
			if (p & b.validMoves.down4) {
				validMoves |= p>>7;
				count++;
			}
		}
	}
	else {
		if (p & b.validMoves.up5or3) {
			count++;
			if (p & up5down3)
				validMoves |= p<<5;
			else
				validMoves |= p<<3;
		}
		
		if (p & b.validMoves.up4) {
			validMoves |= p<<4;
			count++;
		}
		
		if (p & b.validMoves.down5or3) {
			if (p & up5down3)
				validMoves |= p>>3;
			else
				validMoves |= p>>5;
			count++;
		}
		
		if (p & b.validMoves.down4) {
			validMoves |= p>>4;
			count++;
		}
	}
	
	OnePieceValidMoves vm = {validMoves, count};
	return vm;
}



void movePiece(Board* b, int i1, int i2) {
	
	//printf("%d %d\n", i1, i2);
	
	if (i1==i2) {
		b->turn++;
		b->lastTurnCapture = 0;
		return;
	}

	unsigned int p1ORp2 = (1U<<i1) | (1U<<i2);
	
	if (p1ORp2 & b->kings)
		b->kings ^= p1ORp2;
	
	if (p1ORp2 & b->whites) {
		b->whites ^= p1ORp2;
		if (i2 >= 28)
			b->kings |= 1U<<i2;
	}
	else {
		b->blacks ^= p1ORp2;
		if (i2 <= 3)
			b->kings |= 1U<<i2;
	}
	
	if (i1-i2>5 || i1-i2<-5) {
		unsigned int notEatenPiece = ~(1U<<(((i1+i2)>>1) + (!(p1ORp2 & up3down5))));
		
		b->kings &= notEatenPiece;
		b->whites &= notEatenPiece;
		b->blacks &= notEatenPiece;
		
		b->lastTurnCapture = 1;
		b->lastCaptureIndex = i2;
		updateValidMoves(b);
		if (!(b->canCapture & 1U<<i2)) {
			b->turn++;
			b->lastTurnCapture = 0;
			updateValidMoves(b);
		}
		return;
	}
	b->turn++;
	b->lastTurnCapture = 0;
	updateValidMoves(b);
}


