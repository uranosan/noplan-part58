#BUG

## コンティニュー時のプレイヤーと、他キャラクターが被っていたら
date: 2014/07/19

プレイヤーは死亡すると初期配置座標に再配置される。  
この再配置される座標上に他のキャラクターがいた場合、キャラクター同士の衝突判定が働くため、再配置されたプレイヤーは身動きが取れない。  
回避案：

1. 初期配置座標から一定範囲内にプレイヤーがいる場合、衝突判定（Human.c:HumanCollideTo()）を切る。
2. プレイヤーの初期配置座標周辺を安全地帯にする（他キャラクターが侵入できないようにする）

衝突判定を切っても、範囲外に出た時に他キャラクターと重なる形であれば、結局身動きが取れない。
そういうわけで安全地帯が有力か。

## キャラクタ同士がめり込む。
date: 2014/02/01

プレイヤのパーツの基点添え字は HumanUpdate() で更新される。

	Human.c: @HumanUpdate():
		human->partsOrigin = GetPartsOrigin(human->state.dir, human->state.isAttack);

state.dir と state.isAttack は Player.c 内で更新される。

	PlayerAction()
	...
	PlayerUpdate()

プレイヤが移動したとき、dir は PlayerAction() で更新されるが、パーツの基点添え字は PlayerUpdate()。

プレイヤに対してヒロインが衝突判定したとき、この判定はパーツ基点添え字に依存している。
判定に使うキャラクタの配列は、パーツ基点添え字を基点にキャラクタの大きさ分の範囲だが、
ヒロインが判定時に使う添え字は、更新前になる。

	PlayerAction() <- maybe update dir.
	HiroinAction() <- maybe collision with parts index.
	...
	PlayerUpdate() <- update parts index.
	HiroinUpdate() <- update parts index.

なので、プレイヤが向きを変えて移動したとき、ヒロインは、向きを変える前のプレイヤと衝突判定していることになる。
というわけでめり込む。
