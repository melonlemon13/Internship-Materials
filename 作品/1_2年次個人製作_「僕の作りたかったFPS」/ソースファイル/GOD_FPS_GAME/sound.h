#pragma once

#include <xaudio2.h>

// サウンドファイル
typedef enum
{
	//SOUND_LABEL_BGM000 = 0,		// サンプルBGM
	////SOUND_LABEL_BGM001,		// サンプルBGM
	////SOUND_LABEL_SE000,		// サンプルSE
	////SOUND_LABEL_SE001,		// サンプルSE
	SOUND_LABEL_VOICE_TAKE_DAMAGE,
	SOUND_LABEL_VOICE_RELOAD,
	SOUND_LABEL_VOICE_LOW_HP,
	SOUND_LABEL_VOICE_GRENADE,
	SOUND_LABEL_VOICE_ULT,
	SOUND_LABEL_SE_PISTOL_FIRING,
	SOUND_LABEL_SE_PISTOL_RELOAD,
	SOUND_LABEL_SE_AR_FIRING,
	SOUND_LABEL_SE_AR_RELOAD_START,
	SOUND_LABEL_SE_AR_RELOAD_END,
	SOUND_LABEL_SE_KNIFE,
	SOUND_LABEL_SE_ENEMY_HIT,
	SOUND_LABEL_SE_ENEMY_DEAD,
	SOUND_LABEL_SE_THROW_GRENADE,
	SOUND_LABEL_SE_EXPLOSION_GRENADE,
	SOUND_LABEL_SE_BLINK,
	SOUND_LABEL_SE_WARNING,
	SOUND_LABEL_SE_LOADING,
	SOUND_LABEL_SE_ULT,
	SOUND_LABEL_SE_GAMEOVER,
	SOUND_LABEL_SE_START,
	SOUND_LABEL_SE_CLICK,
	SOUND_LABEL_SE_WIN,

	SOUND_LABEL_MAX,
} SOUND_LABEL;

class Sound {
private:
	// パラメータ構造体
	typedef struct
	{
		LPCSTR filename;	// 音声ファイルまでのパスを設定
		bool bLoop;			// trueでループ。通常BGMはture、SEはfalse。
		float volume; //音量
	} PARAM;

	PARAM m_param[SOUND_LABEL_MAX] =
	{
//		{"asset/BGM/sample000.wav", true},	// サンプルBGM（ループさせるのでtrue設定）
////		{"asset/BGM/○○○.wav", true},	// サンプルBGM
////		{"asset/SE/○○○.wav", false},  		// サンプルSE（ループしないのでfalse設定）
////		{"asset/SE/○○○.wav", false},		// サンプルSE
		//セリフ
		 {"assets/sound/voice/Take_Damage.wav", false,0.8f},//被弾
		 {"assets/sound/voice/Reload.wav", false,1.2f},//リロード
		 {"assets/sound/voice/Hp_low.wav", false,1.2f},//低HP音
		 {"assets/sound/voice/Grenade.wav", false,1.2f},//グレネード使用
		 {"assets/sound/voice/Ult.wav", false,1.2f},//ウルト
		 //SE
		  {"assets/sound/se/Pistol_Firing.wav", false,0.2f},      //ピストル発射
		{"assets/sound/se/Pistol_Reload.wav", false,1.0f},      //ピストルリロード
		{"assets/sound/se/Assault_Firing.wav", false,0.1f},      //AR発射
		{"assets/sound/se/Assault_Reload_Start.wav", false,1.0f},      //ARリロード開始
		{"assets/sound/se/Assault_Reload_Finish.wav", false,1.0f},      //ARリロード終了
		{"assets/sound/se/Knife.wav", false,1.0f},       //ナイフ攻撃
		{"assets/sound/se/Enemy_Hit.wav", false,0.2f},        //敵に対してのヒット音
		{"assets/sound/se/Enemy_Dead.wav", false,0.08f}, //敵死亡音
		{"assets/sound/se/Throw_Grenade.wav", false,0.3f}, //グレネード投擲音
		{"assets/sound/se/Explosion_Grenade.wav", false,0.3f}, //グレネード爆発音
		{"assets/sound/se/Blink.wav", false,0.3f}, //グレネード爆発音
		{"assets/sound/se/Warning.wav", false,0.2f}, //警告音
		{"assets/sound/se/Loading.wav", true,0.2f}, //Loading
		{"assets/sound/se/ULT_SE.wav", false,0.2f}, //ULT
		{"assets/sound/se/Lose.wav", false,0.2f}, //ゲームオーバー
		{"assets/sound/se/Start.wav", false,0.2f}, //スタート
		{"assets/sound/se/click.wav", false,0.2f}, //クリック
		{"assets/sound/se/Win.wav", false,0.2f}, //クリック

	};

	IXAudio2* m_pXAudio2 = NULL;
	IXAudio2MasteringVoice* m_pMasteringVoice = NULL;
	IXAudio2SourceVoice* m_pSourceVoice[SOUND_LABEL_MAX];
	WAVEFORMATEXTENSIBLE m_wfx[SOUND_LABEL_MAX]; // WAVフォーマット
	XAUDIO2_BUFFER m_buffer[SOUND_LABEL_MAX];
	BYTE* m_DataBuffer[SOUND_LABEL_MAX];

	HRESULT FindChunk(HANDLE, DWORD, DWORD&, DWORD&);
	HRESULT ReadChunkData(HANDLE, void*, DWORD, DWORD);

public:
	// ゲームループ開始前に呼び出すサウンドの初期化処理
	HRESULT Init(void);

	// ゲームループ終了後に呼び出すサウンドの解放処理
	void Uninit(void);

	// 引数で指定したサウンドを再生する
	void Play(SOUND_LABEL label);

	// 引数で指定したサウンドを停止する
	void Stop(SOUND_LABEL label);

	// 引数で指定したサウンドの再生を再開する
	void Resume(SOUND_LABEL label);

};