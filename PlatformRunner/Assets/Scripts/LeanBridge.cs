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

    //Õ“ËŽž‚ÉŒÄ‚Î‚ê‚éŠÖ”
    private void OnCollisionEnter(Collision collision)
    {
        //Õ“Ë‚ªˆê’èˆÈã‚È‚ç‹´‚ð‰Ë‚¯‚é
        if(collision.impulse.magnitude>0.5f)
        {
            animator.SetTrigger("Lean");
        }
    }
}
