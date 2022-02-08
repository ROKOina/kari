using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Warp : MonoBehaviour
{
    [SerializeField]
    private Transform outPoint;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    //ワープ判定コライダーに衝突した時に呼ばれる
    private void OnTriggerEnter(Collider other)
    {
        //ワープ先が設定されていない場合は無視
        if (outPoint == null) return;

        //プレイヤーに衝突したらワープする
        Player player = other.GetComponent<Player>();
        if(player!=null)
        {
            //プレイヤーの位置をワープ先に設定
            player.Warp(outPoint.position);
        }
    }
}
