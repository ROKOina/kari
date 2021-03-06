using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(Animator))]
public class LeanBridge : MonoBehaviour
{
    private Animator animator;

    // Start is called before the first frame update
    void Start()
    {
        animator = GetComponent<Animator>();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    //衝突時に呼ばれる関数
    private void OnCollisionEnter(Collision collision)
    {
        //衝突が一定以上なら橋を架ける
        if(collision.impulse.magnitude>0.5f)
        {
            animator.SetTrigger("Lean");
        }
    }
}
