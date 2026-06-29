using System.Collections;
using UnityEngine;
using UnityEngine.UI;
public class SellectBotton : MonoBehaviour
{
    public Image targetImage;//点滅させる画像
    [SerializeField] private float interval = 0.1f;//間隔
    [SerializeField] private int blinkcount = 3;//点滅の回数
    [SerializeField] private float Amount = 0.2f;//どれくらい色を変化させるか
   


    // Update is called once per frame
    void Update()
    {
        
    }

    public void ChangeColor()
    {

        StartCoroutine(BlinkCoroutine());
    }

    IEnumerator BlinkCoroutine()
    {
        Color currentcolor = targetImage.color;//変更前の色
        Color afterclor;//変更後の色

        //点滅色を設定する
        afterclor.r = currentcolor.r - Amount; // 点滅色
        afterclor.g = currentcolor.g - Amount;// 点滅色
        afterclor.b = currentcolor.b - Amount; // 点滅色

        //ここで点滅ループ
        for (int i = 0; i < blinkcount; i++)
        {

            targetImage.color = new Color(afterclor.r, afterclor.g, afterclor.b);//点滅色
            yield return new WaitForSeconds(interval);

            targetImage.color = new Color(currentcolor.r, currentcolor.g, currentcolor.b);// 元の色
            yield return new WaitForSeconds(interval);
        }
    }

}
