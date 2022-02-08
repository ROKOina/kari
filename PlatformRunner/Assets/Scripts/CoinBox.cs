using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CoinBox : MonoBehaviour
{
    [SerializeField]
    private int itemCount = 5;

    [SerializeField]
    private GameObject coinPrefab;

    [SerializeField]
    private Transform appearPoint;

    [SerializeField]
    private float pushPower = 0.1f; //箱を叩く力

    [SerializeField]
    private float gravity = 1.0f;   //重力

    private Vector3 originalPosition;   //初期位置

    private Vector3 position;   //現在位置

    private bool isMoving;  //動いている状態か

    private float verticalSpeed;    //垂直移動速度

    [SerializeField]
    private Material emptyBoxMaterial; //マテリアル


    // Start is called before the first frame update
    void Start()
    {
        position = originalPosition = transform.position;
    }

    // Update is called once per frame
    void Update()
    {
        //箱が縦揺れする処理
        if(isMoving)
        {
            verticalSpeed -= gravity * Time.deltaTime;
            position.y += verticalSpeed;
            if(position.y<originalPosition.y)
            {
                position.y = originalPosition.y;
                isMoving = false;
            }
            transform.position = position;
        }
    }

    //箱の下側に当たった時の処理
    private void OnTriggerEnter(Collider other)
    {
        //プレイヤーに衝突した場合
        Player player = other.GetComponent<Player>();
        if(player!=null)
        {
            //コインが出現する回数を制限
            if(itemCount>0)
            {
                itemCount--;

                //空っぽのマテリアルに切り替え
                if(itemCount==0)
                {
                    MeshRenderer meshRenderer = GetComponent<MeshRenderer>();
                    meshRenderer.material = emptyBoxMaterial;
                }

                //箱の縦揺れ処理開始
                verticalSpeed = pushPower;
                isMoving = true;

                //コイン生成
                GameObject coin = Instantiate(coinPrefab, appearPoint.position, Quaternion.identity);
                Destroy(coin.gameObject, 0.5f); //生成後、指定時間後に削除
                player.AddCoin(1);  //コインを獲得

                //剛体コンポーネントを追加し、上方向に力を設定
                Rigidbody body = coin.AddComponent<Rigidbody>();
                body.velocity = new Vector3(0.0f, 6.0f, 0.0f);
            }
        }
    }
}
