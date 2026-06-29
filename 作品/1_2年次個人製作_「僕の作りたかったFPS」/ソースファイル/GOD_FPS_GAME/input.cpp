#include "input.h"

#include <Windows.h>
#include "Application.h"

Input* Input::m_Instance = {};

void Input::Create()
{
	if (m_Instance)return;
	m_Instance = new Input;

	m_Instance->VibrationTime = 0;

	GetCursorPos(&m_Instance->mousePos);
	m_Instance->mousePosOld = m_Instance->mousePos;
	m_Instance->mouseDeltaX = 0;
	m_Instance->mouseDeltaY = 0;
}

void Input::Update()
{
	//1フレーム前の入力を記録しておく
	for (int i = 0; i < 256; i++) { m_Instance->keyState_old[i] = m_Instance->keyState[i]; }
	m_Instance->controllerState_old = m_Instance->controllerState;

	//キー入力を更新
	BOOL hr = GetKeyboardState(m_Instance->keyState);

	//コントローラー入力を更新(XInput)
	XInputGetState(0, &(m_Instance->controllerState));

	//マウス更新
	HWND hwnd = Application::GetWindow();

	//クライアント領域の中央をスクリーン座標で求める
	RECT rc;
	GetClientRect(hwnd, &rc);

	POINT center;
	center.x = (rc.right - rc.left) / 2;
	center.y = (rc.bottom - rc.top) / 2;
	ClientToScreen(hwnd, &center); // スクリーン座標へ変換

	POINT now;
	GetCursorPos(&now);

	if (m_Instance->m_MouseLock)
	{
		// 中央との差分をマウス移動量にする
		m_Instance->mouseDeltaX = now.x - center.x;
		m_Instance->mouseDeltaY = now.y - center.y;

		// カーソルを中央に戻す（FPSの無限回転）
		SetCursorPos(center.x, center.y);
	}
	else
	{
		// 通常時（固定しない）
		m_Instance->mousePosOld = m_Instance->mousePos;
		m_Instance->mousePos = now;

		m_Instance->mouseDeltaX = m_Instance->mousePos.x - m_Instance->mousePosOld.x;
		m_Instance->mouseDeltaY = m_Instance->mousePos.y - m_Instance->mousePosOld.y;
	}

	if (m_Instance->VibrationTime > 0) {
		m_Instance->VibrationTime--;
		if (m_Instance->VibrationTime == 0) {
			XINPUT_VIBRATION vibration;
			ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
			vibration.wLeftMotorSpeed = 0;
			vibration.wRightMotorSpeed = 0;
			XInputSetState(0, &vibration);
		}
	}
}
	//// マウス更新
	//m_Instance->mousePosOld = m_Instance->mousePos;

	//GetCursorPos(&m_Instance->mousePos);

	//m_Instance->mouseDeltaX = m_Instance->mousePos.x - m_Instance->mousePosOld.x;
	//m_Instance->mouseDeltaY = m_Instance->mousePos.y - m_Instance->mousePosOld.y;

	////1フレーム前の入力を記録しておく
	//for (int i = 0; i < 256; i++) { m_Instance->keyState_old[i] = m_Instance->keyState[i]; }
	//m_Instance->controllerState_old = m_Instance->controllerState;

	////キー入力を更新
	//BOOL hr = GetKeyboardState(m_Instance->keyState);

	////コントローラー入力を更新(XInput)
	//XInputGetState(0, &(m_Instance->controllerState));

	////振動継続時間をカウント
	//if (m_Instance->VibrationTime > 0) {
	//	m_Instance->VibrationTime--;
	//	if (m_Instance->VibrationTime == 0) { //振動継続時間が経った時に振動を止める
	//		XINPUT_VIBRATION vibration;
	//		ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	//		vibration.wLeftMotorSpeed = 0;
	//		vibration.wRightMotorSpeed = 0;
	//		XInputSetState(0, &vibration);
	//	}
	//}
	//}

void Input::Release()
{
	//振動を終了させる
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = 0;
	vibration.wRightMotorSpeed = 0;
	XInputSetState(0, &vibration);

	//解放
	if (m_Instance)
	{
		delete m_Instance;
		m_Instance = NULL;
	}
}

//キー入力
bool Input::GetKeyPress(int key) //プレス
{
	return m_Instance->keyState[key] & 0x80;
}
bool Input::GetKeyTrigger(int key) //トリガー
{
	return (m_Instance->keyState[key] & 0x80) && !(m_Instance->keyState_old[key] & 0x80);
}
bool Input::GetKeyRelease(int key) //リリース
{
	return !(m_Instance->keyState[key] & 0x80) && (m_Instance->keyState_old[key] & 0x80);
}

//左アナログスティック
DirectX::XMFLOAT2 Input::GetLeftAnalogStick(void)
{
	SHORT x = m_Instance->controllerState.Gamepad.sThumbLX; // -32768～32767
	SHORT y = m_Instance->controllerState.Gamepad.sThumbLY; // -32768～32767

	DirectX::XMFLOAT2 res;
	res.x = x / 32767.0f; //-1～1
	res.y = y / 32767.0f; //-1～1
	return res;
}
//右アナログスティック
DirectX::XMFLOAT2 Input::GetRightAnalogStick(void)
{
	SHORT x = m_Instance->controllerState.Gamepad.sThumbRX; // -32768～32767
	SHORT y = m_Instance->controllerState.Gamepad.sThumbRY; // -32768～32767

	DirectX::XMFLOAT2 res;
	res.x = x / 32767.0f; //-1～1
	res.y = y / 32767.0f; //-1～1
	return res;
}

//左トリガー
float Input::GetLeftTrigger(void)
{
	BYTE t = m_Instance->controllerState.Gamepad.bLeftTrigger; // 0～255
	return t / 255.0f;
}
//右トリガー
float Input::GetRightTrigger(void)
{
	BYTE t = m_Instance->controllerState.Gamepad.bRightTrigger; // 0～255
	return t / 255.0f;
}

//ボタン入力
bool Input::GetButtonPress(WORD btn) //プレス
{
	return (m_Instance->controllerState.Gamepad.wButtons & btn) != 0;
}
bool Input::GetButtonTrigger(WORD btn) //トリガー
{
	return (m_Instance->controllerState.Gamepad.wButtons & btn) != 0 && (m_Instance->controllerState_old.Gamepad.wButtons & btn) == 0;
}
bool Input::GetButtonRelease(WORD btn) //リリース
{
	return (m_Instance->controllerState.Gamepad.wButtons & btn) == 0 && (m_Instance->controllerState_old.Gamepad.wButtons & btn) != 0;
}

//振動
void Input::SetVibration(int frame, float powor)
{
	// XINPUT_VIBRATION構造体のインスタンスを作成
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

	// モーターの強度を設定（0～65535）
	vibration.wLeftMotorSpeed = (WORD)(powor * 65535.0f);
	vibration.wRightMotorSpeed = (WORD)(powor * 65535.0f);
	XInputSetState(0, &vibration);

	//振動継続時間を代入
	m_Instance->VibrationTime = frame;
}

int Input::GetMouseMoveX()
{
	return m_Instance ? m_Instance->mouseDeltaX : 0;
}

int Input::GetMouseMoveY()
{
	return m_Instance ? m_Instance->mouseDeltaY : 0;
}

void Input::SetMouseLock(bool lock)
{
	if (!m_Instance) return;

	m_Instance->m_MouseLock = lock;

	// カーソル表示/非表示
	ShowCursor(!lock);
}