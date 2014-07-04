#include "Common.h"

extern int CommonGetRandom(int min, int max)
{
    return min + (int)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}

extern double CommonGetDistance(int ay, int ax, int by, int bx)
{
	return sqrt((ay - by)*(ay - by) + (ax - bx)*(ax - bx));
}

/* LOS経路作成
 * @param	path		作成したパスを格納する配列
 * @param	pathSize	パスの要素数
 * @param	me			パスを作成する始点座標
 * @param	to			パスを作成する終点座標
*/
extern void CommonBuildPathTo(Position *path, int pathSize, Position me, Position to)
{
	int nextCol = me.x;
	int nextRow = me.y;
	int deltaRow = to.y - me.y;
	int deltaCol = to.x - me.x;
	int stepCol, stepRow;
	int currentStep, fraction;
	
	// 経路の初期化
	for(currentStep=0; currentStep < pathSize; currentStep++) {
		path[ currentStep ].y = -1;
		path[ currentStep ].x = -1;
	}
	currentStep = 0;
	
	// 進行方向の計算。計算済みの deltaRow と deltaCol の値を使って進行方向を決定する
	if( deltaRow < 0 ) stepRow = -1; else stepRow = 1;
	if( deltaCol < 0 ) stepCol = -1; else stepCol = 1;
	deltaRow = abs( deltaRow * 2 );
	deltaCol = abs( deltaCol * 2 );
	
	// ブレゼンハムアルゴリズム
	path[ currentStep ].y = nextRow;
	path[ currentStep ].x = nextCol;
	currentStep++;
	if( currentStep >= pathSize ) {
		return;
	}
	// 列軸のほうが長い場合
	if( deltaCol > deltaRow ) {
		fraction = deltaRow * 2 - deltaCol;
		
		while( nextCol != to.x )
		{
			if( fraction >= 0 ){
				nextRow  = nextRow + stepRow;
				fraction = fraction - deltaCol;
			}
			nextCol = nextCol + stepCol;
			fraction = fraction + deltaRow;
			path[ currentStep ].y = nextRow;
			path[ currentStep ].x = nextCol;
			currentStep++;
			if( currentStep >= pathSize )
				return;
		}
	} else {
		// 行軸のほうが長い場合
		fraction = deltaCol * 2 - deltaRow;
		
		while( nextRow != to.y )
		{
			if( fraction >= 0 ){
				nextCol  = nextCol + stepCol;
				fraction = fraction - deltaRow;
			}
			nextRow = nextRow + stepRow;
			fraction = fraction + deltaCol;
			path[ currentStep ].y = nextRow;
			path[ currentStep ].x = nextCol;
			currentStep++;
			if( currentStep >= pathSize )
				return;
		}
	}
	return;
}

