using UnityEngine;
using TMPro;//Textを使うため
using System.Collections.Generic;
using System.IO;

public class email : MonoBehaviour
{
    [SerializeField] GameObject EmailFirst;             // ボタンを取得、Inspectorで設定
    [SerializeField] GameObject EmailSecond;            // ボタンを取得、Inspectorで設定
    [SerializeField] GameObject EmailThird;             // ボタンを取得、Inspectorで設定
    [SerializeField] TextMeshProUGUI EmailFirstTxt;     // textboxを取得、Inspectorで設定
    [SerializeField] TextMeshProUGUI EmailSecondTxt;    // textboxを取得、Inspectorで設定
    [SerializeField] TextMeshProUGUI EmailThirdTxt;    // textboxを取得、Inspectorで設定
    [SerializeField] TextMeshProUGUI textbox;           // textboxを取得、Inspectorで設定
    [SerializeField] MapManager mapmanager;             // MapManager、Inspectorで設定
    [SerializeField] GameSceneManager gamescenemanager; // GameSceneManager、Inspectorで設定
    [SerializeField] InputManager inputmanager;         // InputManager、Inspectorで設定
    [SerializeField] Call call;                         // Call、Inspectorで設定

    private int currentTurn = 0;    // 現在ターン
    private bool IsGetClick = false;   // クリックされたかどうか

