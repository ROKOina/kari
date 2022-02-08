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

    //�Փˎ��ɌĂ΂��֐�
    private void OnCollisionEnter(Collision collision)
    {
        //�Փ˂����ȏ�Ȃ狴���˂���
        if(collision.impulse.magnitude>0.5f)
        {
            animator.SetTrigger("Lean");
        }
    }
}
