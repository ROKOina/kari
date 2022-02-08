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

    //���[�v����R���C�_�[�ɏՓ˂������ɌĂ΂��
    private void OnTriggerEnter(Collider other)
    {
        //���[�v�悪�ݒ肳��Ă��Ȃ��ꍇ�͖���
        if (outPoint == null) return;

        //�v���C���[�ɏՓ˂����烏�[�v����
        Player player = other.GetComponent<Player>();
        if(player!=null)
        {
            //�v���C���[�̈ʒu�����[�v��ɐݒ�
            player.Warp(outPoint.position);
        }
    }
}