    private int CurrentLine = 0;            // 現在の行数を格納する変数
    private int ChangedDistance = 0;        // 距離の変化量を格納する配列
    List<MapManager.MessageData> LineData;  // 選択肢のメッセージデータを格納するリスト
    int[] RandomLine = { 10, 10, 10 };      // ランダムな数値を格納する配列

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        // 現在のターン数を取得
        currentTurn = gamescenemanager.GetCurrentTurn();
        // 状態変更イベントを受け取る
        gamescenemanager.OnStateChanged += OnStateChanged;
        // ボタン非アクティブ
        SetMailButtomActiveOff();
        //textbox.text = "メリーさんからメールが届いた\n開いてみよう";//テキストを変更
    }

    // Update is called once per frame
    void Update()
    {
        if (inputmanager.GetMouseLeftTrigger() && IsGetClick)
        {
            EndSendEmailEvent(); // メールイベント終了
        }
    }

    //ゲームの状態が変わったときに呼び出される関数
    void OnStateChanged(GameSceneManager.GameState state)
    {
        //ゲームの状態がSendEmailのときにメール返信イベント開始
        if (state == GameSceneManager.GameState.SendEmail)
        {
            currentTurn = gamescenemanager.GetCurrentTurn(); // 現在のターン数を取得

            IsGetClick = false; // クリックされたかどうかを初期化
            bool supStart = false; // フラグ

            // 例外
            if (currentTurn == 0)
            {
                MessageUpdate("0というターンは存在しないはずですが…\nここはOnStateChangedですね\n");
            }
            // チュートリアル分岐
            else if (currentTurn == 1)
            {
                MessageUpdate("めりぃちゃんから届いたメールに\n返信しましょう。\n");//テキストを変更
                if (inputmanager.GetMouseLeftTrigger())
                {
                    supStart = true; // フラグを立てる
                }
            }
            else
            {
                MessageUpdate("返信しなくちゃ。\n");//テキストを変更
                if (inputmanager.GetMouseLeftTrigger())
                {
                    supStart = true; // フラグを立てる
                }
            }

            if (supStart)
            {
                // 1行目からスタート
                CurrentLine = 1;
                StartWriteMail();
            }
        }
    }

    // 1行目処理
    void StartWriteMail()
    {
        //--CurrentLine行目のメッセージデータを取得--//
        //LineData = mapmanager.GetPlayerMessages(CurrentLine); // CurrentLine行目のメッセージデータを取得

        bool supStart = false; // フラグ

        // 例外
        if (currentTurn == 0)
        {
            MessageUpdate("0というターンは存在しないはずですが…\nここはStartWriteMailですね\n");
        }
        // チュートリアル分岐
        else
        {
            MessageUpdate("メールに打ち込む文章を決めましょう。\n");//テキストを変更

            ReferenceLine(CurrentLine); // CurrentLine行目のメッセージデータを取得、設定

            if (inputmanager.GetMouseLeftTrigger())
            {
                supStart = true; // フラグを立てる
            }
        }

        if (supStart)
        {
            SetMailButtomActiveOn(); // 選択肢ボタンアクティブ
        }
        //---------------------------------//
    }

    // 2行目以降処理
    void WriteMail()
    {
        ReferenceLine(CurrentLine); // CurrentLine行目のメッセージデータを取得、設定

        SetMailButtomActiveOn(); // 選択肢ボタンアクティブ
    }

    /// <summary>
    /// ボタンを押されたら押されたボタンに応じてテキストと距離変化量計算
    /// </summary>
    /// <param name="button">押されたボタン0~2</param>
    public void SelectMessage(int button)
    {
        // 例外
        if (currentTurn == 0)
        {
            MessageUpdate("0というターンは存在しないはずですが…\nここはSelectMessageですね\n");
        }
        // チュートリアル分岐
        else if (currentTurn == 1)
        {
            // 選択肢ボタン非アクティブ
            SetMailButtomActiveOff();

            // チュートリアル用メッセージを取得、設定
            MessageUpdate(LineData[button + 6].text);//テキストを変更
            ChangedDistance += LineData[button + 6].distanceDelta; // 距離の変化量を加算

            // CurrentLineが3行目未満ならCurrentLineを増やして次の行の処理、3行目ならメールイベント終了
            if (CurrentLine < 3)
            {
                CurrentLine++;
                RandomLine[0] = 10;
                RandomLine[1] = 10;
                RandomLine[2] = 10;
                WriteMail();
            }
            else if (CurrentLine == 3)
            {
                IsGetClick = true;
            }
        }
        else
        {
            // 選択肢ボタン非アクティブ
            SetMailButtomActiveOff();

            // 選択されたボタンに応じてテキストを変更、距離の変化量を加算
            MessageUpdate(LineData[RandomLine[button]].text);//テキストを変更
            ChangedDistance += LineData[RandomLine[button]].distanceDelta;

            // CurrentLineが3行目未満ならCurrentLineを増やして次の行の処理、3行目ならメールイベント終了
            if (CurrentLine < 3)
            {
                CurrentLine++;
                RandomLine[0] = 10;
                RandomLine[1] = 10;
                RandomLine[2] = 10;
                WriteMail();
            }
            else if (CurrentLine == 3)
            {
                IsGetClick = true;
            }
        }
    }

    /// <summary>
    /// n行目決定(ボタン)
    /// </summary>
    /// <param name="lineNum">何行目か</param>
    private void ReferenceLine(int lineNum)
    {
        //--lineNum行目のメッセージデータを取得--//
        LineData = mapmanager.GetPlayerMessages(lineNum); // lineNum行目のメッセージデータを取得

        // 例外
        if (currentTurn == 0)
        {
            MessageUpdate("0というターンは存在しないはずですが…\nここはReferenceLineですね\n");
        }
        // チュートリアル分岐
        else if (currentTurn == 1)
        {
            // チュートリアル用メッセージを取得、設定
            ButtonTxtUpdate(EmailFirstTxt, LineData[6].text);
            ButtonTxtUpdate(EmailSecondTxt, LineData[7].text);
            ButtonTxtUpdate(EmailThirdTxt, LineData[8].text);
        }
        else
        {
            // ランダムな数値を生成して、lineNum行目のメッセージデータからランダムに選択肢を取得、設定(内容が全て重ならないように)
            for (int i = 0; i < 3; i++)
            {
                switch (i)
                {
                    case 0:
                        RandomLine[i] = Random.Range(0, 6); // 0~5のランダムな数値を生成
                        break;
                    case 1:
                        do
                        {
                            RandomLine[i] = Random.Range(0, 6); // 0~5のランダムな数値を生成
                        } while (RandomLine[i] == RandomLine[0] || RandomLine[i] == RandomLine[2]);
                        break;
                    case 2:
                        do
                        {
                            RandomLine[i] = Random.Range(0, 6); // 0~5のランダムな数値を生成
                        } while (RandomLine[i] == RandomLine[0] || RandomLine[i] == RandomLine[1]);
                        break;
                }
            }

            // 通常用メッセージを取得、設定
            ButtonTxtUpdate(EmailFirstTxt, LineData[RandomLine[0]].text);
            ButtonTxtUpdate(EmailSecondTxt, LineData[RandomLine[1]].text);
            ButtonTxtUpdate(EmailThirdTxt, LineData[RandomLine[2]].text);
        }
    }

    /// <summary>
    /// メールイベント終了(距離の変化量をCallに渡して、初期化して、ターンを進めてステートをCall開始に変更、メールイベントを終了)
    /// </summary>
    private void EndSendEmailEvent()
    {
        // 距離の変化量をCallに渡す
        call.SetDistanceChange(ChangedDistance);
        IsGetClick = false; // クリックされたかどうかを初期化

        // 初期化
        CurrentLine = 0;
        ChangedDistance = 0;
        RandomLine[0] = 10;
        RandomLine[1] = 10;
        RandomLine[2] = 10;

        // メールイベント終了(ステートをCall開始に変更、メールイベントを終了)
        gamescenemanager.SetState(GameSceneManager.GameState.Call);
    }

    /// <summary>
    /// メッセージのテキスト更新
    /// </summary>
    /// <param name="message">変更内容</param>
    void MessageUpdate(string message)
    {
        textbox.text = message;
    }

    /// <summary>
    /// ボタンのテキスト更新
    /// </summary>
    /// <param name="button">ボタンの種類</param>
    /// <param name="message">変更内容</param>
    void ButtonTxtUpdate(TextMeshProUGUI button, string message)
    {
        button.text = message;
    }

    // 選択肢ボタンアクティブ
    public void SetMailButtomActiveOn()
    {
        EmailFirst.gameObject.SetActive(true);
        EmailSecond.gameObject.SetActive(true);
        EmailThird.gameObject.SetActive(true);
    }

    // 選択肢ボタン非アクティブ
    public void SetMailButtomActiveOff()
    {
        EmailFirst.gameObject.SetActive(false);
        EmailSecond.gameObject.SetActive(false);
        EmailThird.gameObject.SetActive(false);
    }
}
