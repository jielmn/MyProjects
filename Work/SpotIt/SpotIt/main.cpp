#include <stdio.h>

#define  CARDS_CNT    30
#define  IMAGES_CNT   31

#define  TRUE    1
#define  FALSE   0

typedef int BOOL;

int g_data[30][6] = {
	{  1,  2,  3,  4,  5, 31 },
	{  6,  7,  8,  9, 10, 31 },
	{ 11, 12, 13, 14, 15, 31 },
	{ 16, 17, 18, 19, 20, 31 },
	{ 21, 22, 23, 24, 25, 31 },
	{ 26, 27, 28, 29, 30, 31 },               // 6

	{ 6,  11, 16, 21, 26, 1 },
	{ 7,  12, 17, 22, 27, 1 },
	{ 8,  13, 18, 23, 28, 1 },
	{ 9,  14, 19, 24, 29, 1 },
	{ 10, 15, 20, 25, 30, 1 },                // 11

	{ 6, 12, 18, 24, 30, 2 },
	{ 7, 13, 19, 25, 26, 2 },
	{ 8, 14, 20, 21, 27, 2 },
	{ 9, 15, 16, 22, 28, 2 },
	{ 10,11, 17, 23, 29, 2 },                // 16

	{ 6,  13, 20, 22, 29, 3 },
	{ 7,  14, 16, 23, 30, 3 },
	{ 8,  15, 17, 24, 26, 3 },
	{ 9,  11, 18, 25, 27, 3 },
	{ 10, 12, 19, 21, 28, 3 },              // 21

	{ 6, 14, 17, 25, 28, 4 },
	{ 7, 15, 18, 21, 29, 4 },
	{ 8, 11, 19, 22, 30, 4 },
	{ 9, 12, 20, 23, 26, 4 },
	{ 10,13, 16, 24, 27, 4 },             // 26

	{ 6, 15, 19, 23, 27, 5 },
	{ 7, 11, 20, 24, 28, 5 },
	{ 8, 12, 16, 25, 29, 5 },
	{ 9, 13, 17, 21, 30, 5 },
};

int CheckSingleCard(int * pCard) {
	for ( int i = 0; i < 6; i++ ) {
		// 图案必须是1~31
		if ( pCard[i] < 0 || pCard[i] > IMAGES_CNT ) {
			return 1;
		}

		for (int j = i + 1; j < 6; j++) {
			if ( pCard[i] == pCard[j] ) {
				return 2;
			}
		}
	}
	return 0;
}

int CheckTwinCards(int * pCard1, int * pCard2) {
	int nSameImageCnt = 0;
	for ( int i = 0; i < 6; i++ ) {
		for ( int j = 0; j < 6; j++ ) {
			if ( pCard1[i] == pCard2[j] ) {
				nSameImageCnt++;
				if (nSameImageCnt > 1) {
					return 1;
				}
			}
		}
	}

	if ( nSameImageCnt == 1 ) {
		return 0;
	}
	else {
		return 2;
	}
}

BOOL CheckCards() {
	for ( int i = 0; i < CARDS_CNT; i++ ) {
		// 每张卡片的6种图案在卡片内互不相同
		int nCheckSingle = CheckSingleCard(g_data[i]);
		if ( 2 == nCheckSingle ) {
			printf("第%d张卡片内部有重复的图案!\n", i + 1);
			return FALSE;
		}
		else if (1 == nCheckSingle) {
			printf("第%d张卡片内部图案不在1~31范围内!\n", i + 1);
			return FALSE;
		}

		// 每两张卡片有且有一张同样的图案
		for ( int j = i + 1; j < CARDS_CNT; j++ ) {
			int nCheckTwin = CheckTwinCards(g_data[i], g_data[j]);
			if ( nCheckTwin == 1 ) {
				printf("第%d张卡片和第%d张卡片相同的图案有两种或以上!\n", i + 1, j + 1);
				return FALSE;
			}
			else if (nCheckTwin == 2) {
				printf("第%d张卡片和第%d张卡片没有相同的图案!\n", i + 1, j + 1);
				return FALSE;
			}
		}
	}

	return TRUE;
}

int main() {
	BOOL bCheck = CheckCards();
	if (bCheck) {
		printf("all cards is checked OK! \n");
	}
	getchar();
	return 0;
}