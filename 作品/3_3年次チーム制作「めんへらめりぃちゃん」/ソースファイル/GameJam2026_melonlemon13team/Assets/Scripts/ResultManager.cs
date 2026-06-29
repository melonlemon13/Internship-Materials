using UnityEngine;
using static Unity.Collections.AllocatorManager;
using UnityEngine.Audio;


public class GameClearManager : MonoBehaviour
{
    [SerializeField] InputManager inputmanager;//InputManagerを取得/Inspectorで設定
    [SerializeField] GameObject LoadSceneButton;//ボタンを取得、Inspectorで設定
    public AudioClip click;//クリックSE
    AudioSource audioSource;//AudioSourceを取得
    bool isTitleButtonActive = false;//タイトルボタンがアクティブかどうか
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        audioSource = GetComponent<AudioSource>();//AudioSourceを取得
        isTitleButtonActive = false;
        LoadSceneButton.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {
        //もしマウス左クリックが押された&&タイトルボタンがアクティブでないなら
        if (inputmanager.GetMouseLeftTrigger() == true && isTitleButtonActive == false)
        {
            audioSource.PlayOneShot(click);
            isTitleButtonActive = true;
            LoadSceneButton.SetActive(true);
        }
    }
}
