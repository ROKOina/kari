using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraController : MonoBehaviour
{
    [SerializeField]
    private Transform target;

    [SerializeField]
    private float smoothnedd = 0.1f;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if(target!=null)
        {
            //�ڕW�n�_�֕⊮���Ȃ���ړ�
            transform.position = Vector3.Lerp(transform.position, target.position, smoothnedd);

            //�΂ߏ�ɂ��邽�߂ɍ����p�́@Quaternion�@���g��
            Quaternion q= Quaternion.Euler(0f, 30.0f, 0f);

            //�L�����̌������ނ�
            //�v���C���[�E
            if (transform.rotation.y < target.rotation.y+q.y)
            {
                transform.Rotate(new Vector3(0, 1, 0), 0.5f);
            }
            //�v���C���[��
            if (transform.rotation.y > target.rotation.y - q.y)
            {
                transform.Rotate(new Vector3(0, 1, 0), -0.5f);
            }
        }
    }
}
