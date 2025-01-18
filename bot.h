#include "util.h"
#include <limits.h>
#include "checkers.h"

// Forward declaration of the struct
struct board_node_s;

// Typedef the struct as Node
typedef struct board_node_s Node;

// Define the struct
struct board_node_s {
    Board b;
    Node** children;
    unsigned char childrenCount;
    int eval;
};


Node* createNode(Board b) {
	Node* node = (Node*)malloc(sizeof(Node));
	node->b = b;
	node->children = NULL;
	node->childrenCount = 0;
	return node;
}


void addChild(Node* parent, Board child) {
    parent->childrenCount++;
    parent->children = (Node**)realloc(parent->children, parent->childrenCount * sizeof(Node*));
    parent->children[parent->childrenCount - 1] = createNode(child);
}


void generateChildren(Node* n) {
	Board b = n->b;
	
	unsigned int canMove = b.validMoves.up4 | b.validMoves.down4 | b.validMoves.up5or3 | b.validMoves.down5or3;
	
	for (int i=0; i<32; i++) {
		unsigned int p = 1U<<i;
		if (p & canMove) {
			if (b.canCapture) {
				if (p & 0x0F0F0F0F) {
					if (p & b.validMoves.up5or3) {
						Board child = b;
						movePiece(&child, i, i+9);
						addChild(n, child);
					}
					
					if (p & b.validMoves.up4) {
						Board child = b;
						movePiece(&child, i, i+7);
						addChild(n, child);
					}
					
					if (p & b.validMoves.down5or3) {
						Board child = b;
						movePiece(&child, i, i-7);
						addChild(n, child);
					}
					
					if (p & b.validMoves.down4) {
						Board child = b;
						movePiece(&child, i, i-9);
						addChild(n, child);
					}
				}
				
				else {
					if (p & b.validMoves.up5or3) {
						Board child = b;
						movePiece(&child, i, i+7);
						addChild(n, child);
					}
					
					if (p & b.validMoves.up4) {
						Board child = b;
						movePiece(&child, i, i+9);
						addChild(n, child);
					}
					
					if (p & b.validMoves.down5or3) {
						Board child = b;
						movePiece(&child, i, i-9);
						addChild(n, child);
					}
					
					if (p & b.validMoves.down4) {
						Board child = b;
						movePiece(&child, i, i-7);
						addChild(n, child);
					}
				}
			} else {
				if (p & b.validMoves.up5or3) {
					Board child = b;
					if (p & up5down3)
						movePiece(&child, i, i+5);
					else
						movePiece(&child, i, i+3);
					addChild(n, child);
				}
				
				if (p & b.validMoves.up4) {
					Board child = b;
					movePiece(&child, i, i+4);
					addChild(n, child);
				}
				
				if (p & b.validMoves.down5or3) {
					Board child = b;
					if (p & up5down3)
						movePiece(&child, i, i-3);
					else
						movePiece(&child, i, i-5);
					addChild(n, child);
				}
				
				if (p & b.validMoves.down4) {
					Board child = b;
					movePiece(&child, i, i-4);
					addChild(n, child);
				}
			}
		}
	}
}


int getChildren(Board b, Board* children)
{
	int childCounter = 0;

	unsigned int canMove = b.validMoves.up4 | b.validMoves.down4 | b.validMoves.up5or3 | b.validMoves.down5or3;

	for (int i = 0; i < 32; i++)
	{
		unsigned int p = 1U << i;
		if (p & canMove)
		{
			if (b.canCapture)
			{
				if (p & 0x0F0F0F0F)
				{
					if (p & b.validMoves.up5or3)
					{
						Board child = b;
						movePiece(&child, i, i + 9);
						children[childCounter++] = child;
					}

					if (p & b.validMoves.up4)
					{
						Board child = b;
						movePiece(&child, i, i + 7);
						children[childCounter++] = child;
					}

					if (p & b.validMoves.down5or3)
					{
						Board child = b;
						movePiece(&child, i, i - 7);
						children[childCounter++] = child;
					}

					if (p & b.validMoves.down4)
					{
						Board child = b;
						movePiece(&child, i, i - 9);
						children[childCounter++] = child;
					}
				}

				else
				{
					if (p & b.validMoves.up5or3)
					{
						Board child = b;
						movePiece(&child, i, i + 7);
						children[childCounter++] = child;
					}

					if (p & b.validMoves.up4)
					{
						Board child = b;
						movePiece(&child, i, i + 9);
						children[childCounter++] = child;
					}

					if (p & b.validMoves.down5or3)
					{
						Board child = b;
						movePiece(&child, i, i - 9);
						children[childCounter++] = child;
					}

					if (p & b.validMoves.down4)
					{
						Board child = b;
						movePiece(&child, i, i - 7);
						children[childCounter++] = child;
					}
				}
			}
			else
			{
				if (p & b.validMoves.up5or3)
				{
					Board child = b;
					if (p & up5down3)
						movePiece(&child, i, i + 5);
					else
						movePiece(&child, i, i + 3);
					children[childCounter++] = child;
				}

				if (p & b.validMoves.up4)
				{
					Board child = b;
					movePiece(&child, i, i + 4);
					children[childCounter++] = child;
				}

				if (p & b.validMoves.down5or3)
				{
					Board child = b;
					if (p & up5down3)
						movePiece(&child, i, i - 3);
					else
						movePiece(&child, i, i - 5);
					children[childCounter++] = child;
				}

				if (p & b.validMoves.down4)
				{
					Board child = b;
					movePiece(&child, i, i - 4);
					children[childCounter++] = child;
				}
			}
		}
	}
}

