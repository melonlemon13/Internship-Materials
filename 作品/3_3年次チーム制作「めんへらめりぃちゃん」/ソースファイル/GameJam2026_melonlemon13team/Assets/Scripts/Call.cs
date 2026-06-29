using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UI;//Sliderを使うため

public class Call : MonoBehaviour
{
    [SerializeField] GameSceneManager gameSceneManager;     // ゲームシーンマネージャーへの参照
    [SerializeField] Button AcceptButton;                   // 着信ボタン
    [SerializeField] Button TalkAcceptButton;               // 電話を受けるボタン
    [SerializeField] Button TalkRefuseButton;               // 電話を拒否するボタン
    [SerializeField] Button RecallPattern1;                 // 電話応答時の返答パターン1
    [SerializeField] Button RecallPattern2;                 // 電話応答時の返答パターン2
    [SerializeField] TextMeshProUGUI messageBox;            // 電話のメッセージボックス
    [SerializeField] TextMeshProUGUI TalkAcceptButtonText;  // 電話を受けるボタン
    [SerializeField] TextMeshProUGUI TalkRefuseButtonText;  // 電話を拒否するボタン
    [SerializeField] TextMeshProUGUI RecallPattern1Text;    // 電話応答時の返答パターン1
    [SerializeField] TextMeshProUGUI RecallPattern2Text;    // 電話応答時の返答パターン2

    private int currentTurn = 1; // 現在のターン数
    private bool isCallEventActive = false; // 電話イベントがアクティブかどうか

    public Slider MaryDistanceSlider; // メリーさんとの距離を表示するスライダー

    private int currentDistance = 99;       //現在のメリーさんとの距離
    private int DistanceChange = 0;         //距離の変化量
    private const int MaxDistance = 100;    //メリーさんとの最大距離//ここまで行くとメリーさんばいばい
    private const int MinDistance = 0;      //メリーさんとの最小距離//ここまで行くとゲームオーバー

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        // 全ボタン非アクティブ
        SetAllButtomActiveOff();

        MaryDistanceSlider.value = currentDistance; //現在値
        MaryDistanceSlider.maxValue = MaxDistance;  //最大値
        MaryDistanceSlider.minValue = MinDistance;  //最小値

