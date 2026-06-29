using UnityEngine;
using UnityEngine.SceneManagement;//LoadSceneを使うため

public class GameManager : MonoBehaviour
{
    private float delay = 0.5f;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void GameClear()
    {
        //ゲームクリアシーンへ遷移
        SceneManager.LoadScene("GameClear");
    }
    public void DelayGameClear()
    {
        Invoke("GameClear", delay);
    }

    public void GameOver()
    {
        //ゲームオーバーシーンへ遷移
        SceneManager.LoadScene("GameOver");
    }
    public void DelayGameOver()
    {
        Invoke("GameOver", delay);
    }

    public void LoadTitleScene()
    {
        //タイトルシーンへ遷移
        SceneManager.LoadScene("Title");
    }
    public void DelayLoadTitleScene()
    {
        Invoke("LoadTitleScene", delay);
    }

    public void LoadGameScene()
    {
        //ゲームシーンへ遷移
        SceneManager.LoadScene("Game");
    }
    //少し遅らせてゲームシーンへ遷移する
    public void DelayLoadGameScene()
    {
        Invoke("LoadGameScene", delay);
    }
}