int buildTree(Node* n, int depth) {
	if (depth==0)
		return 1;
	
	if (n->childrenCount == 0)
		generateChildren(n);
	
	int sum = 0;
	
	for (int i = 0; i < n->childrenCount; i++)
		sum += buildTree(n->children[i], ((n->b.lastTurnCapture || n->b.canCapture) ? depth : depth-1));
	
	return sum;
}



void freeTree(Node* root) {
	if (root == NULL) return;

	// Recursively free all child nodes
	for (unsigned char i = 0; i < root->childrenCount; i++)
		freeTree(root->children[i]);

	// Free the array of children
	free(root->children);

	// Free the root node itself
	free(root);
}



int evaluatePosition(Board b) {
	if (!(b.validMoves.up4 | b.validMoves.down4 | b.validMoves.up5or3 | b.validMoves.down5or3))
		return (b.turn ? 1000 : -1000);
	
	int whitesCount = hammingWeight(b.whites);
	int whiteKingsCount = hammingWeight(b.whites & b.kings);
	
	int blacksCount = hammingWeight(b.blacks);
	int blackKingsCount = hammingWeight(b.blacks & b.kings);
	
	int score = (whitesCount<<4) + (whiteKingsCount<<3) - (blacksCount<<4) - (blackKingsCount<<3);
	
	if (whitesCount + blacksCount <= 12) {
		unsigned int nonKingWhites = b.whites & (~b.kings);
		unsigned int nonKingBlacks = b.whites & (~b.kings);
		
		score += hammingWeight(0x0FF00000 & nonKingWhites) * 3;
		score += hammingWeight(0x000FF000 & nonKingWhites) * 2;
		score += hammingWeight(0x00000FF0 & nonKingWhites);
		
		score -= hammingWeight(0x00000FF0 & nonKingBlacks) * 3;
		score -= hammingWeight(0x000FF000 & nonKingBlacks) * 2;
		score -= hammingWeight(0x0FF00000 & nonKingBlacks);
	}
	
	return score;
}



int minimax(Node* node) {
    if (node->children == NULL) {
        return evaluatePosition(node->b);
    }

    if (node->b.turn == 0) { // if white is moving
        int maxEval = INT_MIN;
        for (unsigned char i = 0; i < node->childrenCount; i++) {
            int eval = minimax(node->children[i]);
            maxEval = eval > maxEval ? eval : maxEval;
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (unsigned char i = 0; i < node->childrenCount; i++) {
            int eval = minimax(node->children[i]);
            minEval = eval < minEval ? eval : minEval;
        }
        return minEval;
    }
}


int minimax2(Board b, int depth)
{
	if (depth <= 0)
		return evaluatePosition(b);

	Board children[42];
	int childrenCount = getChildren(b, children);
	if (!b.canCapture) depth--;

	if (b.turn == 0) { // whites turn
		int maxEval = INT_MIN;

		for (unsigned char i = 0; i < childrenCount; i++)
		{
			int eval = minimax2(children[i], depth);
			maxEval = eval > maxEval ? eval : maxEval;
		}
		return maxEval;
	}
	else {
		int minEval = INT_MAX;

		for (unsigned char i = 0; i < childrenCount; i++)
		{
			int eval = minimax2(children[i], depth);
			minEval = eval < minEval ? eval : minEval;
		}
		return minEval;
	}
}

Board choose_best_move(Node* root) {
	Board bestMove;
	
	if (root->childrenCount == 0) return root->b;
	
	if (root->childrenCount == 1) return root->children[0]->b;
	
	if (root->b.turn==0) {
		int bestEval = INT_MIN;
		for (int i = 0; i < root->childrenCount; i++) {
		    int eval = minimax(root->children[i]);
		    if (eval > bestEval) {
		        bestEval = eval;
		        bestMove = root->children[i]->b;
		    }
		}
	} else {
		int bestEval = INT_MAX;
		for (int i = 0; i < root->childrenCount; i++) {
		    int eval = minimax(root->children[i]);
		    if (eval < bestEval) {
		        bestEval = eval;
		        bestMove = root->children[i]->b;
		    }
		}
	}
    return bestMove;
}



