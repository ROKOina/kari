using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DeathTrigger : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void OnTriggerEnter(Collider other)
    {
        //プレイヤー死亡処理
        Player player = other.GetComponent<Player>();
        if(player!=null)
        {
            player.Death();
        }
    }

    private void OnCollisionEnter(Collision collision)
    {
        //プレイヤー死亡処理
        Player player = collision.gameObject.GetComponent<Player>();
        if(player!=null)
        {
            player.Death();
        }
    }
}
