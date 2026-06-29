using System.Collections.Generic;
using UnityEngine;

public class MapManager : MonoBehaviour
{
    [System.Serializable]//PlayerのMessageDateの定義
    public class MessageData
    {
        public string text;        //表示文章
        public int distanceDelta;  //距離増減

        public MessageData(string text, int distanceDelta)
        {
            this.text = text;
            this.distanceDelta = distanceDelta;
        }
    }

    private Dictionary<int, List<string>> maryTextMapData = new Dictionary<int, List<string>>();//メリーさんのテキストマップデータを格納する辞書
    private Dictionary<int, List<MessageData>> playerMessageData = new Dictionary<int, List<MessageData>>();//Playerのテキストマップデータを格納する辞書
    private Dictionary<int, List<string>> callMessageData = new Dictionary<int, List<string>>();//電話のテキストマップデータを格納する辞書

    //メリーさんとの距離の状態
    public enum DistanceState
    {
        //END = 0,//0%
        TENPERCENT = 10,//10%
        TWENTYPERCENT = 20,//20%
        THIRTYPERCENT = 30,//30%
        FORTYPERCENT = 40,//40%
        FIFTYPERCENT = 50,//50%
        SIXTYPERCENT = 60,//60%
        SEVENTYPERCENT = 70,//70%
        EIGHTYPERCENT = 80,//80%
        NINETYPERCENT = 90,//90%
        //CLEAR = 100//100%
    }//これを因数にしてmap内を検索する

    void Awake()
    {
        CreateMaryMap();
        CreatePlayerMap();
        CreateCallMap();
    }
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {

    }
    void CreateMaryMap()
    {
        maryTextMapData[0] = new List<string> { "私、めりぃちゃん。今、あなたの後ろにいるの。" };
        maryTextMapData[10] = new List<string> { "私、めりぃちゃん。今、ベランダにいるの。",
            "私、めりぃちゃん。今、カーテン閉めたの見えてるよ。",
            "私、めりぃちゃん。今、お風呂場にいるの。" };
        maryTextMapData[20] = new List<string> { "私、めりぃちゃん。今、あなたの階層に向かっているの。",
            "私、めりぃちゃん。今、階段を登っているの。",
            "私、めりぃちゃん。今、エレベーターの中にいるの。" };
        maryTextMapData[30] = new List<string> { "私、めりぃちゃん。今、ゴミ置き場にいるの。",
            "私、めりぃちゃん。今、駐輪場にいるの。",
            "私、めりぃちゃん。今、駐輪場にいるの。" };
        maryTextMapData[40] = new List<string> { "私、めりぃちゃん。今、あなたの最寄駅にいるの。",
            "私、めりぃちゃん。今、あなたの近くのコンビニにいるの。",
            "私、めりぃちゃん。今、あなたの近くの公園にいるの。" };
        maryTextMapData[50] = new List<string> { "私、めりぃちゃん。今、公衆電話にいるの。",
            "私、めりぃちゃん。今、交差点にいるの。",
            "私、めりぃちゃん。今、郵便局にいるの。" };
        maryTextMapData[60] = new List<string> { "私、めりぃちゃん。今、ゴミ捨て場にいるの。",
            "私、めりぃちゃん。今、ゴミ収集車にいるの。",
            "私、めりぃちゃん。ここちょっと臭うね。" };
        maryTextMapData[70] = new List<string> { "私、めりぃちゃん。今、ショッピングモールにいるの。",
            "私、めりぃちゃん。今、商店街にいるの。",
            "私、めりぃちゃん。今、大通りに出たの。" };
        maryTextMapData[80] = new List<string> { "私、めりぃちゃん。今、あなたのとなり駅にいるの。",
            "私、めりぃちゃん。今、あなたのとなり町にいるの。",
            "私、めりぃちゃん。今、近くのエリアにいるの。" };
        maryTextMapData[90] = new List<string> { "私、めりぃちゃん。今、電車の中にいるの。",
            "私、めりぃちゃん。今、トンネルを抜けたところなの。",
            "私、めりぃちゃん。今、橋を渡っているの。" };
        maryTextMapData[100] = new List<string> { "私・・めりぃ・・。い・・マ森・・ちカ・・いる・。",
        "わ・・めりぃ・・ま・・とお・・・ケど・・。",
        "わた・・め・・ちゃん。今・・そっチ・・くね。"};
        maryTextMapData[101] = new List<string> { "音信不通" };
    }

