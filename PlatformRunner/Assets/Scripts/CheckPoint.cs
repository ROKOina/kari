using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CheckPoint : MonoBehaviour
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
        //スポーンポイント更新
        Player player = other.GetComponent<Player>();
        if (player != null)
        {
            player.SetSpawnPoint(transform);
        }
    }
}
