using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BreakBox : MonoBehaviour
{
    [SerializeField]
    private float impulse = 4.0f;

    [SerializeField]
    private GameObject destroyBoxPrefab;

    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {

    }
    //箱の下側に当たった時の処理
    private void OnTriggerEnter(Collider other)
    {
        //プレイヤーに衝突した場合
        Player player = other.GetComponent<Player>();
        if (player != null)
        {
            //破壊箱生成
            GameObject destroyBox = Instantiate(destroyBoxPrefab, transform.position, transform.rotation);

            //破壊箱の子の全ての破片を列挙する
            foreach(Rigidbody body in destroyBox.GetComponentsInChildren<Rigidbody>())
            {
                //破片にランダムに衝撃を与える
                body.AddForce(
                    Random.Range(-impulse, impulse),
                    Random.Range(0.0f, impulse),
                    Random.Range(-impulse, impulse),
                    ForceMode.Impulse
                    );
            }

            //指定時間後に破壊箱を削除
            Destroy(destroyBox, 3.0f);

            //自分を削除
            Destroy(gameObject);
        }
    }
}
