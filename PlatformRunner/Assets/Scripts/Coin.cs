using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Coin : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        //Y軸回転（１秒間に１８０度回転する）
        transform.Rotate(0.0f, 180 * Time.deltaTime, 0.0f);
    }

    //衝突した時に呼ばれる関数
    private void OnTriggerEnter(Collider other)
    {
        //プレイヤーと衝突した時
        Player player = other.GetComponent<Player>();
        if(player!=null)
        {
            //コイン獲得
            player.AddCoin(1);

            //自信を破棄する
            Destroy(gameObject);
        }
        
    }
}