    void CreatePlayerMap()
    {
        //1行目
        //playerMessageData[1][0].text
        playerMessageData[1] = new List<MessageData>
        {
            new MessageData("おい！。", -4),
            new MessageData("ちょっと聞いて。", -3),
            new MessageData("やほ。", -2),
            new MessageData("やっほー。", 2),
            new MessageData("やっほ、元気？", 3),
            new MessageData("今日もお疲れさま！。", 4),
            new MessageData("ごめん。", -10),
            new MessageData("申し訳ねぇ。", -10),
            new MessageData("マジごめん！。", -10),
        };
        //2行目
        playerMessageData[2] = new List<MessageData>
        {
            new MessageData("今日はリカちゃんとご飯を食べに行ったんだ！。", -4),
            new MessageData("今家。", -3),
            new MessageData("今日はショッピングモールに遊びに行ったんだ！。", -2),
            new MessageData("今日はいい天気だったね！", 2),
            new MessageData("さっきまでお風呂に入ってたんだ！。", 3),
            new MessageData("今テレビを見てるんだ！",4),
            new MessageData("同級会には行けません。",-10),
            new MessageData("今日の予定完っ全に忘れてた。",-10),
            new MessageData("今日は友達と遊びに行ってたんだー。",-10),
        };
        //3行目
        playerMessageData[3] = new List<MessageData>
        {
            new MessageData("じゃ。", -4),
            new MessageData("また暇なときにでも連絡するねー。", -3),
            new MessageData("何かあったら連絡してね。", -2),
            new MessageData("また連絡するね。", 2),
            new MessageData("またゆっくり話そうね。", 3),
            new MessageData("また明日も連絡するね。", 4),
            new MessageData("いま、シンガポールにいます。", -10),
            new MessageData("許してくれないかな？", -10),
            new MessageData("今日会うのは無理かもー", -10),
        };
    }

    void CreateCallMap()
    {
        callMessageData[10] = new List<string>
        {
            "会えるのは楽しみだけど、無理はしないで",
            "ちゃんと聞いてるよ、でも落ち着いて話して",
            "うん、いるよ。ちゃんとここにいるよ",
            "来てくれるのは嬉しいけど、急がなくていいから",
            "今は無理しないでいいからね"
        };
        callMessageData[-10] = new List<string>
        {
            "合えるのは楽しみだし、来れる時でいいよ",
            "ちゃんと聞いてるよ、落ち着いたら来てね",
            "わかってるよ、ちゃんと聞こえてるから大丈夫",
            "来てくれるのは嬉しいし、ゆっくりでいいからね",
            "今どんな感じかだけ教えてくれる？"
        };
        callMessageData[-20] = new List<string>
        {
            "そこまでして会いたい理由ある？",
            "今は来なくていいから、ゆっくりしてて"
        };
    }

    //メリーさんから送られてくるメールのテキストのリストを取得
    public List<string> GetMaryTexts(int key)
    {
        if (maryTextMapData.ContainsKey(key))
        {
            return maryTextMapData[key];
        }

        return null;
    }

    //Playerがメリーさんに送るメールのテキストのリストを取得
    public List<MessageData> GetPlayerMessages(int key)
    {
        if (playerMessageData.ContainsKey(key))
        {
            return playerMessageData[key];
        }

        return null;
    }

    //電話の返信用テキストのリストを取得
    public List<string> GetCallMessages(int key)
    {
        if (callMessageData.ContainsKey(key))
        {
            return callMessageData[key];
        }

        return null;
    }

}