        // 状態変更イベントを受け取る
        gameSceneManager.OnStateChanged += OnStateChanged;
    }

    // Update is called once per frame
    void Update()
    {
        if (currentDistance > 100)
        {
            currentDistance = 100;
        }
        if (currentDistance < 0)
        {
            currentDistance = 0;
        }
    }

    //ゲームの状態が変わったときに呼び出される関数
    void OnStateChanged(GameSceneManager.GameState state)
    {
        //ゲームの状態がCallのときに電話イベント開始
        if (state == GameSceneManager.GameState.Call && !isCallEventActive)
        {
            // 現在のターン数を取得
            currentTurn = gameSceneManager.GetCurrentTurn();

            if (currentTurn == 0 || currentTurn == 1 || currentTurn % 3 == 0)
            {
                // ターンに応じたメッセージを表示
                switch (currentTurn)
                {
                    case 0: // ターン外
                        MessageUpdate("ターン外を参照しています\n");
                        break;
                    case 1: // チュートリアル
                        MessageUpdate("めりぃちゃんから電話がかかってきたみたいです\nボタンを押して出てみましょう。\n");
                        break;
                    default:
                        MessageUpdate("めりぃちゃんから電話がかかってきた。\n");
                        break;
                }

                // 着信ボタンアクティブ
                AcceptButton.gameObject.SetActive(true);

                // 電話イベント開始
                isCallEventActive = true;
            }
            else
            {
                gameSceneManager.GoNextTurn();
            }
        }
    }

    public void StartCallEvent()
    {
        // 着信ボタン非アクティブ
        AcceptButton.gameObject.SetActive(false);


        if (currentTurn == 1)
        {
            MessageUpdate("めりぃちゃんと電話で話すことができました！\n返答の選択肢を選んでみましょう。\n");

            RectTransform acceptButtonRect = TalkAcceptButton.GetComponent<RectTransform>();

            acceptButtonRect.anchoredPosition = new Vector2(0, 70); // 画面中央に配置

            TalkAcceptButton.gameObject.SetActive(true);
            TalkAcceptButtonText.text = "話す";
        }
        else
        {
            MessageUpdate("めりぃちゃんから電話がかかってきた。\n");

            RectTransform acceptButtonRect = TalkAcceptButton.GetComponent<RectTransform>();

            acceptButtonRect.anchoredPosition = new Vector2(-300, 70); // 画面中央に配置

            SetCallButtomActiveOn();
        }
    }

    public void CallEvent(bool button)
    {
        // 応答/拒否ボタン非アクティブ
        SetCallButtomActiveOff();

        if (button)
        {
            // 応答の選択肢アクティブ
            SetRecallButtomActiveOn();
        }
        else
        {
            // 電話イベント終了
            EndCallEvent();
        }
    }

    public void AddCallEvent()
    {
        // 返答ボタン非アクティブ
        SetRecallButtomActiveOff();

        if (currentTurn == 1)
        {
            SetDistanceChange(-20);
        }
        else
        {
            SetDistanceChange(10);
        }

        // 終了処理
        EndCallEvent();
    }

    public void SubtractCallEvent()
    {
        // 返答ボタン非アクティブ
        SetRecallButtomActiveOff();

        if (currentTurn == 1)
        {
            // 誤った選択肢、距離ー
            SetDistanceChange(-20);
        }
        else
        {
            // 誤った選択肢、距離ー
            SetDistanceChange(-10);
        }

        // 終了処理
        EndCallEvent();
    }

    public void EndCallEvent()
    {
        // 距離の変化を適用
        currentDistance += DistanceChange;
        Debug.Log(DistanceChange);
        MaryDistanceSlider.value = currentDistance;
        DistanceChange = 0;

        // 電話イベント終了(ターンを進めてステートをターン開始に変更、電話イベントを終了)
        isCallEventActive = false;
        gameSceneManager.GoNextTurn();
    }

    // 現在距離取得
    public int GetCurrentDistance()
    {
        return currentDistance;
    }

    // 距離の変化量設定
    public void SetDistanceChange(int changedDistance)
    {
        DistanceChange += changedDistance;
    }

    // ↓以下基本機能

    // メッセージ更新
    private void MessageUpdate(string Message)
    {
        messageBox.text = Message;
    }

    // 応答/拒否ボタンアクティブ
    public void SetCallButtomActiveOn()
    {
        TalkAcceptButton.gameObject.SetActive(true);
        TalkAcceptButtonText.text = "話す";
        TalkRefuseButton.gameObject.SetActive(true);
        TalkRefuseButtonText.text = "話さない";
    }

    // 返答ボタンアクティブ
    public void SetRecallButtomActiveOn()
    {
        RecallPattern1.gameObject.SetActive(true);
        RecallPattern2.gameObject.SetActive(true);

        if (currentTurn == 0)
        {
            MessageUpdate("ターン外を参照しています\n");
        }
        else if (currentTurn == 1)
        {
            // 誤った選択肢、距離ー
            RecallPattern1Text.text = "会ってもやることなくない？\n";
            RecallPattern2Text.text = "そこまでして会いたい理由ある？\n";
        }
        else
        {
            int rdmNum1 = UnityEngine.Random.Range(1, 6);
            int rdmNum2 = UnityEngine.Random.Range(1, 6);

            switch (rdmNum1)
            {
                case 1:
                    // 正しい選択肢、距離＋
                    RecallPattern1Text.text = "会えるのは楽しみだけど、無理はしないで\n";
                    break;
                case 2:
                    // 正しい選択肢、距離＋
                    RecallPattern1Text.text = "ちゃんと聞いてるよ、でも落ち着いて話して\n";
                    break;
                case 3:
                    // 正しい選択肢、距離＋
                    RecallPattern1Text.text = "うん、いるよ。ちゃんとここにいるよ\n";
                    break;
                case 4:
                    // 正しい選択肢、距離＋
                    RecallPattern1Text.text = "来てくれるのは嬉しいけど、急がなくていいから\n";
                    break;
                case 5:
                    // 正しい選択肢、距離＋
                    RecallPattern1Text.text = "今は無理しないでいいからね\n";
                    break;
            }

            // 誤った選択肢、距離ー
            switch (rdmNum2)
            {
                case 1:
                    // 正しい選択肢、距離-
                    RecallPattern2Text.text = "合えるのは楽しみだし、来れる時でいいよ\n";
                    break;
                case 2:
                    // 正しい選択肢、距離-
                    RecallPattern2Text.text = "ちゃんと聞いてるよ、落ち着いたら来てね\n";
                    break;
                case 3:
                    // 正しい選択肢、距離-
                    RecallPattern2Text.text = "わかってるよ、ちゃんと聞こえてるから大丈夫\n";
                    break;
                case 4:
                    // 正しい選択肢、距離-
                    RecallPattern2Text.text = "来てくれるのは嬉しいし、ゆっくりでいいからね\n";
                    break;
                case 5:
                    // 正しい選択肢、距離-
                    RecallPattern2Text.text = "今どんな感じかだけ教えてくれる？\n";
                    break;
            }
        }
    }

    // 全てのボタンアクティブ
    public void SetAllButtomActiveOn()
    {
        AcceptButton.gameObject.SetActive(true);
        SetCallButtomActiveOn();
        SetRecallButtomActiveOn();
    }

    // 応答/拒否ボタン非アクティブ
    public void SetCallButtomActiveOff()
    {
        TalkAcceptButton.gameObject.SetActive(false);
        TalkRefuseButton.gameObject.SetActive(false);
    }

    // 返答ボタン非アクティブ
    public void SetRecallButtomActiveOff()
    {
        int rdmNum = UnityEngine.Random.Range(1, 8);

        switch (rdmNum)
        {
            case 1:
            case 3:
            case 6:
            case 7:
                RectTransform acceptBttRect1 = RecallPattern1.GetComponent<RectTransform>();
                acceptBttRect1.anchoredPosition = new Vector2(0, 250); // 画面中央に配置
                RectTransform acceptBttRect2 = RecallPattern2.GetComponent<RectTransform>();
                acceptBttRect2.anchoredPosition = new Vector2(0, 0); // 画面中央に配置
                break;
            case 2:
            case 4:
            case 5:
            case 8:
                RectTransform acceptBttRect3 = RecallPattern1.GetComponent<RectTransform>();
                acceptBttRect3.anchoredPosition = new Vector2(0, 0); // 画面中央に配置
                RectTransform acceptBttRect4 = RecallPattern2.GetComponent<RectTransform>();
                acceptBttRect4.anchoredPosition = new Vector2(0, 250); // 画面中央に配置
                break;
        }

        RecallPattern1.gameObject.SetActive(false);
        RecallPattern2.gameObject.SetActive(false);
    }

    // 全てのボタン非アクティブ
    public void SetAllButtomActiveOff()
    {
        AcceptButton.gameObject.SetActive(false);
        SetCallButtomActiveOff();
        SetRecallButtomActiveOff();
    }
}
