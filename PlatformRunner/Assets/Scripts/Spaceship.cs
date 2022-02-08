using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Spaceship : MonoBehaviour
{
    [SerializeField]
    private float force = 1.0f;

    private float timer;

    private Rigidbody body;

    // Start is called before the first frame update
    void Start()
    {
        body = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        //上昇開始から一定時間後にゴール
        if(timer>0.0f)
        {
            timer -= Time.deltaTime;
            if(timer<0.0f)
            {
                Goal();
            }
        }
    }

    //固定時間毎更新処理
    private void FixedUpdate()
    {
        if (timer <= 0.0f) return;

        //上昇する力を加える
        body.AddForce(0, force * Time.deltaTime, 0);
    }

    //衝突判定処理
    private void OnTriggerEnter(Collider other)
    {
        //プレイヤーが当たったらゴール演出開始
        Player player = other.GetComponent<Player>();
        if(player!=null)
        {
            //Goal();
            //プレイヤーを非表示
            player.gameObject.SetActive(false);
            //上昇する時間を設定（上昇開始）
            timer = 5.0f;
        }
    }

    //ゴール処理
    private void Goal()
    {
        //現在のシーンを再ロード
        SceneManager.LoadScene(SceneManager.GetActiveScene().name);
    }
}
