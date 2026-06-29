using TMPro;
using UnityEngine;
using UnityEngine.UI;//ボタンを使うため
using System;//randomを使うため

public class MaryEmail : MonoBehaviour
{
    [SerializeField] GameSceneManager gamescenemanager;//GameSceneManager、Inspectorで設定
    [SerializeField] TextMeshProUGUI textbox;//textboxを取得、Inspectorで設定

    [SerializeField] GameObject mailPopupButton;//メール受信のポップアップを取得、Inspectorで設定

    [SerializeField] MapManager mapmanager;//MapManagerを取得//Inspectorで設定
    [SerializeField] Call call;//Callスクリプトを取得

    [SerializeField] InputManager inputmanager;//InputManagerを取得/Inspectorで設定

    private int currentTurn = 0;//現在のターン数
    private int mapKey; //MapManagerのマップを検索するためのキー
    private int rndtextkey;//ランダムにテキストを選ぶためのキー

    bool isMakeKeys = false;//キーが作られたかどうか

    private int currentDistance = 55; //現在のメリーさんとの距離

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        gamescenemanager.OnStateChanged += OnStateChanged;
        currentTurn = 1;
        isMakeKeys = false;
        mailPopupButton.SetActive(true);//メール受信のポップアップを表示
        textbox.text = "めりぃちゃんからメールが届いた\n開いてみよう";//テキストを変更
    }

    // Update is called once per frame
    void Update()
    {
        if (gamescenemanager.GetState() != GameSceneManager.GameState.GetEmail)
            return;
        //テキスト表示処理
        if (isMakeKeys == false)//キーが作られていないなら
        {
            currentDistance = call.GetCurrentDistance();//Callスクリプトから現在の距離を取得
            MakeMapKey(currentDistance);//現在の距離からマップのキーを作成
            System.Random rnd = new System.Random();    // Randomオブジェクトの作成
            rndtextkey = rnd.Next(0, 3);  //0以上3未満の値がランダムに出力
            isMakeKeys = true;
        }

        if (mapKey > 100)
        {
            mapKey = 101;
            rndtextkey = 0;
        }
        if(mapKey <= 0)
        {
            mapKey = 0;
            rndtextkey = 0;
        }

        textbox.text = mapmanager.GetMaryTexts(mapKey)[rndtextkey];//マップからテキストを取得、テキストを表示
        //もしマウス左クリックが押されたら
        if (inputmanager.GetMouseLeftTrigger() == true)
        {
            isMakeKeys = false;//キーを作り直すためにfalseにする
            gamescenemanager.SetState(GameSceneManager.GameState.SendEmail);//SendEmailの状態に変更
        }
    }

    public void NextTurn()
    {
        currentTurn++;
    }

    //現在距離によりキー値を作成
    private void MakeMapKey(int currentdistance)
    {
        if(currentdistance == 0)//0なら0
        {
            //ここは呼ばれないはず
            mapKey = 0;
        }
        else if(currentdistance >= 1 && currentdistance < 10)//1以上10未満なら10
        {
            mapKey = 10;
        }
        else if (currentdistance >= 11 && currentdistance < 20)//11以上20未満なら20
        {
            mapKey = 20;
        }
        else if (currentdistance >= 21 && currentdistance < 30)//21以上30未満なら30
        {
            mapKey = 30;
        }
        else if (currentdistance >= 31 && currentdistance < 40)//31以上40未満なら40
        {
            mapKey = 40;
        }
        else if (currentdistance >= 41 && currentdistance < 50)//41以上50未満なら50
        {
            mapKey = 50;
        }
        else if (currentdistance >= 51 && currentdistance < 60)//51以上60未満なら60
        {
            mapKey = 60;
        }
        else if (currentdistance >= 61 && currentdistance < 70)//61以上70未満なら70
        {
            mapKey = 70;
        }
        else if (currentdistance >= 71 && currentdistance < 80)//71以上80未満なら80
        {
            mapKey = 80;
        }
        else if (currentdistance >= 81 && currentdistance < 90)//81以上90未満なら90
        {
            mapKey = 90;
        }
        else if(currentdistance >= 91 && currentdistance < 100)//91以上100未満なら100
        {
            //ここは呼ばれないはず
            mapKey = 100;
        }
    }

    //ゲームの状態が変わったときに呼び出される関数
    void OnStateChanged(GameSceneManager.GameState state)
    {
        if (state == GameSceneManager.GameState.Start)
        {
            mailPopupButton.SetActive(true);//メール受信のポップアップを表示
            textbox.text = "めりぃちゃんからメールが届いた\n開いてみよう";//テキストを変更
        }
        else　if (state == GameSceneManager.GameState.GetEmail)
        {
            mailPopupButton.SetActive(false);
        }
    }
}