#include "Manager.h"

static Human* MakeHuman(const char* makename);
static int AddHuman(Human* addhuman);
static void Cleanup(void);

static int InitAll(void);
static int ActionAll(void);
static int UpdateAll(void);
static int PrintAll(void);
static int SpawnAll(void);

static Manager m_mana = {
	.MakeHuman = MakeHuman,
	.AddHuman = AddHuman,
	.Cleanup = Cleanup,

	.InitAll   = InitAll,
	.ActionAll = ActionAll,
	.UpdateAll = UpdateAll,
	.PrintAll  = PrintAll,
	.SpawnAll  = SpawnAll,
};

/* static Manager変数 m_mana のアドレスを返す。 */
extern const Manager* ManagerGetInstance(void)
{
	return &m_mana;
}

/* Human型変数を作成する。(Factory のラッパ関数）
 * makename が "Player" のとき、m_mana のメンバでそのアドレスを保持する。
 * @param <makename> 作成するHuman型変数の種類。
 * @return 作成した Human型変数のアドレス。
*/
static Human* MakeHuman(const char* makename)
{
	Human* newhuman = FactoryMakeHuman(makename);
	if (strcmp(makename, "Player") == 0) {
		m_mana.player = newhuman;
	}
	else if (strcmp(makename, "Hiroin") == 0) {
		m_mana.hiroin = newhuman;
	}
	return newhuman;
}

/* Manager に Human型変数を追加する。
 * @param <addhuman> 追加する Human型変数のアドレス。
 * @return 追加した Human型変数の uid。
*/
static int AddHuman(Human* addhuman)
{
	if (m_mana.humansSize >= NELEMS(m_mana.humans)) {
		return -1;
	}
	addhuman->uid = m_mana.uniqueID++;
	m_mana.humans[ m_mana.humansSize++ ] = addhuman;
	return addhuman->uid;//?
}

/* Manager に追加された Human型変数のアドレスからメモリを開放する。*/
static void Cleanup(void)
{
	for (int i = 0; i < m_mana.humansSize; ++i) {
		free(m_mana.humans[i]);
		m_mana.humans[i] = NULL;
	}
	m_mana.humansSize = 0;
	m_mana.uniqueID = 0;
}

/* Manager に追加されているすべてのHuman型変数を初期化。
 * Manager に追加されている Human型変数のメンバ関数を実行する
*/
static int InitAll(void)
{
	for (int i = 0; i < m_mana.humansSize; ++i) {
		Human* human = m_mana.humans[i];
		human->Init( human );
	}
	return 1;
}

/* Manager に追加されているすべてのHuman型変数を行動させる。
 * Manager に追加されている Human型変数のメンバ関数を実行する
*/
static int ActionAll(void)
{
	for (int i = 0; i < m_mana.humansSize; ++i) {
		Human* human = m_mana.humans[i];
		human->Action( human );
	}
	return 1;
}

/* Manager に追加されているすべてのHuman型変数を更新する。
 * Manager に追加されている Human型変数のメンバ関数を実行する
*/
static int UpdateAll(void)
{
	for (int i = 0; i < m_mana.humansSize; ++i) {
		Human* human = m_mana.humans[i];
		human->Update( human );
	}
	return 1;
}

/* Manager に追加されているすべてのHuman型変数を描画する。
 * Manager に追加されている Human型変数のメンバ関数を実行する
*/
static int PrintAll(void)
{
	for (int i = 0; i < m_mana.humansSize; ++i) {
		Human* human = m_mana.humans[i];
		human->Print( human );
	}
	return 1;
}

/* humans[] に格納された全てのキャラクターをランダムに配置する。
 * プレイヤーは配置しない。
 * キャラクターは 構築済み（なハズ）のウェイポイント上に置かれる。
 */
static int SpawnAll(void)
{
	struct WayPointData *wpd;
	bool loop;
	Position pos;
	Human *human;
	
	wpd = MapGetWayPointData();
	for (int i = 0; i < m_mana.humansSize; ++i)
	{
		human = m_mana.humans[i];
		if (human == m_mana.player)/*　プレイヤーはとばす　*/
			continue;

		do {
			loop = false;
			/* ウェイポイント配列からランダムなウェイポイントの座標を得る */
			pos  = wpd->points[ CommonGetRandom(0, wpd->elems - 1) ].pos;
			/* pos が重複しないかを走査 */
			for (int j = 0; j < m_mana.humansSize; ++j)
			{
				Human *h = m_mana.humans[j];
				if (human == h)/* 自分自身であればとばす */
					continue;
				if (h->pos.y == pos.y && h->pos.x == pos.x)
				{
					/* 重複する場合、座標の取得からやり直す */
					loop = true;
					break;
				}
			}
		} while (loop);
		/* 重複しない座標をセット */
		human->pos = pos;
	}
	return 1;
}

