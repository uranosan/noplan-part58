#BUG

## キャラクタ同士がめり込む。
date:2014/02/01

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
