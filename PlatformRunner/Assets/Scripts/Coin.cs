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
        //Y²‰ñ“]i‚P•bŠÔ‚É‚P‚W‚O“x‰ñ“]‚·‚éj
        transform.Rotate(0.0f, 180 * Time.deltaTime, 0.0f);
    }

    //Õ“Ë‚µ‚½‚ÉŒÄ‚Î‚ê‚éŠÖ”
    private void OnTriggerEnter(Collider other)
    {
        //ƒvƒŒƒCƒ„[‚ÆÕ“Ë‚µ‚½
        Player player = other.GetComponent<Player>();
        if(player!=null)
        {
            //ƒRƒCƒ“Šl“¾
            player.AddCoin(1);

            //©M‚ğ”jŠü‚·‚é
            Destroy(gameObject);
        }
        
    }
}
