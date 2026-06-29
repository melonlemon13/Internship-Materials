using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Controls;

public class InputManager : MonoBehaviour
{
    [SerializeField] private GameObject DebugObject; // デバッグ用のオブジェクトをInspectorで設定

    // キーボード入力用
    private Keyboard keyboard;
    // マウス入力用
    private Mouse mouse;

    // 入力をとりたいキー一覧
    private ButtonControl mouseLeftButton;
    private ButtonControl mouseRightButton;
    private Vector2 cursorPosition;
    private bool trIsMouseLeftButton;
    private bool upIsMouseLeftButton;
    private bool dwIsMouseLeftButton;
    private bool trIsMouseRightButton;
    private bool upIsMouseRightButton;
    private bool dwIsMouseRightButton;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        // キーボードを取得
        keyboard = Keyboard.current;
        // マウスを取得
        mouse = Mouse.current;

        // キーボードとマウスの両方が存在しない場合は処理を終了(処理数軽減)
        if (keyboard == null && mouse == null)
        {
            return;
        }

        // キーボード処理
        if (keyboard != null)
        {
            // キーボードの入力処理をここに追加
        }

        // マウス処理
        if (mouse != null)
        {
            // マウスの位置を取得
            cursorPosition = mouse.position.ReadValue();

            // マウスの入力処理をここに追加
            mouseLeftButton = mouse.leftButton;
            mouseRightButton = mouse.rightButton;
        }

        // マウスの左クリックとの状態を更新(Trigger)
        if (mouseLeftButton.wasPressedThisFrame)
        {
            trIsMouseLeftButton = true;
        }
        else
        {
            trIsMouseLeftButton = false;
        }

        // マウスの右クリックの状態を更新(Trigger)
        if (mouseRightButton.wasPressedThisFrame)
        {
            trIsMouseRightButton = true;
        }
        else
        {
            trIsMouseRightButton = false;
        }

        // デバッグ用のオブジェクトの表示/非表示を切り替える
        if (GetMouseLeftTrigger() && DebugObject != null)
        {
            DebugObject.SetActive(false);
        }
    }

    /// <summary>
    /// マウスのカーソル位置
    /// </summary>
    /// <returns>Vector2</returns>
    public Vector2 GetCursorPosition()
    {
        return cursorPosition;
    }

    /// <summary>
    /// マウス左クリックの状態(Trigger)
    /// </summary>
    /// <returns>bool</returns>
    public bool GetMouseLeftTrigger()
    {
        return trIsMouseLeftButton;
    }

    /// <summary>
    /// マウスの右クリックの状態(Trigger)
    /// </summary>
    /// <returns>bool</returns>
    public bool GetMouseRightTrigger()
    {
        return trIsMouseRightButton;
    }
}
