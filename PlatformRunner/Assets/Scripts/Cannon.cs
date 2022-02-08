using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Cannon : MonoBehaviour
{
    [SerializeField]
    private Rigidbody cannonBallPrefab; //弾丸プレハブ

    [SerializeField]
    private Transform spawnPoint;   //射出位置

    [SerializeField]
    private Vector3 shotVelocity = new Vector3(-5.0f, 2.0f, 0.0f);  //射出ベクトル

    [SerializeField]
    private float shotTime = 2.0f;  //射出間隔

    private float timer;    //射出タイマー

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        //一定間隔毎に弾丸を射出する
        timer += Time.deltaTime;
        if(timer>=shotTime)
        {
            timer = 0.0f;

            //弾丸を射出位置に生成
            Rigidbody cannonBall = Instantiate(cannonBallPrefab, spawnPoint.position, spawnPoint.rotation);
            cannonBall.velocity = shotVelocity; //射出する方向と力を設定

            //射出してから指定時間後に破棄する
            Destroy(cannonBall.gameObject, 2.0f);
        }
    }
}
