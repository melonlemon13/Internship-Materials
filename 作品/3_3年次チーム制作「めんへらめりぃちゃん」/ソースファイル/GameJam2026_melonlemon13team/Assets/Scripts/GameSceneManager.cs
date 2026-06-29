using System;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.Audio;

public class GameSceneManager : MonoBehaviour
{
    public GameState currentState = GameState.New;//現在のゲーム状態
    private int currentTurn = 1;//現在のターン数
    // 状態変更イベント
    public event Action<GameState> OnStateChanged;
    [SerializeField] InputManager inputmanager;//InputManagerを取得/Inspectorで設定
    public AudioClip click;//クリックSE
    AudioSource audioSource;//AudioSourceを取得
    [SerializeField] Call call;//Callクラスを取得/Inspectorで設定
    [SerializeField] GameManager gamemanager;//GameManager、Inspectorで設定

    //ゲームの状態
    public enum GameState
    {
        New,      //ゲーム開始
        Start,　　//ターン開始//Indexの更新//クリックするとメールが送られてくる//currentTurnが1ならチュートリアル
        //画面中央にメール受信のポップアップ、それをクリックするとGetEmailへ
        GetEmail, //メリーさんからメールが送られてくる//currentTurnが1ならチュートリアル
        SendEmail,//メリーさんにメールを送る//currentTurnが1ならチュートリアル
        Call,     //メリーさんから電話がかかってくる//currentTurnが1ならチュートリアル
    }

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        //Playを開始したよ
        //currentState = GameState.New;
        currentState = GameState.New;
        currentTurn = 1;
        //状態変更を通知
        OnStateChanged?.Invoke(currentState);

        audioSource = GetComponent<AudioSource>();//AudioSourceを取得
    }

    // Update is called once per frame
    void Update()
    {
        Debug.Log("Current State: " + currentState); // 現在の状態をログに出力
        //距離が100になったらゲームオーバー
        if (currentState == GameState.Start && call.GetCurrentDistance() >= 100)
        {
            gamemanager.DelayGameClear(); 
            return;
        }

        if(currentState == GameState.Start && call.GetCurrentDistance() <= 0)
        {
            gamemanager.DelayGameOver();
            return;
        }
            //もしマウス左クリックが押されたら
            if (inputmanager.GetMouseLeftTrigger() == true)
        {
            //クリック音再生
            audioSource.PlayOneShot(click);
        }
    }

    //Stateをセット
    public void SetState(GameState state)
    {
        currentState = state;
        OnStateChanged?.Invoke(currentState);//状態変更を通知
    }

    //現在のStateを取得
    public GameState GetState()
    {
        return currentState;
    }

    public int GetCurrentTurn()
    {
        return currentTurn;
    }

    public void GoNextTurn()
    {
        currentTurn++;
        SetState(GameState.Start);
    }

    public void SetGetEmail()
    {
        SetState(GameState.GetEmail);
    }
}
